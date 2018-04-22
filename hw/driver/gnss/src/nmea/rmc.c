#include <ctype.h>
#include <console/console.h>
#include <gnss/gnss.h>

void
gnss_nmea_dump_rmc(struct gnss_nmea_rmc *rmc)
{
    if (rmc->time.present) {
	console_printf("Time: %2d:%02d:%02d.%03d\n",
		       rmc->time.hours,
		       rmc->time.minutes,
		       rmc->time.seconds,
		       rmc->time.microseconds / 1000);
    }
    if (rmc->date.present) {
	console_printf("Date: %2d-%02d-%02d\n",
		       rmc->date.year,
		       rmc->date.month,
		       rmc->date.day);
    }
    if (rmc->valid) {
	console_printf("LatLng: %f, %f\n", rmc->latitude, rmc->longitude);
	console_printf("Speed : %f\n", rmc->speed);
	console_printf("Course: %f\n", rmc->course);
	console_printf("Variation: %f\n", rmc->variation);
	console_printf("FAA mode : %c\n", rmc->faa_mode);
    } else {
	console_printf("No valid RMC data\n");
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


