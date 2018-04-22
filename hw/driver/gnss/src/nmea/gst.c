#include <gnss/gnss.h>


bool
gnss_nmea_decoder_gst(struct gnss_nmea_gst *gst, char *field, int fid) {
    bool success = true;

    switch(fid) {
    case  0:	/* xxGST */
	break;

    case  1:	/* UTC Time of associated fix */
	success = gnss_nmea_field_parse_time(field, &gst->time);
	break;

    case  2:	/* RMS standard deviation */
	success = gnss_nmea_field_parse_float(field, &gst->rms_stddev);
	break;

    case  3:	/* Standard deviation  of semi-major axis of error ellipse */
	success = gnss_nmea_field_parse_float(field, &gst->semi_major_stddev);
	break;	

    case  4: 	/* Standard deviation of semi-minor axis of error ellipse */
	success = gnss_nmea_field_parse_float(field, &gst->semi_minor_stddev);
	break;

    case  5: 	/* Orientation of semi-major axis of error ellipse */
	success = gnss_nmea_field_parse_float(field, &gst->semi_major_orientation);
	break;

    case  6: 	/* Standard deviation of latitude error */
	success = gnss_nmea_field_parse_float(field, &gst->latitude_stddev);
	break;

    case  7:	/* Standard deviation of longitude error */
	success = gnss_nmea_field_parse_float(field, &gst->longitude_stddev);
	break;

    case  8: 	/* Course over ground */
	success = gnss_nmea_field_parse_float(field, &gst->altitude_stddev);
	break;

    default:
	assert(0);
	break;
    }

    return success;
}


