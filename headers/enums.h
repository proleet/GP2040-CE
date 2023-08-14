#ifndef _ENUMS_H_
#define _ENUMS_H_

#include "enums.pb.h"

typedef enum
{
	CONFIG_TYPE_WEB = 0,
	CONFIG_TYPE_SERIAL,
	CONFIG_TYPE_DISPLAY
} ConfigType;

typedef enum  {
    PORTS_8 = 8,
    PORTS_16 = 16,
} ExpanderPorts;

#endif
