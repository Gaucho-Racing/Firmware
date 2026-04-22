#include "GRCAN_NODE_ID.h"

#ifndef NODE_ID_H
#define NODE_ID_H

#ifndef NODE
#pragma message("NODE is not defined. Please compile Tire Temp for a specific target. Defaulting to FL.")
#define LOCAL_GR_ID GRCAN_TireTemp_FL
#else
#define LOCAL_GR_ID GRCAN_TireTemp_##NODE
#endif

#endif
