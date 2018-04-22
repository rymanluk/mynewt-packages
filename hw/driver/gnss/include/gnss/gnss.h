#ifndef _GNSS_H_
#define _GNSS_H_

#include <gnss/nmea.h>
#include <gnss/mediatek.h>
#include <gnss/os_types.h>



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


bool gnss_send_cmd(gnss_decoder_t *ctx, char *cmd);

static inline uint8_t
gnss_nmea_crc(char *str) {
   uint8_t crc = 0;
   for ( ; *str ; str++) {
       crc ^= *str;
   }
   return crc;
}



struct gnss_event {
    gnss_os_event_t event;
    union {
	struct gnss_nmea nmea;
    };
};

struct gnss_decoder {
    char *tx_string;
    struct os_sem tx_sem;
    struct uart_dev *uart_dev;

    
    int decoder;
    
    int error;
    gnss_os_event_t error_event;
    gnss_error_callback_t error_callback;

    struct gnss_event *gnss_event;
    gnss_callback_t callback;
    
    union {
	struct gnss_nmea_decoder nmea;
    };
};




bool gnss_decoder(gnss_decoder_t *ctx, uint8_t byte);

bool gnss_nmea_decoder(gnss_decoder_t *ctx, uint8_t byte);


void gnss_init(void);

void gnss_decoder_init(gnss_decoder_t *ctx, int decoder,
		       gnss_callback_t callback,
		       gnss_error_callback_t error_callback);

#endif
