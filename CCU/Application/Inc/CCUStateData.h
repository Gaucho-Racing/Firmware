#include <stdbool.h>
#include <stdint.h>

#include "StateMachine.h"

#ifndef STATE_STATEDATA_H
#define STATE_STATEDATA_H

typedef volatile struct {
	uint16_t Accumulator_Voltage;
	uint8_t Accumulator_SOC;
	uint8_t Max_Cell_Temp;

	CCU_STATE state;

	bool recv_charge_cmd;
	bool recv_stop_cmd;

	// ACU_STATUS_2

	// Error & Warnings
	bool ACU_S2_OVERTEMP_ERROR;
	bool ACU_S2_OVERVOLT_ERROR;
	bool ACU_S2_UNDERVOLT_ERROR;
	bool ACU_S2_OVERCURR_ERROR;
	bool ACU_S2_UNDERCURR_ERROR;
	bool ACU_S2_UNDER20v_WARNING;
	bool ACU_S2_UNDER12v_WARNING;
	bool ACU_S2_UNDERVOLTSDC_WARNING;

	// State
	bool IR_MINUS; // IR- State
	bool IR_PLUS;  // IR+ State
	bool SOFTWARE_LATCH;

} CCU_StateData;

extern CCU_StateData state_data;

#endif
