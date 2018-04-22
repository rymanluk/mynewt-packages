#include <string.h>
#include <ctype.h>
#include <console/console.h>
#include <gnss/gnss.h>
#include <gnss/mynewt.h>

#define GNSS_STATE_VIRGIN		0x00
#define GNSS_STATE_FLG_STARTED		0x01
#define GNSS_STATE_FLG_CRC		0x02
#define GNSS_STATE_FLG_CR 		0x04



static gnss_nmea_field_decoder_t
gnss_nmea_get_field_decoder(const char *tag,
			    uint16_t *talker, uint16_t *sentence)
{
    gnss_nmea_field_decoder_t field_decoder = NULL;
    uint16_t                  t_id          = 0;
    uint16_t                  s_id          = 0;

    /* Easy case, tag is coded with 5 char: talker(2) + sentence(3) */
    if (strlen(tag) == 5) {
	char talker_str[3] = { tag[0], tag[1], 0 };
	t_id = (uint16_t)strtoul(talker_str, NULL, 36);
	s_id = (uint16_t)strtoul(&tag[2],    NULL, 36);
	
	switch(s_id) {
#if defined(GNSS_NMEA_USE_GGA)
	case GNSS_NMEA_SENTENCE_GGA:
	    field_decoder = (gnss_nmea_field_decoder_t)gnss_nmea_decoder_gga;
	    break;
#endif
#if defined(GNSS_NMEA_USE_GLL)
	case GNSS_NMEA_SENTENCE_GLL:
	    field_decoder = (gnss_nmea_field_decoder_t)gnss_nmea_decoder_gll;
	    break;
#endif
#if defined(GNSS_NMEA_USE_GSA)
	case GNSS_NMEA_SENTENCE_GSA:
	    field_decoder = (gnss_nmea_field_decoder_t)gnss_nmea_decoder_gsa;
	    break;
#endif
#if defined(GNSS_NMEA_USE_GST)
	case GNSS_NMEA_SENTENCE_GST:
	    field_decoder = (gnss_nmea_field_decoder_t)gnss_nmea_decoder_gst;
	    break;
#endif
#if defined(GNSS_NMEA_USE_GSV)
	case GNSS_NMEA_SENTENCE_GSV:
	    field_decoder = (gnss_nmea_field_decoder_t)gnss_nmea_decoder_gsv;
	    break;
#endif
#if defined(GNSS_NMEA_USE_RMC)
	case GNSS_NMEA_SENTENCE_RMC:
	    field_decoder = (gnss_nmea_field_decoder_t)gnss_nmea_decoder_rmc;
	    break;
#endif
#if defined(GNSS_NMEA_USE_VTG)
	case GNSS_NMEA_SENTENCE_VTG:
	    field_decoder = (gnss_nmea_field_decoder_t)gnss_nmea_decoder_vtg;
	    break;
#endif
	}

    /* Proprietary tags */
    } else if (!strncmp(tag, "PMTK", 4)) {
	t_id = GNSS_NMEA_TALKER_PMTK;
	s_id = strtoul(&tag[4], NULL, 10);	
    }
    
    if (field_decoder != NULL) {
	if (talker) {
	    *talker   = t_id;
	}
	if (sentence) {
	    *sentence = s_id;
	}
    }

    return field_decoder;
}




static bool
gnss_decode_nmea_field(gnss_decoder_t *ctx)
{
    struct gnss_nmea_decoder * const nctx = &ctx->nmea;

    /* Check that we are in the main part */
    /*  (ie: started but no crc, no <cr>) */
    if (nctx->state != GNSS_STATE_FLG_STARTED) {
	nctx->stats.parsing_error++;
	return false;
    }
    
    /* Make it a NUL terminated string,               */
    /*  we kept 1 byte in the buffer for that purpose */
    nctx->buffer[nctx->bufcnt] = '\0';

    /* Special handling for NMEA tag (field id == 0)  */
    /*  we need to get the dedicated decoder          */
    if (nctx->fid == 0) {
	nctx->field_decoder =
	    gnss_nmea_get_field_decoder(nctx->buffer,
					&ctx->gnss_event->nmea.talker,
					&ctx->gnss_event->nmea.sentence);
	/* If we don't have a decoder, just trash everything */
	if (nctx->field_decoder == NULL) {
	    nctx->stats.no_decoder++;
	    return false;
	}
	
    /* Decode NMEA field (field id >= 1) */
    } else {
	if (!nctx->field_decoder(&ctx->gnss_event->nmea.data,
				 nctx->buffer, nctx->fid)) {
	    //console_out('-');
	    nctx->stats.parsing_error++;
	    return false;
	} else {
	    //console_out('*');
	}
    }

    return true;
}

bool
gnss_nmea_decoder(gnss_decoder_t *ctx, uint8_t byte)
{
    struct gnss_nmea_decoder *nctx = &ctx->nmea;
    
    /* Decode NMEA one byte at a time
     */
    switch(byte) {
    /* Start of sentence marker */
    case '$':
	/* Check if we need to allocate an nmea event */
	if (ctx->gnss_event == NULL) {
	    ctx->gnss_event = gnss_os_fetch_gnss_event(ctx);
	    if (ctx->gnss_event == NULL) {
		nctx->stats.allocation_error++;
		goto trash_it;
	    }
	}
	
	/* Ensure context is reseted */
	nctx->state     = 0x01;
	nctx->fid       = 0;
	nctx->crc       = 0;
	nctx->binaries  = 0;
	nctx->bufcnt    = 0;
	break;
	
    /* Field separator */
    case ',':
	/* Decode current field (will also ensure state is legit) */
	if (!gnss_decode_nmea_field(ctx)) {
	    goto trash_it;
	}
	
	/* Mark field as processed */
	nctx->crc     ^= byte; /* Compute CRC (',' is in main part) */
	nctx->fid     += 1;    /* Moving to next field              */
	nctx->bufcnt   = 0;    /* Reset field buffer                */
	break;

    /* CRC marker */
    case '*':
	/* Decode current field (will also ensure state is legit) */
	if (!gnss_decode_nmea_field(ctx)) {
	    goto trash_it;
	}
	
	/* Mark field as processed, and starting CRC decoding */
	nctx->bufcnt = 0;                   /* Reset field buffer    */
	nctx->state |= GNSS_STATE_FLG_CRC;  /* Start of CRC decoding */
	break;

    /* <CR> marker */
    case '\r':
	/* Ensure that's the first <cr> */
	if (nctx->state & GNSS_STATE_FLG_CR) {
	    nctx->stats.parsing_error++;
	    goto trash_it;
	}

	/* Either marking end of CRC or end of field */
	if (nctx->state & GNSS_STATE_FLG_CRC) {
	    uint8_t crc;

	    /* Make the buffer a NUL terminated string,       */
	    /*  we kept 1 byte in the buffer for that purpose */
	    nctx->buffer[nctx->bufcnt] = '\0';

	    /* Decocde CRC */
	    if (!gnss_nmea_field_parse_crc(nctx->buffer, &crc)) {
		nctx->stats.parsing_error++;
		goto trash_it;
	    }

	    /* Validate CRC */
	    if (crc != nctx->crc) {
		nctx->stats.crc_error++;
		goto trash_it;
	    }
	    
	} else {
	    /* Decode current field (will also ensure state is legit) */
	    if (!gnss_decode_nmea_field(ctx)) {
		goto trash_it;
	    }
	}

	/* Mark state as <CR> acknowledged */
	nctx->state |= GNSS_STATE_FLG_CR;
	break;

    /* <LF> marker: Sentence is finished */
    case '\n':
	/* Ensure we've got everything                    */
	/*  (at leaset started and <cr>, crc is optional) */
	if ((nctx->state & (GNSS_STATE_FLG_STARTED|GNSS_STATE_FLG_CR)) !=
	    (GNSS_STATE_FLG_STARTED|GNSS_STATE_FLG_CR)) {
	    nctx->stats.parsing_error++;
	    goto trash_it;
	}
	
	/* Post event, and detach it from the decoding context */
	gnss_os_emit_gnss_event(ctx);
	ctx->gnss_event = NULL;

	/* Job's done */
	//console_out('\n');
	nctx->state = 0x00;
	break;

    /* Other chars */
    default:
	/* Ensure that's a legal character           */
	/*  => helping detection of wrong baud rate  */
	if (!isprint(byte)) {
	    if (++nctx->binaries > GNNS_NMEA_BINARY_THRESHOLD) {
		/* Generate error */
		gnss_os_emit_error_event(ctx, GNSS_ERROR_WRONG_BAUD_RATE);
		/* Terminate RX */
		nctx->state = 0x00;	/* Reset state 		*/
		return false;		/* Stop reception 	*/
	    }
	    goto trash_it;
	}
	
	/* Ensure we have started decoding */
	if (! (nctx->state & GNSS_STATE_FLG_STARTED)) {
	    nctx->stats.parsing_error++;
	    goto trash_it;
	}
	/* Check that buffer is not full (keeping 1 byte for NUL-char) */
	if (nctx->bufcnt >= (GNSS_NMEA_FIELD_BUFSIZE-1)) {
	    nctx->stats.buffer_full++;
	    goto error;
	}       
	/* If decodind main part, compute CRC */
	if (nctx->state == GNSS_STATE_FLG_STARTED) {
	    nctx->crc ^= byte;
	}
	/* Add byte to buffer */
	nctx->buffer[nctx->bufcnt++] = byte;
	break;

    }

    return true;

 trash_it:
    nctx->state = 0x00;
 error:
    return true;
}
