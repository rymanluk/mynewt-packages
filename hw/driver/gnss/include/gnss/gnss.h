#ifndef _GNSS_H_
#define _GNSS_H_

#include <gnss/nmea.h>
#include <gnss/os_types.h>

#define GNSS_ERROR_NONE				0
#define GNSS_ERROR_WRONG_BAUD_RATE		1


#define GNSS_DECODER_NMEA			1



struct gnss_event;
struct gnss_decoder;

typedef struct gnss_event gnss_event_t;
typedef struct gnss_decoder gnss_decoder_t;


typedef void (*gnss_message_callback_t)(int type, uint8_t flags);

typedef void (*gnss_error_callback_t)(gnss_decoder_t *ctx, uint8_t error);






struct gnss_event {
    gnss_os_event_t event;
    union {
	struct gnss_nmea nmea;
    };
};

struct gnss_decoder {
    int decoder;
    
    unsigned int error;
    gnss_os_event_t error_event;
    gnss_error_callback_t error_callback;

    struct gnss_event *gnss_event;
    gnss_message_callback_t message_callback;
    
    union {
	struct gnss_nmea_decoder nmea;
    };
};




bool gnss_decoder(gnss_decoder_t *ctx, uint8_t byte);

bool gnss_nmea_decoder(gnss_decoder_t *ctx, uint8_t byte);


void gnss_init(void);

void gnss_decoder_init(gnss_decoder_t *ctx, int decoder);

#endif
