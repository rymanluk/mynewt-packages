#ifndef _GNSS_MYNEWT_H_
#define _GNSS_MYNEWT_H_

#include <os/os.h>
#include <gnss/types.h>



void gnss_os_init(void);
void gnss_os_decoder_init(gnss_decoder_t *ctx);


void gnss_os_emit_error_event(gnss_decoder_t *ctx, unsigned int error);


void gnss_os_emit_gnss_event(gnss_decoder_t *ctx);


void gnss_internal_evq_set(struct os_eventq *evq);




gnss_event_t *gnss_os_gnss_event_alloc(gnss_decoder_t *ctx);

#endif
