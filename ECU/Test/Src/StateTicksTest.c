#include "StateTicks.h"

#include <stdint.h>

#include "Logomatic.h"
#include "StateData.h"
#include "StateUtils.h"
#include "can.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

/*
- GLV ON
    - LOGOMATIC_INFO working
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
	// EV.5.11.5: Flash, 2 Hz to 5 Hz, 50% duty cycle
	//     Here we chose a period of 350ms
	if (bmsFailure(stateLumpTest) || imdFailure(stateLumpTest)) {
		LOGOMATIC_INFO("TSSI: TS Faulty\n");
	} else {
		LOGOMATIC_INFO("TSSI: TS Normal\n");
	}

	if (stateLumpTest->ts_active_button_press_interrupt) {
		stateLumpTest->ts_active_button_press_interrupt = false;
		stateLumpTest->ts_active_button_pressed = true;
	} else {
		stateLumpTest->ts_active_button_pressed = false;
	}

	if (stateLumpTest->rtd_button_press_interrupt) {
		stateLumpTest->rtd_button_press_interrupt = false;
		stateLumpTest->rtd_button_pressed = true;
	} else {
		stateLumpTest->rtd_button_pressed = false;
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
			LOGOMATIC_INFO("ECU Current State Unknown: %d\n", stateLumpTest->ecu_state);
			LOGOMATIC_INFO("ECU: Resetting to GLV On\n");
			stateLumpTest->ecu_state = GR_GLV_ON;
			break;
	}
}

int main(void)
{
	ECU_StateData defaultState = {// Start on GLV On
				      .ecu_state = GR_GLV_ON,
				      // Assume ACU good at boot
				      .acu_software_latch = 1,
				      // Startup at minimum power
				      .powerlevel = 0,
				      // See CANdo specification
				      .torquemap = 1,
				      // APPS Deadzone
				      .apps_deadzone = 0.08f,
				      // BMS thresholds
				      .bms_min_thresh = 0.3f,
				      .bms_max_thresh = 1.6f,
				      // IMD thresholds
				      .imd_min_thresh = 0.3f,
				      .imd_max_thresh = 1.6f,
				      // BSPD thresholds
				      .bspd_min_thresh = 0.6f,
				      .bspd_max_thresh = 1.35f,
				      // Timings
				      .ping_timeout_delay_ms = 250,
				      .max_precharge_time_ms = 8000,
				      // Pedals
				      .brake_f_min = 720,
				      .brake_r_min = 700,
				      .brake_bse_min = 720, // Same exact input as brake f
				      .apps_1_min = 2375,
				      .apps_2_min = 2430,
				      .apps_1_max = 1897,
				      .apps_2_max = 1926,
				      // Regen
				      .regen_strength = 2,
				      .enable_regen = false};

	defaultState.bms_sense = 1.5;
	defaultState.imd_sense = 1.5;
	defaultState.bspd_sense = 1.2;

	{
		// ###########################
		// ## Step 0.0              ##
		// ###########################
		LOGOMATIC_INFO("State Ticks test started\n");
		ECU_StateData stateLumpTest = defaultState;

		LOGOMATIC_INFO("Check GLV ON at boot\n");
		stateLumpTest.ecu_state = GR_GLV_ON;
		stateLumpTest.acu_software_latch = 1;
		ECU_Pseudo_State_Tick(&stateLumpTest);

		// ##########################
		// ## Step 0.1             ##
		// ##########################
		LOGOMATIC_INFO("Press and release RTD -> STAY IN GLV ON\n");
		stateLumpTest.rtd_button_press_interrupt = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC_INFO("0.1 Failure: ecu state not in GLV ON\n");
			return 1;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.1 Failure: BMS or IMD reports faulty\n");
			return 1;
		}

		// ##########################
		// ## Step 0.2             ##
		// ##########################
		LOGOMATIC_INFO("Press throttle (1 and 2): STAY IN GLV ON\n");
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_max;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_max;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC_INFO("0.2 Failure: ecu state not in GLV ON\n");
			return 2;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.2 Failure: BMS or IMD reports faulty\n");
			return 1;
		}
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_min;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_min;

		LOGOMATIC_INFO("Press brake: STAY IN GLV ON\n");
		stateLumpTest.bse_signal = stateLumpTest.brake_bse_min + 69;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC_INFO("0.2 Failure: ecu state not in GLV ON\n");
			return 2;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.2 Failure: BMS or IMD reports faulty\n");
			return 2;
		}

		LOGOMATIC_INFO("Release brake: STAY IN GLV ON\n");
		stateLumpTest.bse_signal = 0;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC_INFO("0.2 Failure: ecu state not in GLV ON\n");
			return 2;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.2 Failure: BMS or IMD reports faulty\n");
			return 2;
		}

		// ##########################
		// ## Step 0.3             ##
		// ##########################
		LOGOMATIC_INFO("Press TS ACTIVE: Go to PRECHARGE ENGAGE\n");
		stateLumpTest.ts_active_button_press_interrupt = true;
		stateLumpTest.ir_minus = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_ENGAGED) {
			LOGOMATIC_INFO("0.3 Failure: ecu state not in precharge engaged\n");
			return 3;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.3 Failure: BMS or IMD reports faulty\n");
			return 3;
		}

		// ##########################
		// ## Step 0.4             ##
		// ##########################
		if (stateLumpTest.ecu_state != GR_PRECHARGE_ENGAGED) {
			LOGOMATIC_INFO("0.4 Failure: ecu state not in precharge engaged\n");
			return 4;
		}

		// ##########################
		// ## Step 0.5            ##
		// ##########################
		LOGOMATIC_INFO("Test Precharge Complete (IR PLUS)\n");
		stateLumpTest.ir_plus = true;
		stateLumpTest.ts_voltage = 400; // dummy value
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC_INFO("0.5 Failure: ecu state not in precharge complete\n");
			return 5;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.5 Failure: BMS or IMD reports faulty\n");
			return 5;
		}

		// ##########################
		// ## Step 0.6             ##
		// ##########################
		LOGOMATIC_INFO("Press RTD -> STAY IN PRECHARGE COMPLETE\n");
		stateLumpTest.rtd_button_press_interrupt = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC_INFO("0.6 Failure: ecu state not in precharge complete\n");
			return 6;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.6 Failure: BMS or IMD reports faulty\n");
			return 6;
		}
		LOGOMATIC_INFO("Release RTD -> STAY IN PRECHARGE COMPLETE\n");
		stateLumpTest.rtd_button_press_interrupt = false;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC_INFO("0.6 Failure: ecu state not in precharge complete\n");
			return 6;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.6 Failure: BMS or IMD reports faulty\n");
			return 6;
		}

		// ##########################
		// ## Step 0.7             ##
		// ##########################
		LOGOMATIC_INFO("Press and release the RTD button WHILE pressing the brake\n");
		stateLumpTest.bse_signal = stateLumpTest.brake_bse_min + 69;
		LOGOMATIC_INFO("Press RTD\n");
		stateLumpTest.rtd_button_press_interrupt = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		LOGOMATIC_INFO("Release RTD\n");
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC_INFO("0.7 Failure: ecu state not in drive active\n");
			return 7;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.7 Failure: BMS or IMD reports faulty\n");
			return 7;
		}

		// ##########################
		// ## Step 0.8             ##
		// ##########################
		LOGOMATIC_INFO("Release Brakes -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.bse_signal = 0;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC_INFO("0.8 Failure: ecu state not in drive active\n");
			return 8;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.8 Failure: BMS or IMD reports faulty\n");
			return 8;
		}

		// ##########################
		// ## Step 0.9             ##
		// ##########################
		LOGOMATIC_INFO("Press Throttle -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_max;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_max;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC_INFO("0.9 Failure: ecu state not in drive active\n");
			return 9;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.9 Failure: BMS or IMD reports faulty\n");
			return 9;
		}

		// ##########################
		// ## Step 0.10            ##
		// ##########################
		LOGOMATIC_INFO("Release Throttle -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = 0;
		stateLumpTest.APPS2_Signal = 0;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC_INFO("0.10 Failure: ecu state not in drive active\n");
			return 10;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.10 Failure: BMS or IMD reports faulty\n");
			return 10;
		}

		// ##########################
		// ## Step 0.11            ##
		// ##########################
		LOGOMATIC_INFO("Press Throttle and Brake -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_max;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_max;
		stateLumpTest.bse_signal = stateLumpTest.brake_bse_min + 69;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC_INFO("0.11 Failure: ecu state not in drive active\n");
			return 11;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.11 Failure: BMS or IMD reports faulty\n");
			return 11;
		}

		// ##########################
		// ## Step 0.12            ##
		// ##########################
		LOGOMATIC_INFO("Release Throttle and Brake-> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_min;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_min;
		stateLumpTest.bse_signal = 0;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC_INFO("0.12 Failure: ecu state not in drive active\n");
			return 12;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.12 Failure: BMS or IMD reports faulty\n");
			return 12;
		}

		// ##########################
		// ## Step 0.13             ##
		// ##########################
		LOGOMATIC_INFO("Press Throttle -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_max;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_max;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC_INFO("0.13 Failure: ecu state not in drive active\n");
			return 13;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.13 Failure: BMS or IMD reports faulty\n");
			return 13;
		}

		// ##########################
		// ## Step 0.14            ##
		// ##########################
		LOGOMATIC_INFO("Release Throttle -> STAY IN DRIVE ACTIVE\n");
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_min;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_min;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_DRIVE_ACTIVE) {
			LOGOMATIC_INFO("0.14 Failure: ecu state not in drive active\n");
			return 14;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.14 Failure: BMS or IMD reports faulty\n");
			return 14;
		}

		// ##########################
		// ## Step 0.15            ##
		// ##########################
		LOGOMATIC_INFO("Press RTD -> MOVE TO PRECHARGE COMPLETE\n");
		stateLumpTest.rtd_button_press_interrupt = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC_INFO("0.15 Failure: ecu state not in precharge complete\n");
			return 15;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.15 Failure: BMS or IMD reports faulty\n");
			return 15;
		}
		LOGOMATIC_INFO("Release RTD -> STAY IN PRECHARGE COMPLETE\n");
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC_INFO("0.15 Failure: ecu state not in precharge complete\n");
			return 15;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.15 Failure: BMS or IMD reports faulty\n");
			return 15;
		}

		// ##########################
		// ## Step 0.16             ##
		// ##########################
		LOGOMATIC_INFO("Press Throttle -> STAY IN Precharge Complete\n");
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_max;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_max;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC_INFO("0.16 Failure: ecu state not in precharge complete\n");
			return 16;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.16 Failure: BMS or IMD reports faulty\n");
			return 16;
		}

		// ##########################
		// ## Step 0.17            ##
		// ##########################
		LOGOMATIC_INFO("Release Throttle -> STAY IN Precharge Complete\n");
		stateLumpTest.APPS1_Signal = stateLumpTest.apps_1_min;
		stateLumpTest.APPS2_Signal = stateLumpTest.apps_2_min;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_PRECHARGE_COMPLETE) {
			LOGOMATIC_INFO("0.17 Failure: ecu state not in precharge complete\n");
			return 17;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.17 Failure: BMS or IMD reports faulty\n");
			return 17;
		}

		// ##########################
		// ## Step 0.18            ##
		// ##########################
		LOGOMATIC_INFO("Press TS Active Button -> MOVE to TS DISCHARGE\n");
		stateLumpTest.ts_active_button_press_interrupt = true;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_TS_DISCHARGE) {
			LOGOMATIC_INFO("0.18 Failure: ecu state not in ts discharge\n");
			return 18;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.18 Failure: BMS or IMD reports faulty\n");
			return 18;
		}

		LOGOMATIC_INFO("Release TS Active Button -> STAY IN TS DISCHARGE\n");
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_TS_DISCHARGE) {
			LOGOMATIC_INFO("0.18 Failure: ecu state not in ts discharge\n");
			return 18;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.18 Failure: BMS or IMD reports faulty\n");
			return 18;
		}

		// ##########################
		// ## Step 0.19            ##
		// ##########################
		LOGOMATIC_INFO("TS Voltage Less than 60 -> MOVE to GLV ON\n");
		stateLumpTest.ts_voltage = 40;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		stateLumpTest.ir_minus = !stateLumpTest.ir_minus;
		stateLumpTest.ir_plus = !stateLumpTest.ir_plus;
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC_INFO("0.19 Failure: ecu state not in GLV ON\n");
			return 19;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("0.19 Failure: BMS or IMD reports faulty\n");
			return 19;
		}
	}

	{
		// ##########################
		// ## Step 1.0             ##
		// ##########################
		LOGOMATIC_INFO("Reset system\n");
		ECU_StateData stateLumpTest = defaultState;
		LOGOMATIC_INFO("State Tick Test 1 started\n");
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC_INFO("1.0 Failure: ecu state not in GLV ON\n");
			return 21;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("1.0 Failure: BMS or IMD reports faulty\n");
			return 21;
		}

		// ##########################
		// ## Step 1.1            ##
		// ##########################
		LOGOMATIC_INFO("TS Voltage Greater than 60 -> MOVE to TS DISCHARGE\n");
		stateLumpTest.ts_voltage = 500;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_TS_DISCHARGE) {
			LOGOMATIC_INFO("1.1 Failure: ecu state not in TS DISCHARGE\n");
			return 21;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("1.1 Failure: BMS or IMD reports faulty\n");
			return 21;
		}

		// ##########################
		// ## Step 1.2            ##
		// ##########################
		LOGOMATIC_INFO("TS Voltage Less than 60 -> MOVE to GLV ON\n");
		stateLumpTest.ts_voltage = 40;
		ECU_Pseudo_State_Tick(&stateLumpTest);
		if (stateLumpTest.ecu_state != GR_GLV_ON) {
			LOGOMATIC_INFO("1.2 Failure: ecu state not in GLV ON\n");
			return 22;
		}
		if (bmsFailure(&stateLumpTest) || imdFailure(&stateLumpTest)) {
			LOGOMATIC_INFO("1.2 Failure: BMS or IMD reports faulty\n");
			return 21;
		}
	}
}
