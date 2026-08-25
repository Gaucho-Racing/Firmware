#include "CANutils.h"

#include <stdint.h>

#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateTicks.h"
#include "StateUtils.h"
#include "CubeCAN.h"
#include "main.h"
#include "stm32g4xx_hal_fdcan.h"
#include "string.h"

extern ECU_StateData stateLump;

uint32_t lastTickECUStateDataSent = 0;


//FIXME: Help double check this is correct, current no build errors on debug

void ECU_CAN_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size)
{
	CubeCAN_Handle *handle = NULL;

	switch(bus) {
		case GRCAN_BUS_PRIMARY:
			handle = stateLump.primary_can;
			break;
		case GRCAN_BUS_DATA:
			handle = stateLump.data_can;
			break;
		default:
			LOGOMATIC_ERROR("CAN: Invalid bus ID %d \n", bus);
			return;
	}

	if (handle == NULL){
		LOGOMATIC_ERROR("CAN: handle is NULL for bus %d\n", bus);
		return;
	}

	if (size > ECU_CAN_MAX_DATA_BYTES){
		LOGOMATIC_WARNING("Tried to send more than 64 bytes over CAN\n");
		size = ECU_CAN_MAX_DATA_BYTES;
	}

	CubeCAN_Send(handle, destNode, messageID, data, (uint8_t)size);

}

void ECU_CAN_Send_DTI(GRCAN_CUSTOM_ID msgID, void *data, uint32_t size)
{
	if (stateLump.primary_can == NULL){
		LOGOMATIC_ERROR("CAN: primary handle is NULL\n");
		return;
	}

	uint8_t *msg = (uint8_t *)data;
	for (uint32_t i = 0; i < size / 2; ++i){
		uint8_t temp = msg[i];
		msg[i] = msg[size - i -1];
		msg[size - i - 1] = temp;
	}

	CubeCAN_Send(stateLump.primary_can, GRCAN_ALL, (GRCAN_MSG_ID)msgID, msg, (uint8_t)size);
}



//Didnt want to delete this in case the change I made aren't necessary

// void ECU_CAN_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size)
// {
// 	if (size > FDCAN_MAX_DATA_BYTES) {
// 		size = FDCAN_MAX_DATA_BYTES;
// 		LOGOMATIC_ERROR("Tried to send more than 64 bytes over CAN\n");
// 	}

// 	uint32_t ID = ((0xFF & GRCAN_ECU) << 20) | ((0xFFF & messageID) << 8) | (0xFF & destNode);

// 	FDCAN_TxHeaderTypeDef header = {
// 	    .Identifier = ID,
// 	    .IdType = FDCAN_EXTENDED_ID,
// 	    .TxFrameType = FDCAN_DATA_FRAME,
// 	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
// 	    .DataLength = BytesToCANDLC(size),
// 	    .BitRateSwitch = FDCAN_BRS_OFF,
// 	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
// 	    .FDFormat = (bus == GRCAN_BUS_PRIMARY) ? FDCAN_CLASSIC_CAN : FDCAN_FD_CAN,
// 	    .MessageMarker = 0,
// 	};

// 	FDCANTxMessage msg = {0};
// 	msg.tx_header = header;

// 	memcpy(&(msg.data), data, size);

// 	switch (bus) {
// 		case GRCAN_BUS_PRIMARY:
// 			can_enqueue(stateLump.primary_can, &msg);
// 			break;
// 		case GRCAN_BUS_DATA:
// 			can_enqueue(stateLump.data_can, &msg);
// 			break;
// 		default:
// 			LOGOMATIC_ERROR("CAN: Invalid bus ID %d\n", bus);
// 			break;
// 	}
// }

// // TODO: If you try to send anything but control messages, you are cooked buddy
// // Doesn't actually use Motorola order for multiple fields, just sends the bytes in reverse order
// void ECU_CAN_Send_DTI(GRCAN_CUSTOM_ID msgID, void *data, uint32_t size)
// {
// 	// if ((MSG_DTI_CONTROL_10 & 0xFF) != 0x16) {
// 	// 	LOGOMATIC("NOT A DTI MESSAGE");
// 	// }

// 	FDCAN_TxHeaderTypeDef TxHeader = {
// 	    .IdType = FDCAN_EXTENDED_ID,
// 	    .TxFrameType = FDCAN_DATA_FRAME,
// 	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
// 	    // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
// 	    .BitRateSwitch = FDCAN_BRS_OFF,
// 	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
// 	    .MessageMarker = 0			      // also change this to a real address if you change fifo control
// 	};

// 	TxHeader.Identifier = msgID;
// 	TxHeader.DataLength = size;

// 	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;

// 	FDCANTxMessage msg = {0};
// 	msg.tx_header = TxHeader;

// 	uint8_t temp;
// 	for (uint32_t i = 0; i < size / 2; ++i) {
// 		temp = ((uint8_t *)data)[i];
// 		((uint8_t *)data)[i] = ((uint8_t *)data)[size - i - 1];
// 		((uint8_t *)data)[size - i - 1] = temp;
// 	}

// 	memcpy(&(msg.data), data, size);

// 	// can_send(primary_can, &msg);
// 	can_enqueue(stateLump.primary_can, &msg);
// }

// void SendECUStateDataOverCAN(const ECU_StateData *stateData)
// {
// 	uint32_t currentTime = MillisecondsSinceBoot();

// 	if (lastTickECUStateDataSent > currentTime - ECU_STATE_DATA_SEND_INTERVAL_MS) {
// 		return;
// 	}

// 	lastTickECUStateDataSent = currentTime;

// 	ECU_StateDataToSend messages = {.ECUState = stateData->ecu_state,
// 					.StatusBits = {stateData->status_bits[0], stateData->status_bits[1], stateData->status_bits[2]},
// 					.PowerLevelTorqueMap = (stateData->powerlevel << 4) | (stateData->torquemap & 0x0F),
// 					.MaxCellTemp = (uint8_t)(stateData->max_cell_temp_c * 4),
// 					.AccumulatorStateOfCharge = (uint8_t)(stateData->tractivebattery_soc),
// 					.GLVStateOfCharge = (uint8_t)(stateData->glv_soc),
// 					.TractiveSystemVoltage = (uint16_t)(stateData->ts_voltage * 100),
// 					.VehicleSpeed = (uint16_t)(stateData->vehicle_speed_mph * 100),
// 					.FRWheelRPM = (uint16_t)(stateData->fr_wheel_rpm * 10 + 32768),
// 					.FLWheelRPM = (uint16_t)(stateData->fl_wheel_rpm * 10 + 32768),
// 					.RRWheelRPM = (uint16_t)(stateData->rr_wheel_rpm * 10 + 32768),
// 					.RLWheelRPM = (uint16_t)(stateData->rl_wheel_rpm * 10 + 32768)};

// 	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_ALL, GRCAN_ECU_STATUS_1, (void *)&messages.ECUStatusMsgOne, sizeof(messages.ECUStatusMsgOne));
// 	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_ALL, GRCAN_ECU_STATUS_2, (void *)&messages.ECUStatusMsgTwo, sizeof(messages.ECUStatusMsgTwo));
// 	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_ALL, GRCAN_ECU_STATUS_3, (void *)&messages.ECUStatusMsgThree, sizeof(messages.ECUStatusMsgThree));

// 	SendECUConfigOverCAN(stateData);
// }

// void SendECUAnalogDataOverCAN(const ECU_StateData *stateData)
// {
// 	uint32_t millis_since_boot = MillisecondsSinceBoot();

// 	static uint32_t last_can_tcm_request_millis = 0;
// 	if (millis_since_boot - last_can_tcm_request_millis > 30) {
// 		GRCAN_ECU_ANALOG_DATA_MSG message = {.bspd_signal = stateData->bspd_signal,
// 						     .bse_signal = stateData->bse_signal,
// 						     .apps_1_signal = stateData->APPS1_Signal,
// 						     .apps_2_signal = stateData->APPS2_Signal,
// 						     .brakeline_f_signal = stateData->Brake_F_Signal,
// 						     .brakeline_r_signal = stateData->Brake_R_Signal,
// 						     .steering_angle_signal = stateData->steering_angle_signal,
// 						     .aux_signal = stateData->aux_signal,
// 						     .acc_pedal_travel = CalcAccPedalTravel(stateData) * 65535,
// 						     .brake_pedal_pressure = CalcBrakePressure(stateData)};
// 		ECU_CAN_Send(GRCAN_BUS_DATA, GRCAN_TCM, GRCAN_ECU_ANALOG_DATA, &message, sizeof(message));
// 		last_can_tcm_request_millis = millis_since_boot;
// 	}
// }

// void SendECUConfigOverCAN(const ECU_StateData *stateData)
// {
// 	GRCAN_ECU_CONFIG_MSG message = {.ping_timeout_delay = (uint8_t)(stateData->ping_timeout_delay_ms / 10.0f),
// 					.brake_f_min = (uint8_t)(stateData->brake_f_min / 25.0f),
// 					.brake_r_min = (uint8_t)(stateData->brake_r_min / 25.0f),
// 					.brake_bse_min = (uint8_t)(stateData->brake_bse_min / 25.0f),
// 					.apps_1_min = (uint8_t)(stateData->apps_1_min / 10.0f),
// 					.apps_2_min = (uint8_t)(stateData->apps_2_min / 10.0f),
// 					.apps_1_max = (uint8_t)(stateData->apps_1_max / 10.0f),
// 					.apps_2_max = (uint8_t)(stateData->apps_2_max / 10),
// 					.apps_deadzone = (uint8_t)(stateData->apps_deadzone * 25.5f),
// 					.bms_min_threshold = (uint8_t)(stateData->bms_min_thresh * 20.0f),
// 					.bms_max_threshold = (uint8_t)(stateData->bms_max_thresh * 20.0f),
// 					.imd_min_threshold = (uint8_t)(stateData->imd_min_thresh * 20.0f),
// 					.imd_max_threshold = (uint8_t)(stateData->imd_max_thresh * 20.0f),
// 					.bspd_min_threshold = (uint8_t)(stateData->bspd_min_thresh * 20.0f),
// 					.bspd_max_threshold = (uint8_t)(stateData->bspd_max_thresh * 20.0f),
// 					.max_precharge_time = (uint8_t)(stateData->max_precharge_time_ms / 10),
// 					.regen_strength = (uint8_t)(stateData->regen_strength * 10),
// 					.enable_regen = stateData->enable_regen};

// 	ECU_CAN_Send(GRCAN_BUS_DATA, GRCAN_TCM, GRCAN_ECU_CONFIG, &message, sizeof(message));
// }
