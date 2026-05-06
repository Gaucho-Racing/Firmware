#include <StateMachine.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef STATE_STATEDATA_H
#define STATE_STATEDATA_H

typedef volatile struct {

	CCU_STATE state;

	bool recv_charge_cmd;
	bool recv_stop_cmd;

	// ACU_STATUS_2

	// Error & Warnings
	// uint8_t ACU_S2_ERROR_BITS;
	bool ACU_S2_OVERTEMP_ERROR;
	bool ACU_S2_OVERVOLT_ERROR;
	bool ACU_S2_UNDERVOLT_ERROR;
	bool ACU_S2_OVERCURR_ERROR;
	bool ACU_S2_UNDERCURR_ERROR;
	bool ACU_S2_UNDER20v_WARNING;
	bool ACU_S2_UNDER12v_WARNING;
	bool ACU_S2_UNDERVOLTSDC_WARNING;
	// bool ACU_S2_PRECHARGE_ERROR;

	// State
	// uint8_t ACU_S2_PRECHARGE_BITS;
	bool ACU_S2_IR_MINUS; // IR- State
	bool ACU_S2_IR_PLUS;  // IR+ State
	bool SOFTWARE_LATCH;

	// ACU_PRECHARGE
	// bool PRECHARGE_SET_TS_ACTIVE_FLAG;

	// CCU_Precharge_Step precharge_step;
} CCU_StateData;

extern CCU_StateData state_data;

#endif
