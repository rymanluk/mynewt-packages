#include <ctype.h>
#include <console/console.h>
#include <gnss/gnss.h>

void
gnss_nmea_dump_gga(struct gnss_nmea_gga *gga)
{
    console_printf("Time: %2d:%02d:%02d.%03d\n",
		   gga->time.hours,
		   gga->time.minutes,
		   gga->time.seconds,
		   gga->time.microseconds / 1000
		   );
}



bool
gnss_nmea_decoder_gga(struct gnss_nmea_gga *gga, char *field, int fid) {
    bool success = true;
    union {
	int8_t direction;
	long   fix_indicator;
	long   satellites_in_view;
	long   dgps_age;
	long   dgps_sid;
	char   unit;
    } local;

    switch(fid) {
    case  0:	// xxGGA
	break;
    case  1:	// UTC Time
	success = gnss_nmea_field_parse_time(field, &gga->time);
	break;
    case  2:	// Latitude
	success = gnss_nmea_field_parse_latlng(field, &gga->latitude);
	break;	
    case  3: 	// N/S indicator
	if ((success = gnss_nmea_field_parse_direction(field, &local.direction))) {
	    gga->latitude *= local.direction;
	}
	break;
    case  4: 	// Longitude
	success = gnss_nmea_field_parse_latlng(field, &gga->longitude);
	break;
    case  5: 	// E/W indicator
	if ((success = gnss_nmea_field_parse_direction(field, &local.direction))) {
	    gga->longitude *= local.direction;
	}
	break;
    case  6:	// Fix indicator
	if ((success = gnss_nmea_field_parse_long(field, &local.fix_indicator) &&
	               (local.fix_indicator <= 8))) {
	    gga->fix_indicator = local.fix_indicator;
	}
	break;
    case  7: 	// Satellites in view
	if ((success = gnss_nmea_field_parse_long(field, &local.satellites_in_view) &&
	               (local.satellites_in_view <= 12))) {
	    gga->satellites_in_view = local.satellites_in_view;
	}
	break;
    case  8:	// Horizontal dilution of precision
	success = gnss_nmea_field_parse_float(field, &gga->hdop);
	break;
    case  9:	// Antenna Altitude
	success = gnss_nmea_field_parse_float(field, &gga->altitude);
	break;
    case 10:	// Unit
	success = gnss_nmea_field_parse_char(field, &local.unit) &&
	          (local.unit == 'M');
	break;
    case 11:	// Geoidal separation
	success = gnss_nmea_field_parse_float(field, &gga->geoid_separation);
	break;
    case 12:	// Unit
	success = gnss_nmea_field_parse_char(field, &local.unit) &&
	          (local.unit == 'M');
	break;
    case 13:	// Seconds since last DGPS
	if ((success = gnss_nmea_field_parse_long(field, &local.dgps_age) &&
	               (local.dgps_age <= 65535))) {
	    gga->dgps_age = local.dgps_age;
	}
	break;
    case 14:	// Referential station for DGPS
	if ((success = gnss_nmea_field_parse_long(field, &local.dgps_age) &&
	               (local.dgps_age <= 1023))) {
	    gga->dgps_sid = local.dgps_sid;
	}
	break;
    default:
	assert(0);
	break;
    }

    return success;
}


