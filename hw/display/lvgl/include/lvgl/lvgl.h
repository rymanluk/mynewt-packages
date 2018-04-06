/**
 * @file lvgl.h
 * Include all LittleV GL related headers
 */

#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*Test misc. module version*/
#include "lvgl/include/lvgl/lv_misc/lv_task.h"

#include "lvgl/include/lvgl/lv_hal/lv_hal.h"

#include "lvgl/include/lvgl/lv_core/lv_obj.h"
#include "lvgl/include/lvgl/lv_core/lv_group.h"
#include "lvgl/include/lvgl/lv_core/lv_vdb.h"

#include "lvgl/include/lvgl/lv_themes/lv_theme.h"

#include "lvgl/include/lvgl/lv_objx/lv_btn.h"
#include "lvgl/include/lvgl/lv_objx/lv_img.h"
#include "lvgl/include/lvgl/lv_objx/lv_label.h"
#include "lvgl/include/lvgl/lv_objx/lv_line.h"
#include "lvgl/include/lvgl/lv_objx/lv_page.h"
#include "lvgl/include/lvgl/lv_objx/lv_cont.h"
#include "lvgl/include/lvgl/lv_objx/lv_list.h"
#include "lvgl/include/lvgl/lv_objx/lv_chart.h"
#include "lvgl/include/lvgl/lv_objx/lv_cb.h"
#include "lvgl/include/lvgl/lv_objx/lv_bar.h"
#include "lvgl/include/lvgl/lv_objx/lv_slider.h"
#include "lvgl/include/lvgl/lv_objx/lv_led.h"
#include "lvgl/include/lvgl/lv_objx/lv_btnm.h"
#include "lvgl/include/lvgl/lv_objx/lv_kb.h"
#include "lvgl/include/lvgl/lv_objx/lv_ddlist.h"
#include "lvgl/include/lvgl/lv_objx/lv_roller.h"
#include "lvgl/include/lvgl/lv_objx/lv_ta.h"
#include "lvgl/include/lvgl/lv_objx/lv_win.h"
#include "lvgl/include/lvgl/lv_objx/lv_tabview.h"
#include "lvgl/include/lvgl/lv_objx/lv_mbox.h"
#include "lvgl/include/lvgl/lv_objx/lv_gauge.h"
#include "lvgl/include/lvgl/lv_objx/lv_lmeter.h"
#include "lvgl/include/lvgl/lv_objx/lv_sw.h"
#include "lvgl/include/lvgl/lv_objx/lv_kb.h"

/*********************
 *      DEFINES
 *********************/
/*Current version of LittlevGL*/
#define LVGL_VERSION_MAJOR   5
#define LVGL_VERSION_MINOR   1
#define LVGL_VERSION_PATCH   0
#define LVGL_VERSION_INFO    ""

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif /*LVGL_H*/