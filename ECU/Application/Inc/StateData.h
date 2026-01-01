#include <stdbool.h>
#include <stdint.h>

#include "GR_OLD_MSG_DAT.h"
#include "StateMachine.h"
#include "main.h"

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
typedef union ECU_StateData {
	struct {
		GR_ECU_State currentState;
		uint8_t StatusBits[3];
		uint8_t PowerLevelTorqueMap;
		uint8_t MaxCellTemp;
		uint8_t AccumulatorStateOfCharge;
		uint8_t GLVStateOfCharge;

		uint16_t TractiveSystemVoltage;
		uint16_t VehicleSpeed;
		int16_t FRWheelRPM;
		int16_t FLWheelRPM;

		int16_t RRWheelRPM;
		int16_t RLWheelRPM;

		uint16_t APPS1_SIGNAL;
		uint16_t APPS2_SIGNAL;
		uint16_t AUX_SIGNAL;
		uint16_t BRAKE_F_SIGNAL;
		uint16_t BRAKE_R_SIGNAL;

		int32_t dischargeStartMillis;
		uint32_t lastECUStatusMsgTick;
		uint32_t lastTSSIFlash;
		bool bse_apps_violation;
		int32_t last_drive_active_control_ms;
		uint16_t driving_heat_capacity_1;
		uint16_t driving_heat_capacity_2;
		float min_amk_heat_cap_throttle_percent;
		uint8_t acu_error_warning_bits;
		uint8_t inverter_fault_map;
		bool ts_active;
		bool rtd_engaged;
	};

	struct {
		GR_OLD_ECU_STATUS_1_MSG ecuStatus1;
		GR_OLD_ECU_STATUS_2_MSG ecuStatus2;
		GR_OLD_ECU_STATUS_3_MSG ecuStatus3;
		uint8_t internalState[25]; // FIXME BE AWARE OF BYTE PACKING!
					   // WHAT THIS REPLACES IS BAD AND WILL
					   // BREAK SOON
	};
} ECU_StateData;

#endif
