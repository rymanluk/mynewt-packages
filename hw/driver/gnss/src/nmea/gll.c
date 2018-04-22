#include <gnss/gnss.h>
#include <gnss/mynewt.h>

void
gnss_nmea_dump_gll(struct gnss_nmea_gll *gll)
{
    if (gll->time.present) {
	gnss_os_printf("GLL: Time       = %2d:%02d:%02d.%03d\n",
		       gll->time.hours,
		       gll->time.minutes,
		       gll->time.seconds,
		       gll->time.microseconds / 1000);
    }
    if (gll->valid) {
	gnss_os_printf("GLL: LatLng     = %f, %f\n", gll->latitude, gll->longitude);
	gnss_os_printf("GLL: FAA mode   = %c\n", gll->faa_mode);
    }

    if (!gll->time.present && !gll->valid) {
	gnss_os_printf("GLL: <no valid output>\n");
    }
}

bool
gnss_nmea_decoder_gll(struct gnss_nmea_gll *gll, char *field, int fid) {
    bool success = true;
    union {
	char   status;
	int8_t direction;
    } local;

    switch(fid) {
    case  0:	/* xxGLL */
	break;

    case  1:	/* Latitude */
	success = gnss_nmea_field_parse_latlng(field, &gll->latitude);
	break;	

    case  2: 	/* N/S indicator */
	success = gnss_nmea_field_parse_direction(field, &local.direction);
	if (success) {
	    gll->latitude *= local.direction;
	}
	break;

    case  3: 	/* Longitude */
	success = gnss_nmea_field_parse_latlng(field, &gll->longitude);
	break;

    case  4: 	/* E/W indicator */
	success = gnss_nmea_field_parse_direction(field, &local.direction);
	if (success) {
	    gll->longitude *= local.direction;
	}
	break;

    case  5:	/* UTC Time */
	success = gnss_nmea_field_parse_time(field, &gll->time);
	break;

    case  6:	/* Status */
	success = gnss_nmea_field_parse_char(field, &local.status);
	if (success) {
	    gll->valid = local.status == 'A';
	}
	break;

    case 7:	/* FAA mode */
	success = gnss_nmea_field_parse_char(field, &gll->faa_mode);
	break;

    default:
	assert(0);
	break;
    }

    return success;
}


