#include <stdbool.h>
#include <string.h>
#include <gnss/gnss.h>

#include <gnss/mynewt.h>
#include <uart/uart.h>

#include <console/console.h>

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

//    gnss_send_cmd(g, "PGCMD,380,7");        // SDK Mode
//    gnss_send_cmd(g, "PGCMD,229,1,1,0,1");  // GPS + GLONAS + Galileo
//    gnss_send_cmd(g, "PGCMD,218,1");        // NMEA Sentence
//    gnss_send_cmd(g, "PMTK104");            // Full cold start


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
    gnss_os_init();
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

    gnss_os_decoder_init(ctx);
}
