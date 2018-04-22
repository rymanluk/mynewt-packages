#include <gnss/gnss.h>


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


