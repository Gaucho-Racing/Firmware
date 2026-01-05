#include <stdbool.h>
#include <stdint.h>

#include "GR_OLD_MSG_DAT.h"
#include "StateMachine.h"

#ifndef _STATEDATA_H_
#define _STATEDATA_H_

// FIXME Use CAN message structures when available

/**
 * @brief The ECU state data structure.
 *
 * This structure contains all relevant data for the ECU state machine,
 * including current state, status bits, sensor readings, and control inputs.
 *
 * @remark It is passed to each state function on tick to allow state-specific
 * logic to access and modify the ECU's operational data.
 */
typedef struct ECU_StateData {
	GR_OLD_ECU_STATUS_1_MSG ecuStatus1;
	GR_OLD_ECU_STATUS_2_MSG ecuStatus2;
	GR_OLD_ECU_STATUS_3_MSG ecuStatus3;
	int32_t dischargeStartMillis;
	uint32_t lastECUStatusMsgTick;
	uint32_t lastTSSIFlash;
	int32_t last_drive_active_control_ms;
	float min_amk_heat_cap_throttle_percent;
	uint16_t driving_heat_capacity_1;
	uint16_t driving_heat_capacity_2;
	uint16_t APPS1_Signal;
	uint16_t APPS2_Signal;
	uint16_t Brake_R_Signal;
	uint16_t Brake_F_Signal;
	uint8_t acu_error_warning_bits;
	uint8_t inverter_fault_map;
	bool bse_apps_violation;
	bool ts_active_button_engaged;
	bool rtd_button_engaged;
} ECU_StateData; // FIXME Add comments to each data field with descriptions and
		 // rules (eg -1 = invalid?, etc)

#endif
