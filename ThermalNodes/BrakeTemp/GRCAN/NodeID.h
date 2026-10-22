#include "GRCAN_NODE_ID.h"
#include "Stringification.h"

#ifndef NODE_ID_H
#define NODE_ID_H

#ifndef NODE
#pragma message("NODE is not defined. Please compile Brake Temp for a specific target. Defaulting to FL.")
#define LOCAL_GR_ID GRCAN_BrakeTemp_FL
#else
#define LOCAL_GR_ID CONCAT(GRCAN_BrakeTemp_, NODE)
#endif

#pragma message "Compiling Brake Temp node as " STRINGIFY_MACRO(LOCAL_GR_ID)

#endif
