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

	// These states are copied from the GR_OLD_ECU_STATUS_#_MSG structs
	// TODO: Remove unneeded states
	int8_t ping_block[3]; /** Node timeout status bits (1=OK, 0=Timeout) */
	uint8_t powerlevel_torquemap; /** Power lvl (4b) & torque map (4b) */
	uint8_t max_cell_temp;	      /** Hottest cell temp, 0.25°C/bit */
	uint8_t tractivebattery_soc;  /** Accumulator SoC, 20x/51=% */
	uint8_t glv_soc;	      /** GLV SoC, 20x/51=% */
	uint16_t vehicle_speed; /** Vehicle speed, 0.01 MPH */
	uint16_t fr_wheel_rpm;	/** FR wheel, 0.1x-3276.8 RPM */
	uint16_t fl_wheel_rpm;	/** FL wheel, 0.1x-3276.8 RPM */
	uint16_t rr_wheel_rpm; /** RR wheel, 0.1x-3276.8 RPM */
	uint16_t rl_wheel_rpm; /** RL wheel, 0.1x-3276.8 RPM */

	GR_ECU_State ecu_state;
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
	uint16_t ts_voltage;	/** TS output voltage, 0.01V */
	bool rtd_button_engaged;
} ECU_StateData; // FIXME Add comments to each data field with descriptions and
		 // rules (eg -1 = invalid?, etc)
		 // Will also need to add information from ADC into this struct
		 // --- such as the APPS and Brake signals after doing smoothing
		 // and whatnot to get the values sane

#endif
