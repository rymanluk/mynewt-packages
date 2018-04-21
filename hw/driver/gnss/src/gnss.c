#include <stdbool.h>
#include <gnss/gnss.h>


bool
gnss_decoder(struct gnss_decoder_context *ctx, uint8_t byte)
{
    /* Dont process if we've got an error event pending
     *  => usually means baud rate need to be fixed
     */
    if (ctx->error_event_pending) {
	return false; /* Stop reception */
    }    

    return gnss_nmea_decoder(ctx, byte);

}
