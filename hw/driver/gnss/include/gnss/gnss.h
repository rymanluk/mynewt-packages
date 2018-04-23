#ifndef _GNSS_H_
#define _GNSS_H_

#include <gnss/nmea.h>
#include <gnss/mediatek.h>


extern struct log _gnss_log;

#define GNSS_ERROR_NONE				0
#define GNSS_ERROR_WRONG_BAUD_RATE		1


#define GNSS_DECODER_NMEA			1


int gnss_uart_rx_char(void *arg, uint8_t byte);
int gnss_uart_tx_char(void *arg);
void gnss_uart_tx_done(void *arg);



struct gnss_event;
struct gnss_decoder;

typedef struct gnss_event gnss_event_t;
typedef struct gnss_decoder gnss_decoder_t;


typedef void (*gnss_callback_t)(int type, gnss_event_t *event);

typedef void (*gnss_error_callback_t)(gnss_decoder_t *ctx, int error);






struct gnss_event {
    struct os_event event;
    union {
	struct gnss_nmea nmea;
    };
};




struct gnss_decoder {
#if MYNEWT_VAL(GNSS_RX_ONLY) == 0
    char *tx_string;
    struct os_sem tx_sem;
    struct uart_dev *uart_dev;
#endif
    
    int decoder;
    
    int error;
    struct os_event error_event;
    gnss_error_callback_t error_callback;

    struct gnss_event *gnss_event;
    gnss_callback_t callback;
    
    union {
	struct gnss_nmea_decoder nmea;
    };
};


gnss_event_t *gnss_os_fetch_gnss_event(gnss_decoder_t *ctx);
void gnss_os_emit_gnss_event(gnss_decoder_t *ctx);

void gnss_os_emit_error_event(gnss_decoder_t *ctx, unsigned int error);




bool gnss_decoder(gnss_decoder_t *ctx, uint8_t byte);

bool gnss_nmea_decoder(gnss_decoder_t *ctx, uint8_t byte);


void gnss_init(void);

/**
 *
 */
void gnss_decoder_init(gnss_decoder_t *ctx, int decoder,
		       gnss_callback_t callback,
		       gnss_error_callback_t error_callback);

/**
 *
 */
bool gnss_nmea_send_cmd(gnss_decoder_t *ctx, char *cmd);

/**
 *
 */
void gnss_evq_set(struct os_eventq *evq);

#endif
