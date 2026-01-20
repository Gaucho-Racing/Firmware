#include "CANutils.h"

#include "Logomatic.h"
#include "StateData.h"

ECU_StateDataToSend ParseECUStateDataIntoMessages(ECU_StateData *stateData)
{
	ECU_StateDataToSend messages = {.ECUState = stateData->ecu_state,
					.StatusBits = {stateData->status_bits[0], stateData->status_bits[1], stateData->status_bits[2]},
					.PowerLevelTorqueMap = stateData->powerlevel_torquemap,
					.MaxCellTemp = (uint8_t)(stateData->max_cell_temp * 4),
					.AccumulatorStateOfCharge = (uint8_t)(stateData->tractivebattery_soc * 51 / 20),
					.GLVStateOfCharge = (uint8_t)(stateData->glv_soc * 51 / 20),
					.TractiveSystemVoltage = (uint16_t)(stateData->ts_voltage * 0.01),
					.VehicleSpeed = (uint16_t)(stateData->vehicle_speed * 0.01),
					.FRWheelRPM = (uint16_t)(stateData->fr_wheel_rpm * 0.1 - 3276.8),
					.FLWheelRPM = (uint16_t)(stateData->fl_wheel_rpm * 0.1 - 3276.8),
					.RRWheelRPM = (uint16_t)(stateData->rr_wheel_rpm * 0.1 - 3276.8),
					.RLWheelRPM = (uint16_t)(stateData->rl_wheel_rpm * 0.1 - 3276.8)};

	return messages;

	void SendECUStateDataOverCAN(ECU_StateData * stateData)
	{
		ECU_StateDataToSend messages = ParseECUStateDataIntoMessages(stateData);
		// TODO Enqueue messages over CAN bus
	}
}