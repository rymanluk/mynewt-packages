#ifndef _GNSS_NMEA_H_
#define _GNSS_NMEA_H_

#include <stdint.h>
#include <stdbool.h>
#include <gnss/types.h>


/*
 * See: http://www.catb.org/gpsd/NMEA.html
 */

/*
 * Config
 */
#define GNSS_NMEA_FIELD_BUFSIZE  32

#define GNNS_NMEA_BINARY_THRESHOLD 32




#define GNSS_NMEA_SENTENCE_MAXBYTES  82  

/**
 * Fix type
 */
#define GNNS_NMEA_FIX_NOT_AVAILABLE			0
#define GNNS_NMEA_FIX_GPS				1
#define GNNS_NMEA_FIX_DIFFERENTIAL_GPS			2
#define GNNS_NMEA_FIX_PPS				3
#define GNNS_NMEA_FIX_REAL_TIME_KINEMATIC		4
#define GNNS_NMEA_FIX_FLOAT_RTK				5
#define GNNS_NMEA_FIX_DEAD_RECKONING			6
#define GNNS_NMEA_FIX_MANUAL_INPUT			7
#define GNNS_NMEA_FIX_SIMULATION			8

/**
 * FAA modes
 */
#define GNNS_NMEA_FAA_AUTONOMOUS		      'A'
#define GNNS_NMEA_FAA_DIFFERENTIAL		      'D'
#define GNSS_NMEA_FAA_DEAD_RECKONING		      'E'
#define GNSS_NMEA_FAA_MANUAL			      'M'
#define GNSS_NMEA_FAA_SIMULATED			      'S'
#define GNSS_NMEA_FAA_NOT_VALID			      'N'
#define GNSS_NMEA_FAA_PRECISE			      'P'

/**
 * Talker Id
 */
#define GNSS_NMEA_TALKER_GP  			      601
#define GNSS_NMEA_TALKER_GA  			      586
#define GNSS_NMEA_TALKER_GL  			      597
#define GNSS_NMEA_TALKER_GN  			      599
#define GNSS_NMEA_TALKER_GB  			      587
#define GNSS_NMEA_TALKER_BD  			      409
#define GNSS_NMEA_TALKER_QZ  			      971
#define GNSS_NMEA_TALKER_PBUX			    65011
#define GNSS_NMEA_TALKER_PMTK			    65021
#define GNSS_NMEA_TALKER_PGACK			    65022

/**
 * Sentence type
 */
#define GNSS_NMEA_SENTENCE_RMC  		    35796
#define GNSS_NMEA_SENTENCE_GGA 			    21322
#define GNSS_NMEA_SENTENCE_GSA 			    21754
#define GNSS_NMEA_SENTENCE_GST 			    21773
#define GNSS_NMEA_SENTENCE_GLL 			    21513
#define GNSS_NMEA_SENTENCE_GSV 			    21775
#define GNSS_NMEA_SENTENCE_VTG 			    41236

/**
 * RMC - Recommended Minimum Navigation Information
 */
struct gnss_nmea_rmc {
    gnss_date_t  date;			/**< Date (2-digit year) 	   */ 
    gnss_time_t  time;			/**< UTC Time            	   */
    gnss_float_t latitude;		/**< Latitude (decimale degrees)   */
    gnss_float_t longitude;		/**< Longitude (decimal degrees)   */
    gnss_float_t speed;			/**< Speed over ground (m/s)	   */
    gnss_float_t course;		/**< Course (degrees)		   */
    gnss_float_t variation;		/**< Magnetic variation (degrees)  */
    bool         valid;			/**< Valid fix			   */
    char         faa_mode;		/**< FAA mode			   */
};

/**
 * GGA - Global Positioning System Fix Data
 */
struct gnss_nmea_gga {
    gnss_time_t  time;			/**< UTC time			   */
    gnss_float_t latitude;		/**< Latitude  (decimal degrees)   */
    gnss_float_t longitude;		/**< Longitude (decimal degrees)   */
    gnss_float_t hdop;			/**< Horizontal dilution of precision*/
    gnss_float_t altitude;		/**< Altitude (meters)             */
    gnss_float_t geoid_separation;	/**< Geoid sepearation (meters)    */
    uint16_t     dgps_age;		/**< Age of DGPS (seconds)         */
    uint16_t	 dgps_sid;		/**< Satelitte Id of DGPS (0-1023) */
    uint8_t      fix_indicator     :4;	/**< Fix quality (0-8)             */
    uint8_t      satellites_in_view:4;	/**< Nb. of satelittes in view (0-12)*/
};

/**
 * GST - GPS Pseudorange Noise Statistics
 */
struct gnss_nmea_gst {
    gnss_time_t  time;			/**< Time of associated GGA fix	   */
    gnss_float_t rms_stddev; 	        /**< RMS standard deviation	   */
    gnss_float_t semi_major_stddev;	/**  Semi-major axis (meters)      */
    gnss_float_t semi_minor_stddev;     /**< Semi-minor axis (meters)      */
    gnss_float_t semi_major_orientation;/**< Orientation (true North degrees)*/
    gnss_float_t latitude_stddev;	/**< Latitutde std. dev. (meters)  */
    gnss_float_t longitude_stddev;      /**< Longitude std. dev. (meters)  */
    gnss_float_t altitude_stddev;	/**< Altitude std. dev. (meters)   */
};

/**
 * GSA - GPS DOP and active satellites
 */
struct gnss_nmea_gsa {
    gnss_float_t pdop;			/**< PDOP			   */
    gnss_float_t hdop;			/**< HDOP			   */
    gnss_float_t vdop;			/**< VDOP			   */
    uint8_t      sat_id[12];		/**< List of satelittes id	   */
    char         mode;			/**< Mode			   */
    uint8_t      fix_type;		/**< Fix type			   */
};

/**
 * GLL - Geographic Position - Latitude/Longitude
 */
struct gnss_nmea_gll {
    gnss_float_t latitude;		/**< Latitude  (decimal degrees)   */
    gnss_float_t longitude;		/**< Longitude (decimal degrees)   */
    gnss_time_t  time;			/**< UTC Time            	   */
    bool         valid;			/**< Valid			   */
    char         faa_mode;		/**< FAA mode			   */
};

/**
 * GSV - Satellites in view
 */
struct gnss_nmea_gsv {
    uint8_t         msg_count;		/**< Number of messages 	   */
    uint8_t         msg_id;		/**< Index of this message	   */
    uint8_t         total_sats;		/**< Total number of Sat. in view  */
    gnss_sat_info_t sat_info[4];	/**< Satellite info		   */
};

/**
 * VTG - Track made good and Ground speed
 */
struct gnss_nmea_vtg {
    gnss_float_t true_track;		/**< True track (degrees)	   */
    gnss_float_t magnetic_track;	/**< Magnetic track (degress)      */
    gnss_float_t speed;			/**< Speed (km/h)		   */
    char         faa_mode;		/**< FAA mode			   */
};

/**
 * NMEA message
 */
typedef struct gnss_nmea {
    uint16_t talker;
    uint16_t sentence;
    union {
	struct gnss_nmea_rmc rmc;
	struct gnss_nmea_gga gga;
	struct gnss_nmea_gsa gsa;
	struct gnss_nmea_gst gst;
	struct gnss_nmea_gsv gsv;
	struct gnss_nmea_gll gll;
	struct gnss_nmea_vtg vtg;
    };
} gnss_nmea_t;







typedef bool (*gnss_nmea_field_decoder_t)(void *, char *field, int fid);



struct gnss_nmea_decoder {
    gnss_nmea_field_decoder_t field_decoder;
    uint8_t state;
    uint8_t binaries;
    uint8_t crc;
    uint8_t fid;
    uint8_t bufcnt;
    
    char buffer[GNSS_NMEA_FIELD_BUFSIZE];
    struct {
	uint32_t allocation_error;
	uint32_t parsing_error;
	uint32_t no_decoder;
	uint32_t crc_error;
	uint32_t buffer_full;
    } stats;
};


bool gnss_nmea_field_parse_char(const char *str, char *val);
bool gnss_nmea_field_parse_long(const char *str, long *val);
bool gnss_nmea_field_parse_float(const char *str, gnss_float_t *val);
bool gnss_nmea_field_parse_direction(const char *str, int8_t *val);
bool gnss_nmea_field_parse_latlng(const char *str, gnss_float_t *val);
bool gnss_nmea_field_parse_date(const char *str, gnss_date_t *val);
bool gnss_nmea_field_parse_time(const char *str, gnss_time_t *val);
bool gnss_nmea_field_parse_crc(const char *str, uint8_t *val);

bool gnss_nmea_decoder_gga(struct gnss_nmea_gga *gga, char *field, int fid);
bool gnss_nmea_decoder_rmc(struct gnss_nmea_rmc *rcm, char *field, int fid);
void gnss_nmea_dump_gga(struct gnss_nmea_gga *gga);
void gnss_nmea_dump_rmc(struct gnss_nmea_rmc *rmc);


#endif
