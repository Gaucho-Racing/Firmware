#include "GRCAN_NODE_ID.h"
#include "Stringification.h"

#ifndef NODE_ID_H
#define NODE_ID_H

#ifndef NODE
						#pragma message("NODE is not defined. Please compile Tire Temp for a specific target. Defaulting to FL.")
#define LOCAL_GR_ID GRCAN_TireTemp_FL
#else
#define LOCAL_GR_ID CONCAT(GRCAN_TireTemp_, NODE)
#endif

#pragma message "Compiling Tire Temp node as " STRINGIFY_MACRO(LOCAL_GR_ID)

#endif
