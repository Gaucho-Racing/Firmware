#include <StateMachine.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef STATE_STATEDATA_H
#define STATE_STATEDATA_H

typedef volatile struct {
	uint16_t Accumulator_Voltage;
	uint8_t Accumulator_SOC;
	uint8_t Max_Cell_Temp;

	CCU_STATE state;

	bool recv_charge_cmd;
	bool recv_stop_cmd;

	// BCU_STATUS_2

	// Error & Warnings
	// uint8_t BCU_S2_ERROR_BITS;
	bool BCU_S2_OVERTEMP_ERROR;
	bool BCU_S2_OVERVOLT_ERROR;
	bool BCU_S2_UNDERVOLT_ERROR;
	bool BCU_S2_OVERCURR_ERROR;
	bool BCU_S2_UNDERCURR_ERROR;
	bool BCU_S2_UNDER20v_WARNING;
	bool BCU_S2_UNDER12v_WARNING;
	bool BCU_S2_UNDERVOLTSDC_WARNING;
	// bool BCU_S2_PRECHARGE_ERROR;

	// State
	// uint8_t BCU_S2_PRECHARGE_BITS;
	bool BCU_S2_IR_MINUS; // IR- State
	bool BCU_S2_IR_PLUS;  // IR+ State
	bool SOFTWARE_LATCH;

	// BCU_PRECHARGE
	// bool PRECHARGE_SET_TS_ACTIVE_FLAG;

	// CCU_Precharge_Step precharge_step;
} CCU_StateData;

extern CCU_StateData state_data;

#endif
