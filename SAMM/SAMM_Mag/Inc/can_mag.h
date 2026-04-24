#ifndef CAN_MAG_H
#define CAN_MAG_H

#include <stdint.h>

#include "GRCAN_FancyLayer.h"
#include "Logomatic.h"

#define CAN_MAG_DEFAULT_MSG_ID 0x2C

int can_mag_init(GRCAN_NODE_ID mag_ID);
int can_mag_send(unsigned int *data);

#endif
