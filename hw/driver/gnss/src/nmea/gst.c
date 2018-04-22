#include <gnss/gnss.h>
#include <gnss/mynewt.h>

void
gnss_nmea_dump_gst(struct gnss_nmea_gst *gst)
{
    if (gst->time.present) {
	gnss_os_printf("GST: Time       = %2d:%02d:%02d.%03d\n",
		       gst->time.hours,
		       gst->time.minutes,
		       gst->time.seconds,
		       gst->time.microseconds / 1000);
	gnss_os_printf("GST: RMS        = %f\n", gst->rms_stddev);
	gnss_os_printf("GST: S. major   = %f\n", gst->semi_major_stddev);
	gnss_os_printf("GST: S. minor   = %f\n", gst->semi_minor_stddev);
	gnss_os_printf("GST: S. major or= %f\n", gst->semi_major_orientation);
	gnss_os_printf("GST: Latitude   = %f\n", gst->latitude_stddev);
	gnss_os_printf("GST: Longitude  = %f\n", gst->longitude_stddev);
	gnss_os_printf("GST: Altitude   = %f\n", gst->altitude_stddev);

    } else {
	gnss_os_printf("GST: <no valid output>\n");
    }
}


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


