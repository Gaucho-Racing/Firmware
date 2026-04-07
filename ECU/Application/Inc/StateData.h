#include <stdbool.h>
#include <stdint.h>

#include "GRCAN_MSG_DATA.h"
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

#define SAFE_VOLTAGE_LIMIT 60
#define BUTTON_REFRESH_RATE_MS 100
#define CRITICAL_MAX_CELL_TEMP_C 60
#define CRITICAL_TS_VOLTAGE 600

typedef union {
	struct {
		uint8_t ECUState;
		uint8_t StatusBits[3];
		uint8_t PowerLevelTorqueMap;
		uint8_t MaxCellTemp;
		uint8_t AccumulatorStateOfCharge;
		uint8_t GLVStateOfCharge;
		uint16_t TractiveSystemVoltage;
		uint16_t VehicleSpeed;
		uint16_t FRWheelRPM;
		uint16_t FLWheelRPM;
		uint16_t RRWheelRPM;
		uint16_t RLWheelRPM;
	};

	struct {
		uint8_t ECUStatusMsgOne[8];
		uint8_t ECUStatusMsgTwo[8];
		uint8_t ECUStatusMsgThree[4];
	};
} ECU_StateDataToSend;

typedef volatile struct ECU_StateData {

	// TODO: Remove unneeded states

	uint32_t millisSinceBoot;
	uint32_t can_msg_cooldown_tick;

	int32_t dischargeStartMillis;
	uint32_t lastECUStatusMsgMillis;
	uint32_t lastTSSIFlash;
	int32_t last_drive_active_control_ms;

	float min_amk_heat_cap_throttle_percent;
	float ts_voltage;
	float max_cell_temp_c; /** Temperature of hottest cell, celsius */

	float vehicle_speed_mph; /** Vehicle speed, MPH */
	float fr_wheel_rpm;	 /** FR wheel, RPM */
	float fl_wheel_rpm;	 /** FL wheel, RPM */
	float rr_wheel_rpm;	 /** RRv wheel, RPM */
	float rl_wheel_rpm;	 /** RL wheel, RPM */

	// 0.5V when things go to shit (X_OK low) (BAD)
	// 3V when things almost poggers (X_OK high but SDC not reset) (BAD)
	// 2.4V when things are actually poggers (X_OK high and SDC is not triggered)
	float ams_sense;
	float imd_sense;
	float bspd_sense;

	float estop_sense;
	float driving_heat_capacity_1;
	float driving_heat_capacity_2;
	uint16_t bse_signal;
	uint16_t bspd_signal;
	uint16_t APPS1_Signal;
	uint16_t APPS2_Signal;
	uint16_t Brake_R_Signal;
	uint16_t Brake_F_Signal;
	uint16_t aux_signal;
	uint8_t status_bits[3];
	int8_t ping_block[3];	      /** Node timeout status bits (1=OK, 0=Timeout) */
	uint8_t powerlevel_torquemap; /** Power lvl (4b) & torque map (4b) */
	uint8_t tractivebattery_soc;
	uint8_t glv_soc;
	uint8_t bcu_error_warning_bits;
	uint8_t inverter_fault_map;
	bool ts_active_button_active;
	bool rtd_button_active;
	bool ir_plus;
	bool ir_minus;
	bool bcu_software_latch;

	bool bms_light;
	bool imd_light;
	bool tssi_fault;

	bool apps_bse_violation;

	GR_ECU_State ecu_state;
} ECU_StateData;

#endif
