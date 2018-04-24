#include <hal/hal_gpio.h>
#include <gnss/gnss.h>
#include <gnss/nmea.h>
#include <gnss/mediatek.h>

bool
gnss_mediatek_init(gnss_t *ctx, struct gnss_mediatek *mtk) {
    ctx->driver.conf    = mtk;

    ctx->driver.standby = gnss_mediatek_standby;
    ctx->driver.wakeup  = gnss_mediatek_wakeup;
    ctx->driver.reset   = gnss_mediatek_reset;

    if (mtk->reset_pin >= 0) {
	hal_gpio_init_out(mtk->reset_pin, 1);
    }
    if (mtk->wakeup_pin >= 0) {
	hal_gpio_init_out(mtk->wakeup_pin, 1);
    }

    return true;
}

bool
gnss_mediatek_standby(gnss_t *ctx, int level)
{
    struct gnss_mediatek *mtk = ctx->driver.conf;

    switch(level) {
    case -1:
	if (mtk->wakeup_pin < 0) {
	    return false;
	}
	hal_gpio_write(mtk->wakeup_pin, 0);
	gnss_nmea_send_cmd(ctx, "PMTK225,0");
	gnss_nmea_send_cmd(ctx, "PMTK225,4");
	break;

    case 0:
	gnss_nmea_send_cmd(ctx, "PMTK161,0");
	break;

    default:
	return false;
    }

    return true;
}

bool
gnss_mediatek_wakeup(gnss_t *ctx)
{

#if 0
    gnss_send("");

    hal_gpio_write(g->wakeup_pin, 1);
#endif
    
    return false;
}

bool
gnss_mediatek_reset(gnss_t *ctx, int type)
{
    struct gnss_mediatek *mtk = ctx->driver.conf;
    switch(type) {
    case -1: /* Hard reset */
	if (mtk->reset_pin < 0) {
	    return false;
	}
	hal_gpio_write(mtk->reset_pin, 0);
	os_time_delay(1);
	hal_gpio_write(mtk->reset_pin, 1);
	break;

    case 0: /* Hot restart */
	gnss_nmea_send_cmd(ctx, "PMTK101");
	break;

    case 1: /* Warm restart */
	gnss_nmea_send_cmd(ctx, "PMTK102");
	break;

    case 2: /* Cold restart */
	gnss_nmea_send_cmd(ctx, "PMTK103"); 
	break;

    case 3: /* Full cold restart */
	gnss_nmea_send_cmd(ctx, "PMTK104"); 
	break;

    default:
	return false;
    }
    
    return true;
}
