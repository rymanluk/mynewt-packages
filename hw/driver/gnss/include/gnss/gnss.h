#ifndef _GNSS_H_
#define _GNSS_H_

#include "nmea.h"

#define GNSS_NMEA_SENTENCE_MAXBYTES  82  /* Including $ and <CR><LF> */
#define GNSS_NMEA_PARSER_BUFSIZE 96



#if GNSS_NMEA_PARSER_BUFSIZE < (GNSS_NMEA_SENTENCE_MAXBYTES + 1)
# error "GNSS_NMEA_PARSER_BUFSIZE is too small"
#endif

#include <os/os.h>



#define GNSS_ERROR_EVENT_WRONG_BAUD_RATE		1


struct gnss_decoder_context;




struct gnss;
typedef struct gnss gnss_t;

struct gnss {
    union {
	struct {
	    int dev;
	    uint8_t addr;
	} i2c;
	struct {
	    char *dev;
	} uart;
    };
    int wakeup_pin;
    
    struct {
	uint16_t offset;
	char     buffer[GNSS_NMEA_PARSER_BUFSIZE];
    } parser;

    struct {
	bool (*read     )(gnss_t *g, char *buffer, uint16_t buflen);
	bool (*write    )(gnss_t *g, char *buffer, uint16_t buflen);
	void (*configure)(gnss_t *g);
	bool (*standby  )(gnss_t *g);
	bool (*wakeup   )(gnss_t *g);
    } op;
};




void gnss_init(gnss_t *g);
void gnss_send_cmd(gnss_t *g, char *cmd);
char *gnss_fetch_nmea(gnss_t *g);

bool gnss_fetch_nmea_sentence(gnss_t *g, char **data, char **crc);




static inline void
gnss_flush(gnss_t *g)
{
    while(gnss_fetch_nmea_sentence(g, NULL, NULL)) {
	// no-op
    }
}



static inline bool
gnss_standby(gnss_t *g)
{
    return g->op.standby && g->op.standby(g);
}

static inline bool
gnss_wakeup(gnss_t *g)
{
    return g->op.wakeup && g->op.wakeup(g);
}

extern struct os_mempool gnss_pool;

				 

struct gnss_decoder_context {
    uint8_t error_event_pending: 1;
    struct os_event error_event;
    union {
	struct gnss_nmea_decoder_context nmea;
    };
};


bool gnss_decoder(struct gnss_decoder_context *ctx, uint8_t byte);



bool gnss_nmea_decoder(struct gnss_decoder_context *ctx, uint8_t byte);

bool gnss_nmea_decoder_gga(struct gnss_nmea_gga *gga, char *field, int fid);
void gnss_nmea_dump_gga(struct gnss_nmea_gga *gga);


#endif
