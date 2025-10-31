#include <stdint.h>

#include "StateMachine.h"

#ifndef _STATEDATA_H_
#define _STATEDATA_H_

/**
 * @brief The ECU state data structure.
 *
 * This structure contains all relevant data for the ECU state machine,
 * including current state, status bits, sensor readings, and control inputs.
 *
 * It is passed to each state function on tick to allow state-specific logic
 * to access and modify the ECU's operational data.
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
	};

	struct {
		// TODO Add CAN messages here
	};
} ECU_StateData;

#endif
