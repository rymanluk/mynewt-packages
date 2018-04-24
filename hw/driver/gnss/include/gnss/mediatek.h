#ifndef _GNSS_MEDIATEK_H_
#define _GNSS_MEDIATEK_H_

/**
 * Configuration of the MediaTek driver
 */
struct gnss_mediatek {
    int wakeup_pin;       /**< pin use for wakeup             */
    int reset_pin;        /**< pin use for reset              */
    uint16_t cmd_delay;   /**< delay required after a command */
    int wakeup_level;
};

bool gnss_mediatek_init(gnss_t *ctx, struct gnss_mediatek *mtk);

bool gnss_mediatek_standby(gnss_t *ctx, int level);
bool gnss_mediatek_wakeup(gnss_t *ctx);
bool gnss_mediatek_reset(gnss_t *ctx, int type);

#endif
