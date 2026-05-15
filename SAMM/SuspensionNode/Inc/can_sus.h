#ifndef CAN_SUS_H
#define CAN_SUS_H

#include "GRCAN_FancyLayer.h"
#include "Logomatic.h"
#include "NodeID.h"
#include <stdbool.h>
#include <stdint.h>

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

typedef enum {
	Sus_FL = GRCAN_Suspension_FL,
	Sus_FR = GRCAN_Suspension_FR,
	Sus_RL = GRCAN_Suspension_RL,
	Sus_RR = GRCAN_Suspension_RR
} Sus_Node;

typedef struct {
	/* IMU Data */
	uint16_t bmi323_acc_x;
	uint16_t bmi323_acc_y;
	uint16_t bmi323_acc_z;
	uint16_t bmi323_gyro_x;
	uint16_t bmi323_gyro_y;
	uint16_t bmi323_gyro_z;
	uint16_t bmi323_temp;
	uint16_t bmi323_status;

	/* Mag Encoder Data */
	uint16_t mag_temp;
	uint16_t mag_hysteresis;
	uint16_t mag_angle;
	int16_t mag_turns;
	uint8_t mag_status;
} IMU_Mag_Data;

void TireTemp_Callback(uint32_t id, void *data, uint32_t size);
void TCM_Callback(uint32_t id, void *data, uint32_t size);
int SusNode_CAN_Init(GRCAN_BUS_ID busID);
bool SusNode_CAN_Send(GRCAN_NODE_ID dest_node, GRCAN_MSG_ID msg_id, void *data);

#endif
