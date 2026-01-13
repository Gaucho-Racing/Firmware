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

	// TODO: Remove unneeded states
	int32_t dischargeStartMillis;
	uint32_t lastECUStatusMsgTick;
	uint32_t lastTSSIFlash;
	int32_t last_drive_active_control_ms;

	float min_amk_heat_cap_throttle_percent;
	float ts_voltage;
	float max_cell_temp;	      /** Temperature of hottest cell, celsius */

	float vehicle_speed; /** Vehicle speed, MPH */
	float fr_wheel_rpm;	/** FR wheel, RPM */
	float fl_wheel_rpm;	/** FL wheel, RPM */
	float rr_wheel_rpm; /** RRv wheel, RPM */
	float rl_wheel_rpm; /** RL wheel, RPM */

	// 0.5V when things go to shit (X_OK low)
	// 3V when things almost poggers (X_OK high but SDC not reset)
	// 2.4V when things are actually poggers (X_OK high and SDC is not triggered)
	float ams_sense;
	float imd_sense;
	float bspd_sense;

	float estop_sense;
	uint16_t driving_heat_capacity_1;
	uint16_t driving_heat_capacity_2;
	uint16_t APPS1_Signal;
	uint16_t APPS2_Signal;
	uint16_t Brake_R_Signal;
	uint16_t Brake_F_Signal;
	int8_t ping_block[3]; /** Node timeout status bits (1=OK, 0=Timeout) */
	uint8_t powerlevel_torquemap; /** Power lvl (4b) & torque map (4b) */
	uint8_t tractivebattery_soc;  /** Accumulator SoC, 20x/51=% */
	uint8_t glv_soc;	      /** GLV SoC, 20x/51=% */
	uint8_t acu_error_warning_bits;
	uint8_t inverter_fault_map;
	bool bse_apps_violation;
	bool ts_active_button_engaged;
	bool rtd_button_engaged;
	GR_ECU_State ecu_state;
} ECU_StateData; // FIXME Add comments to each data field with descriptions and
		 // rules (eg -1 = invalid?, etc)
		 // Will also need to add information from ADC into this struct
		 // --- such as the APPS and Brake signals after doing smoothing
		 // and whatnot to get the values sane

#endif
