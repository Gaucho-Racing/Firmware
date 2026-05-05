#ifndef CAN_MAG_H
#define CAN_MAG_H

#include <stdint.h>

#include "GRCAN_FancyLayer.h"
#include "Logomatic.h"

typedef enum {
	CAN_MAG_MSG_DATA = 0x2C,
	CAN_MAG_MSG_STATUS = 0x2D,
	CAN_MAG_MSG_FAULT = 0x2F
} CAN_MAG_MSG_ID; // change with updates <- fixme

int can_mag_init(GRCAN_NODE_ID mag_ID, CAN_MAG_MSG_ID init_msgID);
int can_mag_send(unsigned int *data);

#endif
