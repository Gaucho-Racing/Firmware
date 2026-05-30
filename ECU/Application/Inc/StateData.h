#include <stdbool.h>
#include <stdint.h>

#include "GRCAN_MSG_DATA.h"
#include "StateMachine.h"
#include "can.h"

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

	float apps_deadzone;
	float bms_min_thresh;
	float bms_max_thresh;
	float imd_min_thresh;
	float imd_max_thresh;
	float bspd_min_thresh;
	float bspd_max_thresh;

	float regen_strength;

	// TODO: Remove unneeded states

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
	float bms_sense;
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
	uint16_t steering_angle_signal;
	uint16_t ping_timeout_delay_ms;
	uint16_t brake_f_min;
	uint16_t brake_r_min;
	uint16_t brake_bse_min;
	uint16_t apps_1_min;
	uint16_t apps_2_min;
	uint16_t apps_1_max;
	uint16_t apps_2_max;
	uint16_t max_precharge_time_ms;

	uint8_t status_bits[3];
	int8_t ping_block[3]; /** Node timeout status bits (1=OK, 0=Timeout) */

	uint8_t powerlevel; // 4 bits wide
	uint8_t torquemap;  // 4 bits wide

	uint8_t tractivebattery_soc;
	uint8_t glv_soc;
	uint8_t acu_error_warning_bits;
	uint8_t inverter_fault_map;

	bool ts_active_button_press_interrupt;
	bool ts_active_button_pressed;
	bool rtd_button_press_interrupt;
	bool rtd_button_pressed;
	bool ir_plus;
	bool ir_minus;
	bool acu_software_latch;

	bool bms_light;
	bool imd_light;
	bool tssi_fault;

	bool enable_regen;

	bool apps_bse_violation;

	GR_ECU_State ecu_state;

	CANHandle *primary_can;
	CANHandle *data_can;
} ECU_StateData;

#endif
