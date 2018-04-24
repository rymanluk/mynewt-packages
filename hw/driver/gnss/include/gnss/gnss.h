#ifndef _GNSS_H_
#define _GNSS_H_

#include <stdint.h>
#include <gnss/nmea.h>
#include <gnss/mediatek.h>
#include <gnss/types.h>


#define GNSS_ERROR_NONE				0
#define GNSS_ERROR_WRONG_BAUD_RATE		1


#define GNSS_EVENT_UNKNOWN			0
#define GNSS_EVENT_NMEA				1

#define GNSS_MS_TO_TICKS(ms) (((ms) * OS_TICKS_PER_SEC + 999) / 1000)


typedef void (*gnss_callback_t)(int type, gnss_event_t *event);
typedef void (*gnss_error_callback_t)(gnss_t *ctx, int error);

typedef bool (*gnss_start_rx_t)(gnss_t *ctx);
typedef bool (*gnss_stop_rx_t)(gnss_t *ctx);
typedef int  (*gnss_send_t)(gnss_t *ctx, uint8_t *bytes, uint16_t size);

typedef bool (*gnss_decoder_t)(gnss_t *ctx, uint8_t byte);

typedef bool (*gnss_standby_t)(gnss_t *ctx, int level);
typedef bool (*gnss_wakeup_t)(gnss_t *ctx);
typedef bool (*gnss_reset_t)(gnss_t *ctx, int type);





struct gnss_event {
    struct os_event event;
    uint8_t type;
    union {
	struct gnss_nmea_message nmea;
    };
};


struct gnss_error_event {
    struct os_event ev; /* Need to be first */
};


struct gnss {
    struct {
	struct gnss_uart *conf;
	gnss_start_rx_t   start_rx;
	gnss_stop_rx_t    stop_rx;
	gnss_send_t       send;
    } transport;

    struct {
	struct gnss_mediatek *conf;
	gnss_standby_t standby;
	gnss_wakeup_t wakeup;
	gnss_reset_t reset;
    } driver;

    struct {
	struct gnss_nmea *conf;
	gnss_decoder_t    decoder;
    } protocol;
    
    int error;
    struct gnss_error_event error_event;
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
    return ctx->protocol.decoder &&
	ctx->protocol.decoder(ctx, byte);
}




void gnss_pkg_init(void);

bool gnss_nmea_send_cmd(gnss_t *ctx, char *cmd);




/*-- Public API --------------------------------------------------------*/

/**
 * Specify the event queue to used for generating callback.
 * (if not sepecified, the default OS event queue is used)
 *
 * @note Calling this function after initialization result 
 *       in an undefined behaviour
 *
 * @param evq		Event queue
 */
void gnss_eventq_set(struct os_eventq *evq);

/**
 * Initialise GNSS 
 *
 * Further initialisation will be required for:
 *  o transport : gnss_uart_init
 *  o protocol  : gnss_nmea_init
 *  o driver    : gnss_mediatek_init
 * 
 * @param ctx		GNSS context
 * @param callback	Called when a GNSS message is received
 * @param error_callback  Called when an error has been encoutered
 */
void gnss_init(gnss_t *ctx,
	       gnss_callback_t callback,
	       gnss_error_callback_t error_callback);

/**
 * Put device in standby (aka power saving).
 *
 * @param ctx		GNSS context
 * @param level		If supported allow to specify how deep
 *                      the device should be put in power saving
 *
 * @return true if the operation is supported and successful
 */
static inline bool
gnss_standby(gnss_t *ctx, int level)
{
    return ctx->driver.standby &&
	ctx->driver.standby(ctx, level);
}

/**
 * Wakeup from power saving
 *
 * @param ctx		GNSS context
 *
 * @return true if the operation is supported and successful
 */
static inline bool
gnss_wakeup(gnss_t *ctx)
{
    return ctx->driver.wakeup &&
	ctx->driver.wakeup(ctx);
}

/**
 * Reset the device
 *
 * @param ctx		GNSS context
 * @param type		Type if reset (hard, soft, ...) if supported
 *                        o -1 : hard reset
 *                        o  0 : hot reset
 *                        o  n : cold reset
 *
 * @return true if the operation is supported and successful
 */
static inline bool
gnss_reset(gnss_t *ctx, int type)
{
    return ctx->driver.reset &&
	ctx->driver.reset(ctx, type);
}

/**
 * Start receiving GNSS information
 *
 * @param ctx		GNSS context
 * @parem decoder	Decoder used for parsing data
 *
 * @return true if the operation is supported and successful
 */
static inline bool
gnss_start_rx(gnss_t *ctx)
{
    return ctx->transport.start_rx &&
	   ctx->protocol.decoder  &&
	   ctx->transport.start_rx(ctx);
}

/**
 * Stop receiving GNSS information
 *
 * @param ctx		GNSS context
 *
 * @return true if the operation is supported and successful
 */
static inline bool
gnss_stop_rx(gnss_t *ctx)
{
    if (ctx->transport.stop_rx) {	
	ctx->protocol.decoder = NULL;
	return ctx->transport.stop_rx(ctx);
    } else {
	return false;
    }
}

/**
 * Send information to the GNSS module.
 *  (Whatever it means for the instanciated driver)
 *
 * @param ctx		GNSS context
 * @param bytes		Data to send
 * @param size		Size of the data to send
 */
static inline int
gnss_send(gnss_t *ctx, uint8_t *bytes, uint16_t size)
{
    return ctx->transport.send &&
	ctx->transport.send(ctx, bytes, size);
}

#endif
