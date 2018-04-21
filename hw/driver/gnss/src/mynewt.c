
#include <gnss/gnss.h>
#include <gnss/mynewt.h>


static struct os_eventq *_gnss_internal_evq = NULL;

static struct os_mempool gnss_pool;

static os_membuf_t gnss_memory_buffer[
	      OS_MEMPOOL_SIZE(GNSS_EVENT_MAX, sizeof(gnss_event_t))];


static void
gnss_message_event_cb(struct os_event *ev)
{
    gnss_event_t *gnss_event = (gnss_event_t *) ev;
    // yolo
    (void)gnss_event;
    gnss_nmea_dump_gga(&gnss_event->nmea.gga);

    
    //gnss_decoder_context.nmea_event = &nev;
    
    os_memblock_put(&gnss_pool, ev);
}

static void
gnss_error_event_cb(struct os_event *ev)
{
    unsigned int code = (unsigned int) ev->ev_arg;
    (void)code;

}



void
gnss_os_init(void)
{
    int rc;
    rc = os_mempool_init(&gnss_pool, GNSS_EVENT_MAX, sizeof(gnss_event_t),
			 gnss_memory_buffer, "gnss_pool");
    assert(rc == 0);

    _gnss_internal_evq = os_eventq_dflt_get();
}

void
gnss_os_decoder_init(gnss_decoder_t *ctx)
{
    ctx->error_event.ev_cb = gnss_error_event_cb;
}

void
gnss_internal_evq_set(struct os_eventq *evq)
{
    _gnss_internal_evq = evq;
}

void
gnss_os_emit_error_event(gnss_decoder_t *ctx, unsigned int error)
{
    ctx->error_event.ev_arg = (void *)error;
    os_eventq_put(_gnss_internal_evq, &ctx->error_event);
}

void
gnss_os_emit_gnss_event(gnss_decoder_t *ctx)
{
    os_eventq_put(_gnss_internal_evq, &ctx->gnss_event->event);
}


gnss_event_t *
gnss_os_gnss_event_alloc(gnss_decoder_t *ctx)
{
    gnss_event_t *evt = os_memblock_get(&gnss_pool);
    evt->event = (struct os_event) {
	.ev_cb  = gnss_message_event_cb,
	.ev_arg = &ctx->gnss_event->nmea,
    };
    return evt;
}
