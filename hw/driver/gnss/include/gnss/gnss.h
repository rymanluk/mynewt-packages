#ifndef _GNSS_H_
#define _GNSS_H_

#include <gnss/nmea.h>
#include <gnss/mediatek.h>
#include <gnss/types.h>

extern struct log _gnss_log;

#define GNSS_ERROR_NONE				0
#define GNSS_ERROR_WRONG_BAUD_RATE		1


#define GNSS_EVENT_UNKNOWN			0
#define GNSS_EVENT_NMEA				1






typedef void (*gnss_callback_t)(int type, gnss_event_t *event);
typedef void (*gnss_error_callback_t)(gnss_t *ctx, int error);


typedef bool (*gnss_decoder_t)(gnss_t *ctx, uint8_t byte);

typedef bool (*gnss_start_rx_t)(gnss_t *ctx);
typedef bool (*gnss_stop_rx_t)(gnss_t *ctx);
typedef int  (*gnss_send_t)(gnss_t *ctx, uint8_t *bytes, uint16_t size);

typedef bool (*gnss_standby_t)(gnss_t *ctx, int level);
typedef bool (*gnss_wakeup_t)(gnss_t *ctx);
typedef bool (*gnss_reset_t)(gnss_t *ctx, int type);





struct gnss_event {
    struct os_event event;
    uint8_t type;
    union {
	struct gnss_nmea nmea;
    };
};




struct gnss_decoder {
    struct {
	struct gnss_uart *conf;
	gnss_start_rx_t   start_rx;
	gnss_stop_rx_t    stop_rx;
	gnss_send_t       send;
    } transport;

    struct {
	struct gnss_nmea_decoder *conf;
	gnss_decoder_t    decoder;
    } protocol;
    
    int error;
    struct os_event error_event;
    gnss_error_callback_t error_callback;

    struct gnss_event *gnss_event;
    gnss_callback_t callback;
};


gnss_event_t *gnss_os_fetch_gnss_event(gnss_t *ctx);
void gnss_os_emit_gnss_event(gnss_t *ctx);

void gnss_os_emit_error_event(gnss_t *ctx, unsigned int error);




static inline bool
gnss_decoder(gnss_t *ctx, uint8_t byte)
{
    return ctx->protocol.decoder && ctx->protocol.decoder(ctx, byte);
}


bool gnss_nmea_decoder(gnss_t *ctx, uint8_t byte);


void gnss_init(void);

/**
 *
 */
void gnss_decoder_init(gnss_t *ctx,
		       gnss_callback_t callback,
		       gnss_error_callback_t error_callback);

/**
 *
 */
bool gnss_nmea_send_cmd(gnss_t *ctx, char *cmd);

/**
 *
 */
void gnss_evq_set(struct os_eventq *evq);

#endif
