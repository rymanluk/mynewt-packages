#include <string.h>
#include <gnss/gnss.h>
#include <gnss/mediatek.h>
#include <gnss/log.h>

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

void
gnss_nmea_log_pmtk(struct gnss_nmea_pmtk *pmtk)
{
    static char *status = "???";
    
    switch(pmtk->type) {
    case GNSS_NMEA_PMTK_TYPE_ACK:
	switch(pmtk->ack.status) {
	case GNSS_NMEA_PMTK_ACK_INVALID_COMMAND:
	    status = "Invalid";
	    break;
	case GNSS_NMEA_PMTK_ACK_UNSUPPORTED_COMMAND:
	    status = "Unsupported";
	    break;
	case GNSS_NMEA_PMTK_ACK_ACTION_FAILED:
	    status = "Failed";
	    break;
	case GNSS_NMEA_PMTK_ACK_ACTION_SUCCESSFUL:
	    status  = "Successful";
	    break;
	}
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "PMTK[ACK]: Cmd = %d, Status = %s\n",
		 pmtk->ack.cmd, status);
	break;

    case GNSS_NMEA_PMTK_TYPE_SYS_MSG:
	switch(pmtk->sys_msg) {
	case GNSS_NMEA_PMTK_SYS_MSG_UNKNOWN:
	    status = "Unknown";
	    break;
	case GNSS_NMEA_PMTK_SYS_MSG_STARTUP:
	    status = "Startup";
	    break;
	case GNSS_NMEA_PMTK_SYS_MSG_EPO:
	    status = "EPO";
	    break;
	case GNSS_NMEA_PMTK_SYS_MSG_NORMAL:
	    status  = "Normal";
	    break;
	}	    
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "PMTK[SYS_MSG]: Status = %s\n", status);
	break;

    case GNSS_NMEA_PMTK_TYPE_TXT_MSG:
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "PMTK[TXT_MSG]: %s\n",
		 pmtk->txt_msg);
	break;

    default:
	LOG_INFO(&_gnss_log, LOG_MODULE_DEFAULT,
		 "PMTK: <unknown>\n");
	break;
    }
}
