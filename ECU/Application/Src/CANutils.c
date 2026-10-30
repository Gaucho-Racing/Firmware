#include "CANutils.h"

#include <stdint.h>

#include "CubeCAN.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateTicks.h"
#include "StateUtils.h"
#include "main.h"
#include "stm32g4xx_hal_fdcan.h"
#include "string.h"

extern ECU_StateData stateLump;

uint32_t lastTickECUStateDataSent = 0;

// FIXME: Help double check this is correct, current no build errors on debug

CubeCAN_Handle *ECU_GetCANHandle(GRCAN_BUS_ID bus)
{
	switch (bus) {
		case GRCAN_BUS_PRIMARY:
			return stateLump.primary_can;
		case GRCAN_BUS_DATA:
			return stateLump.data_can;
		default:
			return NULL;
	}
}

HAL_StatusTypeDef ECU_CAN_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, const void *data, uint8_t size)
{
	CubeCAN_Handle *handle = ECU_GetCANHandle(bus);
	if (handle == NULL) {
		LOGOMATIC_ERROR("CAN: no handle for bus %d\n", bus);
		return HAL_ERROR;
	}

	return CubeCAN_Send(handle, destNode, messageID, data, size);
}

HAL_StatusTypeDef ECU_CAN_Send_DTI(GRCAN_CUSTOM_ID msgID, const void *data, uint8_t size)
{
	if (data == NULL || size > 8) {
		return HAL_ERROR;
	}

	uint8_t msg[8];
	const uint8_t *temp = (const uint8_t *)data;
	for (uint8_t i = 0; i < size; ++i) {
		msg[i] = temp[size - 1 - i];
	}

	return CubeCAN_Send_Custom(stateLump.primary_can, msgID, msg, size);
}

// Didnt want to delete this in case the change I made aren't necessary

void SendECUStateDataOverCAN(const ECU_StateData *stateData)
{
	uint32_t currentTime = MillisecondsSinceBoot();

	if (currentTime - lastTickECUStateDataSent < ECU_STATE_DATA_SEND_INTERVAL_MS) {
		return;
	}

	lastTickECUStateDataSent = currentTime;

	ECU_StateDataToSend messages = {.ECUState = stateData->ecu_state,
					.StatusBits = {stateData->status_bits[0], stateData->status_bits[1], stateData->status_bits[2]},
					.ForwardRegenCurrentMap = (stateData->forward_ac_i_code << 4) | (stateData->regen_ac_i_code & 0x0F),
					.MaxCellTemp = (uint8_t)(stateData->max_cell_temp_c * 4),
					.AccumulatorStateOfCharge = (uint8_t)(stateData->tractivebattery_soc),
					.GLVStateOfCharge = (uint8_t)(stateData->glv_soc),
					.TractiveSystemVoltage = (uint16_t)(stateData->ts_voltage * 100),
					.VehicleSpeed = (uint16_t)(stateData->vehicle_speed_mph * 100),
					.FRWheelRPM = (uint16_t)(stateData->fr_wheel_rpm * 10 + 32768),
					.FLWheelRPM = (uint16_t)(stateData->fl_wheel_rpm * 10 + 32768),
					.RRWheelRPM = (uint16_t)(stateData->rr_wheel_rpm * 10 + 32768),
					.RLWheelRPM = (uint16_t)(stateData->rl_wheel_rpm * 10 + 32768)};

	CubeCAN_Handle *primary = ECU_GetCANHandle(GRCAN_BUS_PRIMARY);
	CubeCAN_Send(primary, GRCAN_ALL, GRCAN_ECU_STATUS_1, messages.ECUStatusMsgOne, sizeof(messages.ECUStatusMsgOne));
	CubeCAN_Send(primary, GRCAN_ALL, GRCAN_ECU_STATUS_2, messages.ECUStatusMsgTwo, sizeof(messages.ECUStatusMsgTwo));
	CubeCAN_Send(primary, GRCAN_ALL, GRCAN_ECU_STATUS_3, messages.ECUStatusMsgThree, sizeof(messages.ECUStatusMsgThree));

	SendECUConfigOverCAN(stateData);
}

void SendECUAnalogDataOverCAN(const ECU_StateData *stateData)
{
	uint32_t millis_since_boot = MillisecondsSinceBoot();

	static uint32_t last_can_tcm_request_millis = 0;
	if (millis_since_boot - last_can_tcm_request_millis > 30) {
		GRCAN_ECU_ANALOG_DATA_MSG message = {.bspd_signal = stateData->bspd_signal,
						     .bse_signal = stateData->bse_signal,
						     .apps_1_signal = stateData->APPS1_Signal,
						     .apps_2_signal = stateData->APPS2_Signal,
						     .brakeline_f_signal = stateData->Brake_Signal,
						     .brakeline_r_signal = 0,
						     .steering_angle_signal = stateData->steering_angle_signal,
						     .aux_signal = stateData->aux_signal,
						     .acc_pedal_travel = CalcAccPedalTravel(stateData) * 65535,
						     .brake_pedal_pressure = CalcBrakePressure(stateData)};
		CubeCAN_Send(ECU_GetCANHandle(GRCAN_BUS_DATA), GRCAN_TCM, GRCAN_ECU_ANALOG_DATA, &message, sizeof(message));
		last_can_tcm_request_millis = millis_since_boot;
	}
}

void SendECUConfigOverCAN(const ECU_StateData *stateData)
{
	GRCAN_ECU_CONFIG_MSG message = {.ping_timeout_delay = (uint8_t)(stateData->ping_timeout_delay_ms / 10.0f),
					.brake_f_min = (uint8_t)(stateData->brake_f_min / 25.0f),
					.brake_r_min = (uint8_t)(stateData->brake_r_min / 25.0f),
					.brake_bse_min = (uint8_t)(stateData->brake_bse_min / 25.0f),
					.apps_1_min = (uint8_t)(stateData->apps_1_min / 10.0f),
					.apps_2_min = (uint8_t)(stateData->apps_2_min / 10.0f),
					.apps_1_max = (uint8_t)(stateData->apps_1_max / 10.0f),
					.apps_2_max = (uint8_t)(stateData->apps_2_max / 10),
					.apps_deadzone = (uint8_t)(stateData->apps_deadzone * 25.5f),
					.bms_min_threshold = (uint8_t)(stateData->bms_min_thresh * 20.0f),
					.bms_max_threshold = (uint8_t)(stateData->bms_max_thresh * 20.0f),
					.imd_min_threshold = (uint8_t)(stateData->imd_min_thresh * 20.0f),
					.imd_max_threshold = (uint8_t)(stateData->imd_max_thresh * 20.0f),
					.bspd_min_threshold = (uint8_t)(stateData->bspd_min_thresh * 20.0f),
					.bspd_max_threshold = (uint8_t)(stateData->bspd_max_thresh * 20.0f),
					.max_precharge_time = (uint8_t)(stateData->max_precharge_time_ms / 10),
					.regen_strength = (uint8_t)(stateData->regen_strength * 10),
					.enable_regen = stateData->enable_regen};

	CubeCAN_Send(ECU_GetCANHandle(GRCAN_BUS_DATA), GRCAN_TCM, GRCAN_ECU_CONFIG, &message, sizeof(message));
}
