#include <string.h>
#include <stdio.h>

#include <gnss/gnss.h>
#include <gnss/nmea.h>
#include <uart/uart.h>
#include <gnss/uart.h>

#include <gnss/log.h>

struct log _gnss_log;


struct gnss_dummy_event {
    struct gnss_event event; /* Need to be first */
    uint8_t data[];
};


union gnss_event_memory {
    struct gnss_dummy_event a;
    struct gnss_nmea_event b;
};


#define GNSS_MESSAGE_EVENT_MAXSIZE sizeof(union gnss_event_memory)

static struct os_eventq *_gnss_evq = NULL;

static struct os_mempool _gnss_event_pool;
static os_membuf_t gnss_event_buffer[
	     OS_MEMPOOL_SIZE(MYNEWT_VAL(GNSS_EVENT_MAX),
			     GNSS_MESSAGE_EVENT_MAXSIZE)];




#define GNSS_MS_TO_TICKS(ms) (((ms) * OS_TICKS_PER_SEC + 999) / 1000)





static void
gnss_event_cb(struct os_event *ev)
{
    gnss_event_t *event = (gnss_event_t *) ev;
    gnss_t       *ctx   = (gnss_t *) ev->ev_arg;

    /* Perform logging */
#if MYNEWT_VAL(GNSS_LOG) > 0
    switch(event->type) {
#if MYNEWT_VAL(GNSS_NMEA_LOG) > 0
    case GNSS_EVENT_NMEA:
        gnss_nmea_log(&((struct gnss_nmea_event *)event)->nmea);
	break;
#endif
    }
#endif

    /* Trigger user callback */
    if (ctx->callback) {
	ctx->callback(event->type, &((struct gnss_dummy_event*)event)->data);
    }

    /* Put event back to the memory block */
    os_memblock_put(&_gnss_event_pool, ev);
}

static void
gnss_error_event_cb(struct os_event *ev)
{
    gnss_t *ctx = (gnss_t *) ev;

    /* Trigger user callback */
    if (ctx->error_callback)
	ctx->error_callback(ctx, ctx->error);

    /* Clear error */
    ctx->error = GNSS_ERROR_NONE;
}






void
gnss_os_emit_error_event(gnss_t *ctx, unsigned int error)
{
    os_eventq_put(_gnss_evq, &ctx->error_event.ev);
}

void
gnss_os_emit_gnss_event(gnss_t *ctx)
{
    os_eventq_put(_gnss_evq, &ctx->gnss_event->event);
}


gnss_event_t *
gnss_os_fetch_gnss_event(gnss_t *ctx)
{
    gnss_event_t *evt = os_memblock_get(&_gnss_event_pool);
    evt->event = (struct os_event) {
	.ev_cb  = gnss_event_cb,
	.ev_arg = ctx,
    };
    return evt;
}


void
gnss_eventq_set(struct os_eventq *evq)
{
    _gnss_evq = evq;
}


void
gnss_pkg_init(void)
{
    int rc;
    
    /* Set default event queue */
    _gnss_evq = os_eventq_dflt_get();

    /* Register logger */
    rc = log_register("gnss", &_gnss_log,
		      &log_console_handler, NULL, LOG_LEVEL_DEBUG);
    assert(rc == 0);

    /* Initialise memory pool */
    rc = os_mempool_init(&_gnss_event_pool,
			 MYNEWT_VAL(GNSS_EVENT_MAX),
			 GNSS_MESSAGE_EVENT_MAXSIZE,
			 gnss_event_buffer,
			 "gnss_evt_pool");
    assert(rc == 0);
}

void
gnss_init(gnss_t *ctx,
		  gnss_callback_t callback,
		  gnss_error_callback_t error_callback)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->callback           = callback;
    ctx->error_callback     = error_callback;

    ctx->error_event.ev.ev_cb  = gnss_error_event_cb;
    ctx->error_event.ev.ev_arg = ctx;

}
