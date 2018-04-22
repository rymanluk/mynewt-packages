#ifndef _GNSS_CONFIG_H_
#define _GNSS_CONFIG_H_

#include <syscfg/syscfg.h>

#define GNSS_EVENT_MAX MYNEWT_VAL(GNSS_EVENT_MAX)

#ifdef MYNEWT_VAL_GNSS_USE_HALF_FLOAT
#define GNSS_USE_HALF_FLOAT MYNEWT_VAL(GNSS_USE_HALF_FLOAT)
#endif

#if MYNEWT_VAL(GNSS_NMEA_USE_GGA) > 0
#define GNSS_NMEA_USE_GGA
#endif

#if MYNEWT_VAL(GNSS_NMEA_USE_GLL) > 0
#define GNSS_NMEA_USE_GLL
#endif

#if MYNEWT_VAL(GNSS_NMEA_USE_GSA) > 0
#define GNSS_NMEA_USE_GSA
#endif

#if MYNEWT_VAL(GNSS_NMEA_USE_GST) > 0
#define GNSS_NMEA_USE_GST
#endif

#if MYNEWT_VAL(GNSS_NMEA_USE_GSV) > 0
#define GNSS_NMEA_USE_GSV
#endif

#if MYNEWT_VAL(GNSS_NMEA_USE_RMC) > 0
#define GNSS_NMEA_USE_RMC
#endif

#if MYNEWT_VAL(GNSS_NMEA_USE_VTG) > 0
#define GNSS_NMEA_USE_VTG
#endif


#if MYNEWT_VAL(GNSS_NMEA_USE_MEDIATEK) > 0
#define GNSS_NMEA_USE_MEDIATEK
#endif


#define GNSS_NMEA_YEAR_OFFSET 2000


#endif
