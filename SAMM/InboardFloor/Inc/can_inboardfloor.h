#ifndef CAN_SUS_H
#define CAN_SUS_H

#include "GRCAN_FancyLayer.h"
#include "Logomatic.h"
#include "NodeID.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
	BrakeTemp_FL = GRCAN_BrakeTemp_FL,
	BrakeTemp_FR = GRCAN_BrakeTemp_FR,
	BrakeTemp_RL = GRCAN_BrakeTemp_RL,
	BrakeTemp_RR = GRCAN_BrakeTemp_RR
} BrakeTemp_Node;

typedef enum {
    InboardFloor_FL = GRCAN_InboardFloor_FL,
    InboardFloor_FR = GRCAN_InboardFloor_FR,
    InboardFloor_RL = GRCAN_InboardFloor_RL,
    InboardFloor_RR = GRCAN_InboardFloor_RR
} InboardFloor_Node;

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

	/* Time of Flight Data */
    // TODO: figure out how to get these values from tof results struct when more awake
    uint16_t distance_mm;
    uint16_t ambient_rate_kcps;
    uint16_t ambient_per_spad_kcps;
    uint16_t signal_rate_kcps;
    uint16_t signal_per_spad_kcps;
    uint16_t number_of_spad;
    uint16_t sigma_mm;
	uint8_t range_status;
} IMU_ToF_Data;

void BrakeTemp_Callback(uint32_t id, void *data, uint32_t size);
void TCM_Callback(uint32_t id, void *data, uint32_t size);
int InboardFloor_CAN_Init(GRCAN_BUS_ID busID);
bool InboardFloor_CAN_Send(GRCAN_NODE_ID dest_node, GRCAN_MSG_ID msg_id, void *data);

#endif
