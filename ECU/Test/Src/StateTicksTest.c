#include "StateTicks.h"

#include <stdint.h>

#include "Logomatic.h"
#include "StateData.h"
#include "StateUtils.h"
#include "ecu_can.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

/*
- GLV ON
    - LOGOMATIC working
- TSSI
- PRECHARGE ENGAGED
- PRECHARGE COMPLETE
- DRIVE ACTIVE
- PRECHARGE COMPLETE
- TS DISCHARGE OFF
- GLV ON/PRECHARGE COMPLETE (ts voltage < 60)
- TS DISCHARGE (ts voltage > 60), then less than 60 -> GLV ON
*/

// static void ECU_Pseudo_Time_Progress(uint32_t dt) { stateLumpTest.millisSinceBoot += dt; }

static void ECU_Pseudo_State_Tick(ECU_StateData *stateLumpTest)
{
	stateLumpTest->tssi_fault = bmsFailure(stateLumpTest) || imdFailure(stateLumpTest);

	// EV.5.11.5: Flash, 2 Hz to 5 Hz, 50% duty cycle
	//     Here we chose a period of 350ms
	if (stateLumpTest->tssi_fault) {
		LOGOMATIC("TSSI: TS Faulty\n");
	} else {
		LOGOMATIC("TSSI: TS Normal\n");
	}

	switch (stateLumpTest->ecu_state) {
		case GR_GLV_OFF:
			ECU_GLV_Off(stateLumpTest);
			break;
		case GR_GLV_ON:
			ECU_GLV_On(stateLumpTest);
			break;
		case GR_PRECHARGE_ENGAGED:
			ECU_Precharge_Engaged(stateLumpTest);
			break;
		case GR_PRECHARGE_COMPLETE:
			ECU_Precharge_Complete(stateLumpTest);
			break;
		case GR_DRIVE_ACTIVE:
			ECU_Drive_Active(stateLumpTest);
			break;
		case GR_TS_DISCHARGE:
			ECU_Tractive_System_Discharge(stateLumpTest);
			break;
		default:
			LOGOMATIC("ECU Current State Unknown: %d\n", stateLumpTest->ecu_state);
			LOGOMATIC("ECU: Resetting to GLV On\n");
			stateLumpTest->ecu_state = GR_GLV_ON;
			break;
	}
}

int main(void)
{
	{
		// ###########################
		// ## Step 0.0              ##
		// ###########################
		LOGOMATIC("State Ticks test started\n");
		ECU_StateData stateLumpTest = {.ecu_state = GR_GLV_ON, .ams_sense = 1.5, .imd_sense = 1.5, .bspd_sense = 1.5};
		LOGOMATIC("Check GLV ON at boot\n");
		stateLumpTest.ecu_state = GR_GLV_ON;
		stateLumpTest.acu_software_latch = 1;
		ECU_Pseudo_State_Tick(&stateLumpTest);

		// ##########################
		// ## Step 0.1             ##
		// ##########################
		LOGOMATIC("Press and release RTD -> STAY IN GLV ON\n");
		stateLumpTest.rtd_button_pressed = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC("0.1 Failure: ecu state not in GLV ON\n");
			return 1;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.1 Failure: TSSI reports faulty\n");
			return 1;
		}

		// ##########################
		// ## Step 0.2             ##
		// ##########################
		LOGOMATIC("Press throttle (1 and 2): STAY IN GLV ON\n");
		stateLumpTest.APPS1_Signal = THROTTLE_MAX_1;
		stateLumpTest.APPS2_Signal = THROTTLE_MAX_2;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC("0.2 Failure: ecu state not in GLV ON\n");
			return 2;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.2 Failure: TSSI reports faulty\n");
			return 1;
		}
		stateLumpTest.APPS1_Signal = 0;
		stateLumpTest.APPS2_Signal = 0;

		LOGOMATIC("Press brake: STAY IN GLV ON\n");
		stateLumpTest.bse_signal = BSE_MAX;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC("0.2 Failure: ecu state not in GLV ON\n");
			return 2;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.2 Failure: TSSI reports faulty\n");
			return 2;
		}

		LOGOMATIC("Release brake: STAY IN GLV ON\n");
		stateLumpTest.bse_signal = 0.0f;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC("0.2 Failure: ecu state not in GLV ON\n");
			return 2;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.2 Failure: TSSI reports faulty\n");
			return 2;
		}

		// ##########################
		// ## Step 0.3             ##
		// ##########################
		LOGOMATIC("Press TS ACTIVE: Go to PRECHARGE ENGAGE\n");
		stateLumpTest.ts_active_button_pressed = true;
		stateLumpTest.ir_minus = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_ENGAGED) {
			LOGOMATIC("0.3 Failure: ecu state not in precharge engaged\n");
			return 3;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.3 Failure: TSSI reports faulty\n");
			return 3;
		}

		// ##########################
		// ## Step 0.4             ##
		// ##########################
		if (stateLumpTest.ecu_state != GR_PRECHARGE_ENGAGED) {
			LOGOMATIC("0.4 Failure: ecu state not in precharge engaged\n");
			return 4;
		}

		// ##########################
		// ## Step 0.5            ##
		// ##########################
		LOGOMATIC("Test Precharge Complete (IR PLUS)\n");
		stateLumpTest.ir_plus = true;
		stateLumpTest.ts_voltage = 400; // dummy value
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC("0.5 Failure: ecu state not in precharge complete\n");
			return 5;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.5 Failure: TSSI reports faulty\n");
			return 5;
		}

		// ##########################
		// ## Step 0.6             ##
		// ##########################
		LOGOMATIC("Press RTD -> STAY IN PRECHARGE COMPLETE\n");
		stateLumpTest.rtd_button_pressed = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC("0.6 Failure: ecu state not in precharge complete\n");
			return 6;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.6 Failure: TSSI reports faulty\n");
			return 6;
		}
		LOGOMATIC("Release RTD -> STAY IN PRECHARGE COMPLETE\n");
		stateLumpTest.rtd_button_pressed = false;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC("0.6 Failure: ecu state not in precharge complete\n");
			return 6;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.6 Failure: TSSI reports faulty\n");
			return 6;
		}

		// ##########################
		// ## Step 0.7             ##
		// ##########################
		LOGOMATIC("Press and release the RTD button WHILE pressing the brake\n");
		stateLumpTest.bse_signal = BSE_MAX;
		LOGOMATIC("Press RTD\n");
		stateLumpTest.rtd_button_pressed = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		LOGOMATIC("Release RTD\n");
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC("0.7 Failure: ecu state not in drive active\n");
			return 7;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.7 Failure: TSSI reports faulty\n");
			return 7;
		}

		// ##########################
		// ## Step 0.8             ##
		// ##########################
		LOGOMATIC("Release Brakes -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.bse_signal = 0.0f;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC("0.8 Failure: ecu state not in drive active\n");
			return 8;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.8 Failure: TSSI reports faulty\n");
			return 8;
		}

		// ##########################
		// ## Step 0.9             ##
		// ##########################
		LOGOMATIC("Press Throttle -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = THROTTLE_MAX_1;
		stateLumpTest.APPS2_Signal = THROTTLE_MAX_2;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC("0.9 Failure: ecu state not in drive active\n");
			return 9;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.9 Failure: TSSI reports faulty\n");
			return 9;
		}

		// ##########################
		// ## Step 0.10            ##
		// ##########################
		LOGOMATIC("Release Throttle -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = 0;
		stateLumpTest.APPS2_Signal = 0;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC("0.10 Failure: ecu state not in drive active\n");
			return 10;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.10 Failure: TSSI reports faulty\n");
			return 10;
		}

		// ##########################
		// ## Step 0.11            ##
		// ##########################
		LOGOMATIC("Press Throttle and Brake -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = THROTTLE_MAX_1;
		stateLumpTest.APPS2_Signal = THROTTLE_MAX_2;
		stateLumpTest.bse_signal = BSE_MAX;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC("0.11 Failure: ecu state not in drive active\n");
			return 11;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.11 Failure: TSSI reports faulty\n");
			return 11;
		}

		// ##########################
		// ## Step 0.12            ##
		// ##########################
		LOGOMATIC("Release Throttle and Brake-> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = 0;
		stateLumpTest.APPS2_Signal = 0;
		stateLumpTest.bse_signal = 0.0f;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC("0.12 Failure: ecu state not in drive active\n");
			return 12;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.12 Failure: TSSI reports faulty\n");
			return 12;
		}

		// ##########################
		// ## Step 0.13             ##
		// ##########################
		LOGOMATIC("Press Throttle -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = THROTTLE_MAX_1;
		stateLumpTest.APPS2_Signal = THROTTLE_MAX_2;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC("0.13 Failure: ecu state not in drive active\n");
			return 13;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.13 Failure: TSSI reports faulty\n");
			return 13;
		}

		// ##########################
		// ## Step 0.14            ##
		// ##########################
		LOGOMATIC("Release Throttle -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = 0;
		stateLumpTest.APPS2_Signal = 0;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC("0.14 Failure: ecu state not in drive active\n");
			return 14;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.14 Failure: TSSI reports faulty\n");
			return 14;
		}

		// ##########################
		// ## Step 0.15            ##
		// ##########################
		LOGOMATIC("Press RTD -> MOVE TO PRECHARGE COMPLETE\n");
		stateLumpTest.rtd_button_pressed = !stateLumpTest.rtd_button_pressed;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC("0.15 Failure: ecu state not in precharge complete\n");
			return 15;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.15 Failure: TSSI reports faulty\n");
			return 15;
		}
		LOGOMATIC("Release RTD -> STAY IN PRECHARGE COMPLETE\n");
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC("0.15 Failure: ecu state not in precharge complete\n");
			return 15;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.15 Failure: TSSI reports faulty\n");
			return 15;
		}

		// ##########################
		// ## Step 0.16             ##
		// ##########################
		LOGOMATIC("Press Throttle -> STAY IN Precharge Complete\n");
		stateLumpTest.APPS1_Signal = THROTTLE_MAX_1;
		stateLumpTest.APPS2_Signal = THROTTLE_MAX_2;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC("0.16 Failure: ecu state not in precharge complete\n");
			return 16;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.16 Failure: TSSI reports faulty\n");
			return 16;
		}

		// ##########################
		// ## Step 0.17            ##
		// ##########################
		LOGOMATIC("Release Throttle -> STAY IN Precharge Complete\n");
		stateLumpTest.APPS1_Signal = 0;
		stateLumpTest.APPS2_Signal = 0;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC("0.17 Failure: ecu state not in precharge complete\n");
			return 17;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.17 Failure: TSSI reports faulty\n");
			return 17;
		}

		// ##########################
		// ## Step 0.18            ##
		// ##########################
		LOGOMATIC("Press TS Active Button -> MOVE to TS DISCHARGE\n");
		stateLumpTest.ts_active_button_pressed = !stateLumpTest.ts_active_button_pressed;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_TS_DISCHARGE) {
			LOGOMATIC("0.18 Failure: ecu state not in ts discharge\n");
			return 18;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.18 Failure: TSSI reports faulty\n");
			return 18;
		}

		LOGOMATIC("Release TS Active Button -> STAY IN TS DISCHARGE\n");
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_TS_DISCHARGE) {
			LOGOMATIC("0.18 Failure: ecu state not in ts discharge\n");
			return 18;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.18 Failure: TSSI reports faulty\n");
			return 18;
		}

		// ##########################
		// ## Step 0.19            ##
		// ##########################
		LOGOMATIC("TS Voltage Less than 60 -> MOVE to GLV ON\n");
		stateLumpTest.ts_voltage = 40;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		stateLumpTest.ir_minus = !stateLumpTest.ir_minus;
		stateLumpTest.ir_plus = !stateLumpTest.ir_plus;
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC("0.19 Failure: ecu state not in GLV ON\n");
			return 19;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("0.19 Failure: TSSI reports faulty\n");
			return 19;
		}
	}

	{
		// ##########################
		// ## Step 1.0             ##
		// ##########################
		LOGOMATIC("Reset system\n");
		ECU_StateData stateLumpTest = {.ecu_state = GR_GLV_ON, .ams_sense = 1.5, .imd_sense = 1.5, .bspd_sense = 1.5};
		LOGOMATIC("State Tick Test 1 started\n");
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC("1.0 Failure: ecu state not in GLV ON\n");
			return 21;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("1.0 Failure: TSSI reports faulty\n");
			return 21;
		}

		// ##########################
		// ## Step 1.1            ##
		// ##########################
		LOGOMATIC("TS Voltage Greater than 60 -> MOVE to TS DISCHARGE\n");
		stateLumpTest.ts_voltage = 500;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_TS_DISCHARGE) {
			LOGOMATIC("1.1 Failure: ecu state not in TS DISCHARGE\n");
			return 21;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("1.1 Failure: TSSI reports faulty\n");
			return 21;
		}

		// ##########################
		// ## Step 1.2            ##
		// ##########################
		LOGOMATIC("TS Voltage Less than 60 -> MOVE to GLV ON\n");
		stateLumpTest.ts_voltage = 40;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC("1.2 Failure: ecu state not in GLV ON\n");
			return 22;
		}
		if (stateLumpTest.tssi_fault) {
			LOGOMATIC("1.2 Failure: TSSI reports faulty\n");
			return 21;
		}
	}
}
