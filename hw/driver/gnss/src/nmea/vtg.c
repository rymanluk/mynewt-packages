#include <gnss/gnss.h>
#include <gnss/log.h>

bool
gnss_nmea_decoder_vtg(struct gnss_nmea_vtg *vtg, char *field, int fid) {
    bool success = true;
    union {
	char type;
	char unit;
	gnss_float_t speed;
    } local;

    switch(fid) {
    case  0:	/* xxVTG */
	break;

    case  1:	/* True Track */
	success = gnss_nmea_field_parse_float(field, &vtg->true_track);
	break;	

    case  2: 	/* True (T) */
	success = gnss_nmea_field_parse_char(field, &local.type) &&
	          (local.type == 'T');
	break;

    case  3:	/* Magentic Track */
	success = gnss_nmea_field_parse_float(field, &vtg->true_track);
	break;	

    case  4: 	/* Magnetic (M) */
	success = gnss_nmea_field_parse_char(field, &local.type) &&
	          (local.type == 'M');
	break;

    case  5: 	/* Speed */
	success = gnss_nmea_field_parse_float(field, &local.speed);
	if (success) {
	    local.speed *= 0.514444f; /* Knot to m/s */
	    vtg->speed = local.speed;
	}
	break;

    case  6: 	/* Knots (N) */
	success = gnss_nmea_field_parse_char(field, &local.unit) &&
	          (local.unit == 'N');
	break;

    case  7: 	/* Speed */
	success = gnss_nmea_field_parse_float(field, &local.speed);
	if (success) {
	    if ((vtg->speed == 0.0) && (local.speed != 0.0)) {
		local.speed *= 0.277778; /* km/h to m/s */
		vtg->speed = local.speed;
	    }
	}
	break;

    case  8: 	/* Km/h (K) */
	success = gnss_nmea_field_parse_char(field, &local.unit) &&
	          (local.unit == 'K');
	break;

    case  9:	/* FAA mode */
	success = gnss_nmea_field_parse_char(field, &vtg->faa_mode);
	break;


    default:
	assert(0);
	break;
    }

    return success;
}

#if MYNEWT_VAL(GNSS_LOG) > 0
#if MYNEWT_VAL(GNSS_NMEA_LOG) > 0
void
gnss_nmea_log_vtg(struct gnss_nmea_vtg *vtg)
{
    LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
	     "VTG: Track = %f°[T] | %f°[M], Speed = %f m/s, FAA = %c\n",
	     vtg->true_track, vtg->magnetic_track,
	     vtg->speed, vtg->faa_mode);
}
#endif
#endif
