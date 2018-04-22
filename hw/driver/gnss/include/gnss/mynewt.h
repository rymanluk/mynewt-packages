#ifndef _GNSS_MYNEWT_H_
#define _GNSS_MYNEWT_H_

#include <os/os.h>
#include <gnss/types.h>

#include <stdio.h>

void gnss_os_init(void);
void gnss_os_decoder_init(gnss_decoder_t *ctx);


gnss_event_t *gnss_os_fetch_gnss_event(gnss_decoder_t *ctx);
void gnss_os_emit_gnss_event(gnss_decoder_t *ctx);

void gnss_os_emit_error_event(gnss_decoder_t *ctx, unsigned int error);


void gnss_internal_evq_set(struct os_eventq *evq);

#define gnss_os_printf printf


#endif
