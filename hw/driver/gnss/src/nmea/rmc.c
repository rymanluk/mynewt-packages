#include <gnss/gnss.h>
#include <gnss/log.h>

void
gnss_nmea_log_rmc(struct gnss_nmea_rmc *rmc)
{
    if (rmc->date.present) {
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "RMC: Date       = %2d-%02d-%02d\n",
		 rmc->date.year,
		 rmc->date.month,
		 rmc->date.day);
    }
    if (rmc->time.present) {
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "RMC: Time       = %2d:%02d:%02d.%03d\n",
		       rmc->time.hours,
		       rmc->time.minutes,
		       rmc->time.seconds,
		       rmc->time.microseconds / 1000);
    }
    if (rmc->valid) {
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "RMC: LatLng     = %f, %f\n", rmc->latitude, rmc->longitude);
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "RMC: Speed      = %f\n", rmc->speed);
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "RMC: Course     = %f\n", rmc->course);
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "RMC: Variation  = %f\n", rmc->variation);
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "RMC: FAA mode   = %c\n", rmc->faa_mode);
    }

    if (!rmc->date.present && !rmc->time.present && !rmc->valid) {
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "RMC: <no valid output>\n");
    }
}


bool
gnss_nmea_decoder_rmc(struct gnss_nmea_rmc *rmc, char *field, int fid) {
    bool success = true;
    union {
	char   status;
	int8_t direction;
    } local;

    switch(fid) {
    case  0:	/* xxRMC */
	break;

    case  1:	/* UTC Time */
	success = gnss_nmea_field_parse_time(field, &rmc->time);
	break;

    case  2:	/* Status */
	success = gnss_nmea_field_parse_char(field, &local.status);
	if (success) {
	    rmc->valid = local.status == 'A';
	}
	break;

    case  3:	/* Latitude */
	success = gnss_nmea_field_parse_latlng(field, &rmc->latitude);
	break;	

    case  4: 	/* N/S indicator */
	success = gnss_nmea_field_parse_direction(field, &local.direction);
	if (success) {
	    rmc->latitude *= local.direction;
	}
	break;

    case  5: 	/* Longitude */
	success = gnss_nmea_field_parse_latlng(field, &rmc->longitude);
	break;

    case  6: 	/* E/W indicator */
	success = gnss_nmea_field_parse_direction(field, &local.direction);
	if (success) {
	    rmc->longitude *= local.direction;
	}
	break;

    case  7:	/* Speed over ground */
	success = gnss_nmea_field_parse_float(field, &rmc->speed);
	if (success) {
	    rmc->speed *= 0.514444;
	}
	break;

    case  8: 	/* Course over ground */
	success = gnss_nmea_field_parse_float(field, &rmc->course);
	break;

    case  9:	/* Date */
	success = gnss_nmea_field_parse_date(field, &rmc->date);
	break;

    case 10:	/* Magnetic variation */
	success = gnss_nmea_field_parse_float(field, &rmc->variation);
	break;

    case 11:	/* E/W indicator */
	success = gnss_nmea_field_parse_direction(field, &local.direction);
	if (success) {
	    rmc->variation *= local.direction;
	}
	break;

    case 12:	/* FAA mode */
	success = gnss_nmea_field_parse_char(field, &rmc->faa_mode);
	break;

    default:
	assert(0);
	break;
    }

    return success;
}


