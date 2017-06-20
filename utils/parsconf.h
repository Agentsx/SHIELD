#ifndef __PARSCONF_H__
#define __PARSCONF_H__

#define __FL__ __FILE__, __LINE__

#include "map.h"

/**
 * fn is the configure file name;
 * mp must has been init;
 * key_type and val_type of mp both must be STR.
 */
int parse_config(const char *fn, map_t *mp);

#endif
