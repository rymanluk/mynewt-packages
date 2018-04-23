#include <stdbool.h>
#include <string.h>
#include <gnss/gnss.h>
#include <stdio.h>

#include <uart/uart.h>
#include <gnss/uart.h>

#include <log/log.h>

struct log _gnss_log;


static struct os_eventq *_gnss_evq = NULL;

static struct os_mempool _gnss_event_pool;
static os_membuf_t gnss_event_buffer[
	     OS_MEMPOOL_SIZE(MYNEWT_VAL(GNSS_EVENT_MAX),
			     sizeof(gnss_event_t))];


#if MYNEWT_VAL(GNSS_NMEA_EVENT_MAX) > 0
static struct os_mempool _gnss_nmea_event_pool;
static os_membuf_t gnss_nmea_event_buffer[
	     OS_MEMPOOL_SIZE(MYNEWT_VAL(GNSS_NMEA_EVENT_MAX),
			     sizeof(gnss_nmea_raw_event_t))];
#endif



#define GNSS_MS_TO_TICKS(ms) (((ms) * OS_TICKS_PER_SEC + 999) / 1000)






void
gnss_evq_set(struct os_eventq *evq)
{
    _gnss_evq = evq;
}



static void
gnss_event_cb(struct os_event *ev)
{
    gnss_event_t   *gnss_event = (gnss_event_t   *) ev;
    gnss_t *ctx        = (gnss_t *) ev->ev_arg;

    /* Perform logging */
#if MYNEWT_VAL(GNSS_LOG) > 0
    switch(gnss_event->type) {
#if MYNEWT_VAL(GNSS_NMEA_LOG) > 0
    case GNSS_EVENT_NMEA:
	gnss_nmea_log(&gnss_event->nmea);
	break;
#endif
    }
#endif

    /* Trigger user callback */
    if (ctx->callback) {
	ctx->callback(gnss_event->type, gnss_event);
    }

    /* Put event back to the memory block */
    os_memblock_put(&_gnss_event_pool, ev);
}

static void
gnss_error_event_cb(struct os_event *ev)
{
    gnss_t *ctx = (gnss_t *) ev;

    /* Trigger user callback */
    if (ctx->error_callback)
	ctx->error_callback(ctx, ctx->error);

    /* Clear error */
    ctx->error = GNSS_ERROR_NONE;
}






void
gnss_os_emit_error_event(gnss_t *ctx, unsigned int error)
{
    os_eventq_put(_gnss_evq, &ctx->error_event);
}

void
gnss_os_emit_gnss_event(gnss_t *ctx)
{
    os_eventq_put(_gnss_evq, &ctx->gnss_event->event);
}


gnss_event_t *
gnss_os_fetch_gnss_event(gnss_t *ctx)
{
    gnss_event_t *evt = os_memblock_get(&_gnss_event_pool);
    evt->event = (struct os_event) {
	.ev_cb  = gnss_event_cb,
	.ev_arg = ctx,
    };
    return evt;
}

bool
gnss_nmea_send_cmd(gnss_t *ctx, char *cmd)
{
    char  crc[3];
    char *msg[5];
    int   i;

    /* Generating crc string */
    snprintf(crc, sizeof(crc), "%02X", gnss_nmea_crc(cmd));

    /* Building message array */
    msg[0] = "$";
    msg[1] = cmd;
    msg[2] = "*";
    msg[3] = crc;
    msg[4] = "\r\n";

    /* Sending command */
    LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT, "Command: $%s*%s\n", cmd, crc);
    
    for (i = 0 ; i < sizeof(msg) / sizeof(*msg) ; i++) {
	gnss_uart_send(ctx, (uint8_t*)msg[i], strlen(msg[1]));
    }

    /* Ensure a 10 ms delay */
    os_time_delay(GNSS_MS_TO_TICKS(10));

    return true;
}


void
gnss_init(void)
{
    int rc;
    
    /* Set default event queue */
    _gnss_evq = os_eventq_dflt_get();

    /* Register logger */
    rc = log_register("gnss", &_gnss_log,
		      &log_console_handler, NULL, LOG_LEVEL_DEBUG);
    assert(rc == 0);

    /* Initialise memory pool */
    rc = os_mempool_init(&_gnss_event_pool,
			 MYNEWT_VAL(GNSS_EVENT_MAX),
			 sizeof(gnss_event_t),
			 gnss_event_buffer,
			 "gnss_evt_pool");
    assert(rc == 0);

#if MYNEWT_VAL(GNSS_NMEA_EVENT_MAX) > 0
    rc = os_mempool_init(&_gnss_nmea_event_pool,
			 MYNEWT_VAL(GNSS_NMEA_EVENT_MAX),
			 sizeof(gnss_nmea_raw_event_t),
			 gnss_nmea_event_buffer,
			 "gnss_nmea_evt_pool");
    assert(rc == 0);
#endif
}

void
gnss_decoder_init(gnss_t *ctx,
		  gnss_callback_t callback,
		  gnss_error_callback_t error_callback)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->callback           = callback;
    ctx->error_callback     = error_callback;

    ctx->error_event.ev_cb  = gnss_error_event_cb;
    ctx->error_event.ev_arg = ctx;

}
