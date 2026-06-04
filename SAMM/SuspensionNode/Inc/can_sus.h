#ifndef CAN_SUS_H
#define CAN_SUS_H

#include <stdbool.h>
#include <stdint.h>

#include "GRCAN_FancyLayer.h"
#include "Logomatic.h"
#include "NodeID.h"

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
