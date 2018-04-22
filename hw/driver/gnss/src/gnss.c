#include <stdbool.h>
#include <string.h>
#include <gnss/gnss.h>

#include <gnss/mynewt.h>


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
    ctx->decoder = decoder;
    ctx->callback = callback;
    ctx->error_callback = error_callback;
    gnss_os_decoder_init(ctx);
}
