#include <stdbool.h>
#include <stdint.h>

#ifndef STATE_STATEDATA_H
#define STATE_STATEDATA_H

#define CHECK_BIT(value, bit) (((value) == (bit)) & 1)

typedef struct {
	// name lwk might be too long
	bool Button_Status;

	// ACU_STATUS_2
	uint8_t ACU_S2_20Volt;
	uint8_t ACU_S2_12Volt;
	uint8_t ACU_S2_SDC_Volt;
	uint8_t ACU_S2_MIN_CELL_Volt;
	uint8_t ACU_S2_MAX_CELL_Volt;
	// Error & Warnings
	uint8_t ACU_S2_ERROR_BITS;
	bool ACU_S2_OVERTEMP_ERROR;
	bool ACU_S2_OVERVOLT_ERROR;
	bool ACU_S2_UNDERVOLT_ERROR;
	bool ACU_S2_OVERCURR_ERROR;
	bool ACU_S2_UNDERCURR_ERROR;
	// bool ACU_S2_UNDER20v_WARNING;
	// bool ACU_S2_UNDER12v_WARNING;
	// bool ACU_S2_UNDERVOLTSDC_WARNING;
	// bool ACU_S2_PRECHARGE_ERROR;

	// State
	uint8_t ACU_S2_PRECHARGE_BITS;
	// bool ACU_S2_PRECHARGE_STATE;
	// bool ACU_S2_STATE;
	// bool ACU_S2_SOFTWARE_LATCH;

	// ACU_STATUS_3
	uint16_t ACU_S3_HV_INPUTVolt;
	uint16_t ACU_S3_HV_OUTPUTVolt;
	uint16_t ACU_S3_HV_INPUTCurr;
	uint16_t ACU_S3_HV_OUTPUTCurr;

	// ACU_PRECHARGE
	bool ACU_PRECHARGE_SET_TS_ACTIVE;

} CCU_StateData;

#endif