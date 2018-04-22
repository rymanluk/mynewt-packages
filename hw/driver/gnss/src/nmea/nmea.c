#include <gnss/nmea.h>

void
gnss_nmea_log(struct gnss_nmea *nmea)
{    
    switch(nmea->talker) {
    case GNSS_NMEA_TALKER_MTK:
	switch(nmea->sentence) {
#if MYNEWT_VAL(GNSS_NMEA_USE_PGACK) > 0
	case GNSS_NMEA_SENTENCE_PGACK:
	    gnss_nmea_log_pgack(&nmea->data.pgack);
	    break;
#endif
#if MYNEWT_VAL(GNSS_NMEA_USE_PMTK) > 0
	case GNSS_NMEA_SENTENCE_PMTK:
	    gnss_nmea_log_pmtk(&nmea->data.pmtk);
	    break;
#endif
	}
	break;

    default:
	switch(nmea->sentence) {
#if MYNEWT_VAL(GNSS_NMEA_USE_GGA) > 0
	case GNSS_NMEA_SENTENCE_GGA:
	    gnss_nmea_log_gga(&nmea->data.gga);
	    break;
#endif
#if MYNEWT_VAL(GNSS_NMEA_USE_GLL) > 0
	case GNSS_NMEA_SENTENCE_GLL:
	    gnss_nmea_log_gll(&nmea->data.gll);
	    break;
#endif
#if MYNEWT_VAL(GNSS_NMEA_USE_GSA) > 0
	case GNSS_NMEA_SENTENCE_GSA:
	    gnss_nmea_log_gsa(&nmea->data.gsa);
	    break;
#endif
#if MYNEWT_VAL(GNSS_NMEA_USE_GST) > 0
	case GNSS_NMEA_SENTENCE_GST:
	    gnss_nmea_log_gst(&nmea->data.gst);
	    break;
#endif
#if MYNEWT_VAL(GNSS_NMEA_USE_GSV) > 0
	case GNSS_NMEA_SENTENCE_GSV:
	    gnss_nmea_log_gsv(&nmea->data.gsv);
	    break;
#endif
#if MYNEWT_VAL(GNSS_NMEA_USE_RMC) > 0
	case GNSS_NMEA_SENTENCE_RMC:
	    gnss_nmea_log_rmc(&nmea->data.rmc);
	    break;
#endif
#if MYNEWT_VAL(GNSS_NMEA_USE_VTG) > 0
	case GNSS_NMEA_SENTENCE_VTG:
	    gnss_nmea_log_vtg(&nmea->data.vtg);
	    break;
#endif
	}
	break;
    }
}
