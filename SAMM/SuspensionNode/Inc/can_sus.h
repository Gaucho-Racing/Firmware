#ifndef CAN_MAG_H
#define CAN_MAG_H

#include <stdint.h>

#include "GRCAN_FancyLayer.h"
#include "Logomatic.h"
#include "NodeID.h"

static GRCAN_NODE_ID destNode = GRCAN_TCM;

typedef enum {
	CAN_SUS_MSG_DATA = 0x2C,
	CAN_SUS_MSG_STATUS = 0x2D,
	CAN_SUS_MSG_FAULT = 0x2F
} Sus_MSG_ID; // change with updates <- fixme


typedef enum {
	TireTemp_FL = GRCAN_TireTemp_FL,
	TireTemp_FR = GRCAN_TireTemp_FR,
	TireTemp_RL = GRCAN_TireTemp_RL,
	TireTemp_RR = GRCAN_TireTemp_RR
} TireTemp_Node;

typedef enum{
	Sus_FL = GRCAN_Suspension_FL,
	Sus_FR = GRCAN_Suspension_FR,
	Sus_RL = GRCAN_Suspension_RL,
	Sus_RR = GRCAN_Suspension_RR
} Sus_Node;

typedef enum {
	CAN_SUBNET_BUS = 0,
	CAN_DATAMAIN_BUS = 1
} CAN_SAMM_ROUTING_BUS;

int can_mag_init(GRCAN_NODE_ID mag_ID, CAN_MAG_MSG_ID init_msgID);
int can_mag_send(unsigned int *data);

#endif
