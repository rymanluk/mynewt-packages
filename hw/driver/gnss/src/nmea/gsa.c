#include <gnss/gnss.h>


bool
gnss_nmea_decoder_gsa(struct gnss_nmea_gsa *gsa, char *field, int fid) {
    bool success = true;
    union {
	long mode;
	char selection_mode;
	long sat_id;
    } local;
    
    switch(fid) {
    case  0:	/* xxGSA */
	break;

    case  1:	/* Selection Mode (discard) */
	success = gnss_nmea_field_parse_char(field, &local.selection_mode);
	break;

    case  2:	/* Mode */
	success = gnss_nmea_field_parse_long(field, &local.mode) &&
	          (local.mode <= 3);
	if (success) {
	    gsa->mode = local.mode;
	}
	break;

    case  3:	/* ID of satelitte used for fix */
    case  4:
    case  5:
    case  6:
    case  7:
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
	success = gnss_nmea_field_parse_long(field, &local.sat_id) &&
	          (local.sat_id <= 255);
	if (success) {
	    gsa->sat_id[fid-3] = local.sat_id;
	}
	break;

    case 15:	/* PDOP */
	success = gnss_nmea_field_parse_float(field, &gsa->pdop);
	break;

    case 16:	/* HDOP */
	success = gnss_nmea_field_parse_float(field, &gsa->hdop);
	break;

    case 17:	/* VDOP */
	success = gnss_nmea_field_parse_float(field, &gsa->vdop);
	break;
	
    default:
	assert(0);
	break;
    }

    return success;
}


