#include <gnss/gnss.h>
#include <gnss/mynewt.h>

void
gnss_nmea_dump_gsa(struct gnss_nmea_gsa *gsa)
{
    if (gsa->fix_mode != 0) {
	gnss_os_printf("GSA: PDOP       = %f\n", gsa->pdop);
	gnss_os_printf("GSA: HDOP       = %f\n", gsa->hdop);
	gnss_os_printf("GSA: VDOP       = %f\n", gsa->vdop);
	gnss_os_printf("GSA: Satellites =");
	for (int i = 0 ; i < 12 ; i++) {
	    gnss_os_printf(" %d", gsa->sid[i]);
	}
	gnss_os_printf(" \n");
	
	gnss_os_printf("GSA: FIX mode   = %d\n", gsa->fix_mode);
    } else {
	gnss_os_printf("GSA: <no valid output>\n");
    }
}



bool
gnss_nmea_decoder_gsa(struct gnss_nmea_gsa *gsa, char *field, int fid) {
    bool success = true;
    union {
	long fix_mode;
	long sid;
    } local;
    
    switch(fid) {
    case  0:	/* xxGSA */
	break;

    case  1:	/* Selection Mode */
	success = gnss_nmea_field_parse_char(field, &gsa->fix_mode_selection);
	break;

    case  2:	/* Mode */
	success = gnss_nmea_field_parse_long(field, &local.fix_mode) &&
	          (local.fix_mode <= 3);
	if (success) {
	    gsa->fix_mode = local.fix_mode;
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
	success = gnss_nmea_field_parse_long(field, &local.sid) &&
	          (local.sid <= 255);
	if (success) {
	    gsa->sid[fid-3] = local.sid;
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


