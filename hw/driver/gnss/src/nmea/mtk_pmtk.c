#include <gnss/gnss.h>
#include <gnss/mynewt.h>

void
gnss_nmea_dump_pmtk(struct gnss_nmea_pmtk *pmtk)
{

    switch(pmtk->type) {
    case GNSS_NMEA_PMTK_TYPE_ACK:
	gnss_os_printf("PMTK[%d]: %d\n", pmtk->ack.cmd, pmtk->ack.status);
	break;

    case GNSS_NMEA_PMTK_TYPE_SYS_MSG:
	gnss_os_printf("PMTK: %d\n", pmtk->sys_msg);
	break;

    case GNSS_NMEA_PMTK_TYPE_TXT_MSG:
	gnss_os_printf("PMTK: %s\n", pmtk->txt_msg);
	break;
    }
}

bool
gnss_nmea_decoder_pmtk(struct gnss_nmea_pmtk *pmtk, char *field, int fid) {
    bool success = true;
    union {
	long sys_msg;
	long cmd;
	long status;
    } local;

    if (fid == 0) { /* PMTK */
	pmtk->type = strtoul(&field[4], NULL, 10);
	return true;
    }

    switch(pmtk->type) {
    case GNSS_NMEA_PMTK_TYPE_ACK:
	switch(fid) {
	case 1:	    /* Cmd */
	    success = gnss_nmea_field_parse_long(field, &local.cmd) &&
		      (local.cmd <= 1000);
	    if (success) {
		pmtk->ack.cmd = local.cmd;
	    }
	    break;

	case 2:	    /* Status */
	    success = gnss_nmea_field_parse_long(field, &local.status) &&
		      (local.status <= 3);
	    if (success) {
		pmtk->ack.status = local.status;
	    }
	    break;

	default:
	    /* Ignore other fields */
	    break;
	}
	break;
	
    case GNSS_NMEA_PMTK_TYPE_SYS_MSG:
	switch(fid) {
	case 1:	    /* Systeme message */
	    success = gnss_nmea_field_parse_long(field, &local.sys_msg) &&
		      (local.sys_msg <= 3);
	    if (success) {
		pmtk->sys_msg = local.sys_msg;
	    }
	    break;

	default:
	    success = false;
	    break;
	}

    case GNSS_NMEA_PMTK_TYPE_TXT_MSG:
	switch(fid) {
	case 1:	    /* Systeme text message */
	    strncpy(pmtk->txt_msg, field, sizeof(pmtk->txt_msg));
	    pmtk->txt_msg[sizeof(pmtk->txt_msg) - 1] = '\0'; 
	    break;

	default:
	    success = false;
	    break;
	}

    default:
	success = false;
	break;
    }
    
    return success;
}


