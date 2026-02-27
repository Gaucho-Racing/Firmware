#include <StateMachine.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef STATE_STATEDATA_H
#define STATE_STATEDATA_H

typedef struct {
	CCU_STATE state;

	// name lwk might be too long
	bool Button_Status;

	// BCU_STATUS_2
	uint8_t BCU_S2_20Volt;
	uint8_t BCU_S2_12Volt;
	uint8_t BCU_S2_SDC_Volt;
	uint8_t BCU_S2_MIN_CELL_Volt;
	uint8_t BCU_S2_MAX_CELL_TEMP;
	// Error & Warnings
	// uint8_t BCU_S2_ERROR_BITS;
	bool BCU_S2_OVERTEMP_ERROR;
	bool BCU_S2_OVERVOLT_ERROR;
	bool BCU_S2_UNDERVOLT_ERROR;
	bool BCU_S2_OVERCURR_ERROR;
	bool BCU_S2_UNDERCURR_ERROR;
	// bool BCU_S2_UNDER20v_WARNING;
	// bool BCU_S2_UNDER12v_WARNING;
	// bool BCU_S2_UNDERVOLTSDC_WARNING;
	// bool BCU_S2_PRECHARGE_ERROR;

	// State
	uint8_t BCU_S2_PRECHARGE_BITS;
	// bool BCU_S2_PRECHARGE_STATE;
	// bool BCU_S2_IR_STATE;
	bool BCU_S2_SOFTWARE_LATCH;



	// BCU_PRECHARGE
	bool BCU_PRECHARGE_SET_TS_ACTIVE;

} CCU_StateData;

extern CCU_StateData state_data;

#endif
