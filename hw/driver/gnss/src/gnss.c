#include <stdbool.h>
#include <string.h>
#include <gnss/gnss.h>

#include <gnss/mynewt.h>
#include <uart/uart.h>

#include <console/console.h>


#include <log/log.h>

struct log _gnss_log;


static struct os_eventq *_gnss_internal_evq = NULL;

static struct os_mempool gnss_pool;

static os_membuf_t gnss_memory_buffer[
	      OS_MEMPOOL_SIZE(GNSS_EVENT_MAX, sizeof(gnss_event_t))];



int
gnss_uart_rx_char(void *arg, uint8_t byte)
{
    //console_out(byte);
    //return 1;
    gnss_decoder_t *ctx = (gnss_decoder_t *)arg;
    return gnss_decoder(ctx, byte) ? 1 : 0;
}

int
gnss_uart_tx_char(void *arg)
{
    gnss_decoder_t *ctx = (gnss_decoder_t *)arg;
    if ((ctx->tx_string == NULL) || (*ctx->tx_string == '\0')) {
	return -1;
    }
    

    //console_out(*ctx->tx_string);

    return *ctx->tx_string++;
}

void
gnss_uart_tx_done(void *arg)
{
    gnss_decoder_t *ctx = (gnss_decoder_t *)arg;
    os_sem_release(&ctx->tx_sem);
}



void
gnss_internal_evq_set(struct os_eventq *evq)
{
    _gnss_internal_evq = evq;
}



static void
gnss_os_gnss_event_cb(struct os_event *ev)
{
    gnss_event_t   *gnss_event = (gnss_event_t   *) ev;
    gnss_decoder_t *ctx        = (gnss_decoder_t *) ev->ev_arg;

    switch(ctx->decoder) {
    case GNSS_DECODER_NMEA:
	gnss_nmea_log(&gnss_event->nmea);
	break;
    }

    ctx->callback(ctx->decoder, gnss_event);
    
    os_memblock_put(&gnss_pool, ev);
}

static void
gnss_os_error_event_cb(struct os_event *ev)
{
    gnss_decoder_t *ctx = (gnss_decoder_t *) ev;
    ctx->error_callback(ctx, ctx->error);
    ctx->error = GNSS_ERROR_NONE;
}






void
gnss_os_emit_error_event(gnss_decoder_t *ctx, unsigned int error)
{
    os_eventq_put(_gnss_internal_evq, &ctx->error_event);
}

void
gnss_os_emit_gnss_event(gnss_decoder_t *ctx)
{
    os_eventq_put(_gnss_internal_evq, &ctx->gnss_event->event);
}


gnss_event_t *
gnss_os_fetch_gnss_event(gnss_decoder_t *ctx)
{
    gnss_event_t *evt = os_memblock_get(&gnss_pool);
    evt->event = (struct os_event) {
	.ev_cb  = gnss_os_gnss_event_cb,
	.ev_arg = ctx,
    };
    return evt;
}

bool
gnss_send_cmd(gnss_decoder_t *ctx, char *cmd)
{
    /* Generate crc string */
    char crc[3];
    snprintf(crc, sizeof(crc), "%02x", gnss_nmea_crc(cmd));

    char *msg[] = { "$", cmd, "*", crc, "\r\n" };
    for (int i = 0 ; i < sizeof(msg) / sizeof(*msg) ; i++) {
	os_sem_pend(&ctx->tx_sem, OS_TIMEOUT_NEVER);
	ctx->tx_string = msg[i];
	uart_start_tx(ctx->uart_dev);
    }
    os_time_delay(10);

    return true;
}



bool
gnss_decoder(gnss_decoder_t *ctx, uint8_t byte)
{
    /* Dont process if we've got an error event pending
     *  => usually means baud rate need to be fixed
     */
    if (ctx->error) {
	return false; /* Stop reception */
    }    

    switch(ctx->decoder) {
    case GNSS_DECODER_NMEA:
	return gnss_nmea_decoder(ctx, byte);
    default:
	assert(0);
	return false;
    }
}

void
gnss_init(void)
{
    log_register("gnss", &_gnss_log,
		 &log_console_handler, NULL, LOG_LEVEL_DEBUG);
    int rc;
    rc = os_mempool_init(&gnss_pool, GNSS_EVENT_MAX, sizeof(gnss_event_t),
			 gnss_memory_buffer, "gnss_pool");
    assert(rc == 0);

    _gnss_internal_evq = os_eventq_dflt_get();

}

void
gnss_decoder_init(gnss_decoder_t *ctx, int decoder,
		  gnss_callback_t callback,
		  gnss_error_callback_t error_callback)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->tx_string = NULL;
    ctx->decoder = decoder;
    ctx->callback = callback;
    ctx->error_callback = error_callback;
    os_sem_init(&ctx->tx_sem, 1);

    ctx->error_event.ev_cb  = gnss_os_error_event_cb;
    ctx->error_event.ev_arg = ctx;

}
