#include "StateData.h"
#include "CANutils.h"
#include "Logomatic.h"

uint32_t lastTickECUStateDataSent = 0;

ECU_StateDataToSend ParseECUStateDataIntoMessages(ECU_StateData *stateData)
{
	ECU_StateDataToSend messages = {
		.ECUState = stateData->ecu_state,
		.StatusBits = {stateData->status_bits[0], stateData->status_bits[1], stateData->status_bits[2]},
		.PowerLevelTorqueMap = stateData->powerlevel_torquemap,
		.MaxCellTemp = (uint8_t)(stateData->max_cell_temp * 4),
		.AccumulatorStateOfCharge = (uint8_t)(stateData->tractivebattery_soc * 51 / 20),
		.GLVStateOfCharge = (uint8_t)(stateData->glv_soc * 51 / 20),
		.TractiveSystemVoltage = (uint16_t)(stateData->ts_voltage * 100),
		.VehicleSpeed = (uint16_t)(stateData->vehicle_speed * 100),
	    .FRWheelRPM = (uint16_t)(stateData->fr_wheel_rpm * 10 + 32768),
		.FLWheelRPM = (uint16_t)(stateData->fl_wheel_rpm * 10 + 32768),
	    .RRWheelRPM = (uint16_t)(stateData->rr_wheel_rpm * 10 + 32768),
		.RLWheelRPM = (uint16_t)(stateData->rl_wheel_rpm * 10 + 32768)
	};

	return messages;
	

void SendECUStateDataOverCAN(ECU_StateData* stateData)
{
	if (lastTickECUStateDataSent <  - ECU_STATE_DATA_SEND_INTERVAL_MS)
	{
		lastTickECUStateDataSent = HAL_GetTick();
		ECU_StateDataToSend messages = ParseECUStateDataIntoMessages(stateData);
	}
}
}