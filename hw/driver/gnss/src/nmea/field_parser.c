#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <gnss/nmea.h>

bool
gnss_nmea_field_parse_char(const char *str, char *val)
{
    char v = *str;
    if (val) { *val = v; }
    return str[v ? 1 : 0] == '\0';
}

bool
gnss_nmea_field_parse_long(const char *str, long *val)
{
    char *eob;
    long v = strtol(str, &eob, 10);
    if (val) { *val = v; }
    return *eob == '\0';
}

bool
gnss_nmea_field_parse_crc(const char *str, uint8_t *val)
{
    char *eob;
    long v = strtoul(str, &eob, 16);
    if (v > 255) return false;
    if (val) { *val = v; }
    return *eob == '\0';
}

bool
gnss_nmea_field_parse_float(const char *str, gnss_float_t *val)
{
    char *eob;    
    float v = strtod(str, &eob);
    if (val) { *val = v; }
    return *eob == '\0';
}

bool
gnss_nmea_field_parse_direction(const char *str, int8_t *val)
{
    int8_t v = 0;
    switch (str[0]) {
    case 'N': case 'E': v =  1; break;
    case 'S': case 'W': v = -1; break;
    case '\0': break;
    default: return false;
    }
    if (val) { *val = v; }
    return str[v ? 1 : 0] == '\0';
}

bool
gnss_nmea_field_parse_latlng(const char *str, gnss_float_t *val)
{
    char *eob;
    float v = strtod(str, &eob);
    if (val) { int dd  = v / 100;
	       *val = dd + (v - dd) / 60.0f; }
    return *eob == '\0';    
}


bool
gnss_nmea_field_parse_date(const char *str, gnss_date_t *val)
{
    char *eob;
    gnss_date_t v = { .present = *str != '\0' };
    unsigned long n = strtoul(str, &eob, 10);
    if (val) { v.year    = (n % 100) + GNSS_NMEA_YEAR_OFFSET;
	       v.month   = (n / 100) % 100;
	       v.day     =  n / 10000;
	       *val = v; }
    return *eob == '\0';
}
bool
gnss_nmea_field_parse_time(const char *str, gnss_time_t *val)
{
    char *eob;
    gnss_time_t v = { .present = *str != '\0' };
    float f = strtod(str, &eob);
    if (val) { long n = f;
	       v.hours        =  n / 10000;
	       v.minutes      = (n / 100) % 100;
	       v.seconds      =  n % 100;
	       v.microseconds = (f - n) * 1000000;
	       *val = v; }
    return *eob == '\0';
}
