#ifndef _GNSS_TYPES_H_
#define _GNSS_TYPES_H_

#include <gnss/config.h>

#ifdef GNSS_USE_HALF_FLOAT
typedef __fp16 gnss_float_t;
#else
typedef float  gnss_float_t;
#endif

typedef struct gnss_date {
    uint32_t year         : 16;
    uint32_t month        :  4;
    uint32_t day          :  5;
    uint32_t present      :  1;
} gnss_date_t;

typedef struct gnss_time {
    uint32_t hours        :  5;
    uint32_t minutes      :  6; 
    uint32_t seconds      :  6; 
    uint32_t microseconds : 10; 
    uint32_t present      :  1;
} gnss_time_t;

typedef struct gnss_sat_info {
    uint32_t id           :  8;	    /**< Satellit PRN number             */
    uint32_t elevation    :  7;	    /**< Eleveation (degrees) (0-90)     */
    uint32_t azimuth      :  9;	    /**< Azimuth (True North degrees) (0-359) */
    uint32_t snr          :  7;     /**< SNR (dB) (0-99)                 */
} gnss_sat_info_t;


#endif
