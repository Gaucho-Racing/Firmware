#include "stm32g4xx_hal.h"
#include "StateTicks.h"

#include <stdint.h>

#include "Logomatic.h"
#include "StateData.h"
#include "StateUtils.h"

/*
- GLV ON
    - printf working
- TSSI
- PRECHARGE ENGAGED
- PRECHARGE COMPLETE
- DRIVE ACTIVE
- PRECHARGE COMPLETE
- TS DISCHARGE OFF
- GLV ON/PRECHARGE COMPLETE (ts voltage < 60)
- TS DISCHARGE (ts voltage > 60), then less than 60 -> GLV ON
*/

ECU_StateData stateLump = {0};

#define ECU_STATUS_MSG_PERIOD_MILLIS (1000)
// EV.5.6.3: The Discharge Circuit must be designed to handle the maximum Tractive System voltage for minimum 15 seconds
#define TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS (15000)

static void ECU_Pseudo_Time_Progress(uint32_t dt) { stateLump.millisSinceBoot += dt; }

static void ECU_Pseudo_State_Tick(void)
{
	if (stateLump.millisSinceBoot - stateLump.lastECUStatusMsgMillis >= ECU_STATUS_MSG_PERIOD_MILLIS) {
		printf("ECU Current State: %d\n", stateLump.ecu_state);
		stateLump.lastECUStatusMsgMillis = stateLump.millisSinceBoot;
	}

	if (bmsFailure(&stateLump) || imdFailure(&stateLump)) {
		stateLump.tssi_fault = true;
	}

	// EV.5.11.5: Flash, 2 Hz to 5 Hz, 50% duty cycle
	//     Here we chose a period of 350ms
	if (stateLump.tssi_fault) {
		printf("TSSI: TS Faulty\n");
	} else {
		printf("TSSI: TS Normal\n");
	}

	switch (stateLump.ecu_state) {
		case GR_GLV_OFF:
			ECU_GLV_Off(&stateLump);
			break;
		case GR_GLV_ON:
			ECU_GLV_On(&stateLump);
			break;
		case GR_PRECHARGE_ENGAGED:
			ECU_Precharge_Engaged(&stateLump);
			break;
		case GR_PRECHARGE_COMPLETE:
			ECU_Precharge_Complete(&stateLump);
			break;
		case GR_DRIVE_ACTIVE:
			ECU_Drive_Active(&stateLump);
			break;
		case GR_TS_DISCHARGE:
			ECU_Tractive_System_Discharge(&stateLump);
			break;
		default:
			printf("ECU Current State Unknown: %d\n", stateLump.ecu_state);
			printf("ECU: Resetting to GLV On\n");
			stateLump.ecu_state = GR_GLV_ON;
			break;
	}
}

int main(void)
{
	printf("State Ticks test started\n");

	printf("Check GLV ON at boot\n");
	stateLump.ecu_state = GR_GLV_ON;
	ECU_Pseudo_State_Tick();

	// ##########################
	// ## Step 0.1             ##
	// ##########################
	printf("Press and release RTD -> STAY IN GLV ON\n");
	stateLump.rtd = true;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_GLV_ON) {
		printf("0.1 Failure: ecu state not in GLV ON\n");
		return 1;
	}
	if (stateLump.tssi_fault) {
		printf("0.1 Failure: TSSI reports faulty\n");
		return 1;
	}

	// ##########################
	// ## Step 0.2             ##
	// ##########################
	printf("Press throttle (1 and 2): STAY IN GLV ON\n");
	stateLump.APPS1_Signal = THROTTLE_MAX_1;
	stateLump.APPS2_Signal = THROTTLE_MAX_2;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_GLV_ON) {
		printf("0.2 Failure: ecu state not in GLV ON\n");
		return 2;
	}
	if (stateLump.tssi_fault) {
		printf("0.2 Failure: TSSI reports faulty\n");
		return 1;
	}
	stateLump.APPS1_Signal = 0;
	stateLump.APPS2_Signal = 0;

	printf("Press brake: STAY IN GLV ON\n");
	stateLump.Brake_F_Signal = BRAKE_F_MAX;
	stateLump.Brake_R_Signal = BRAKE_R_MAX;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_GLV_ON) {
		printf("0.2 Failure: ecu state not in GLV ON\n");
		return 2;
	}
	if (stateLump.tssi_fault) {
		printf("0.2 Failure: TSSI reports faulty\n");
		return 2;
	}

	printf("Release brake: STAY IN GLV ON\n");
	stateLump.Brake_F_Signal = 0;
	stateLump.Brake_R_Signal = 0;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_GLV_ON) {
		printf("0.2 Failure: ecu state not in GLV ON\n");
		return 2;
	}
	if (stateLump.tssi_fault) {
		printf("0.2 Failure: TSSI reports faulty\n");
		return 2;
	}

	// ##########################
	// ## Step 0.3             ##
	// ##########################
	printf("Press TS ACTIVE: Go to PRECHARGE ENGAGE\n");
	stateLump.ts_active = true;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_PRECHARGE_ENGAGED) {
		printf("0.3 Failure: ecu state not in precharge engaged\n");
		return 3;
	}
	if (stateLump.tssi_fault) {
		printf("0.3 Failure: TSSI reports faulty\n");
		return 3;
	}

	// ##########################
	// ## Step 0.4             ##
	// ##########################
	if (stateLump.ecu_state != ECU_Precharge_Engaged) {
		printf("0.4 Failure: ecu state not in precharge engaged\n");
		return 4;
	}

	// ##########################
	// ## Step 0.5            ##
	// ##########################
	printf("Test Precharge Complete (IR PLUS)\n");
	stateLump.ir_plus = true;
	ECU_Pseudo_State_Tick();
	stateLump.ts_voltage = 400; // dummy value
	if (stateLump.ecu_state != GR_PRECHARGE_COMPLETE) {
		printf("0.5 Failure: ecu state not in precharge complete\n");
		return 5;
	}
	if (stateLump.tssi_fault) {
		printf("0.5 Failure: TSSI reports faulty\n");
		return 5;
	}

	// ##########################
	// ## Step 0.6             ##
	// ##########################
	printf("Press RTD -> STAY IN PRECHARGE COMPLETE\n");
	stateLump.rtd = true;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_PRECHARGE_COMPLETE) {
		printf("0.6 Failure: ecu state not in precharge complete\n");
		return 6;
	}
	if (stateLump.tssi_fault) {
		printf("0.6 Failure: TSSI reports faulty\n");
		return 6;
	}
	printf("Release RTD -> STAY IN PRECHARGE COMPLETE\n");
	stateLump.rtd = false;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_PRECHARGE_COMPLETE) {
		printf("0.6 Failure: ecu state not in precharge complete\n");
		return 6;
	}
	if (stateLump.tssi_fault) {
		printf("0.6 Failure: TSSI reports faulty\n");
		return 6;
	}

	// ##########################
	// ## Step 0.7             ##
	// ##########################
	printf("Press and release the RTD button WHILE pressing the brake\n");
	stateLump.Brake_F_Signal = BRAKE_F_MAX;
	stateLump.Brake_R_Signal = BRAKE_R_MAX;
	printf("Press RTD\n");
	stateLump.rtd = true;
	ECU_Pseudo_State_Tick();
	printf("Release RTD\n");
	stateLump.rtd = false;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_DRIVE_ACTIVE) {
		printf("0.7 Failure: ecu state not in drive active\n");
		return 7;
	}
	if (stateLump.tssi_fault) {
		printf("0.7 Failure: TSSI reports faulty\n");
		return 7;
	}

	// ##########################
	// ## Step 0.8             ##
	// ##########################
	printf("Release Brakes -> STAY IN DRIVE ACTIVE\n");
	stateLump.Brake_F_Signal = 0;
	stateLump.Brake_R_Signal = 0;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_DRIVE_ACTIVE) {
		printf("0.8 Failure: ecu state not in drive active\n");
		return 8;
	}
	if (stateLump.tssi_fault) {
		printf("0.8 Failure: TSSI reports faulty\n");
		return 8;
	}

	// ##########################
	// ## Step 0.9             ##
	// ##########################
	printf("Press Throttle -> STAY IN DRIVE ACTIVE\n");
	stateLump.APPS1_Signal = THROTTLE_MAX_1;
	stateLump.APPS2_Signal = THROTTLE_MAX_2;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_DRIVE_ACTIVE) {
		printf("0.9 Failure: ecu state not in drive active\n");
		return 9;
	}
	if (stateLump.tssi_fault) {
		printf("0.9 Failure: TSSI reports faulty\n");
		return 9;
	}

	// ##########################
	// ## Step 0.10            ##
	// ##########################
	printf("Release Throttle -> STAY IN DRIVE ACTIVE\n");
	stateLump.APPS1_Signal = 0;
	stateLump.APPS2_Signal = 0;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_DRIVE_ACTIVE) {
		printf("0.10 Failure: ecu state not in drive active\n");
		return 10;
	}
	if (stateLump.tssi_fault) {
		printf("0.10 Failure: TSSI reports faulty\n");
		return 10;
	}

	// ##########################
	// ## Step 0.11            ##
	// ##########################
	printf("Press Throttle and Brake -> STAY IN DRIVE ACTIVE\n");
	stateLump.APPS1_Signal = THROTTLE_MAX_1;
	stateLump.APPS2_Signal = THROTTLE_MAX_2;
	stateLump.Brake_F_Signal = BRAKE_F_MAX;
	stateLump.Brake_R_Signal = BRAKE_R_MAX;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_DRIVE_ACTIVE) {
		printf("0.11 Failure: ecu state not in drive active\n");
		return 11;
	}
	if (stateLump.tssi_fault) {
		printf("0.11 Failure: TSSI reports faulty\n");
		return 11;
	}

	// ##########################
	// ## Step 0.12            ##
	// ##########################
	printf("Release Throttle and Brake-> STAY IN DRIVE ACTIVE\n");
	stateLump.APPS1_Signal = 0;
	stateLump.APPS2_Signal = 0;
	stateLump.Brake_F_Signal = 0;
	stateLump.Brake_R_Signal = 0;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_DRIVE_ACTIVE) {
		printf("0.12 Failure: ecu state not in drive active\n");
		return 12;
	}
	if (stateLump.tssi_fault) {
		printf("0.12 Failure: TSSI reports faulty\n");
		return 12;
	}

	// ##########################
	// ## Step 0.13             ##
	// ##########################
	printf("Press Throttle -> STAY IN DRIVE ACTIVE\n");
	stateLump.APPS1_Signal = THROTTLE_MAX_1;
	stateLump.APPS2_Signal = THROTTLE_MAX_2;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_DRIVE_ACTIVE) {
		printf("0.13 Failure: ecu state not in drive active\n");
		return 13;
	}
	if (stateLump.tssi_fault) {
		printf("0.13 Failure: TSSI reports faulty\n");
		return 13;
	}

	// ##########################
	// ## Step 0.14            ##
	// ##########################
	printf("Release Throttle -> STAY IN DRIVE ACTIVE\n");
	stateLump.APPS1_Signal = 0;
	stateLump.APPS2_Signal = 0;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_DRIVE_ACTIVE) {
		printf("0.14 Failure: ecu state not in drive active\n");
		return 14;
	}
	if (stateLump.tssi_fault) {
		printf("0.14 Failure: TSSI reports faulty\n");
		return 14;
	}

	// ##########################
	// ## Step 0.15            ##
	// ##########################
	printf("Press RTD -> MOVE TO PRECHARGE COMPLETE\n");
	stateLump.rtd = true;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_PRECHARGE_COMPLETE) {
		printf("0.15 Failure: ecu state not in precharge complete\n");
		return 15;
	}
	if (stateLump.tssi_fault) {
		printf("0.15 Failure: TSSI reports faulty\n");
		return 15;
	}
	printf("Release RTD -> STAY IN PRECHARGE COMPLETE\n");
	stateLump.rtd = false;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_PRECHARGE_COMPLETE) {
		printf("0.15 Failure: ecu state not in precharge complete\n");
		return 15;
	}
	if (stateLump.tssi_fault) {
		printf("0.15 Failure: TSSI reports faulty\n");
		return 15;
	}

	// ##########################
	// ## Step 0.16             ##
	// ##########################
	printf("Press Throttle -> STAY IN Precharge Complete\n");
	stateLump.APPS1_Signal = THROTTLE_MAX_1;
	stateLump.APPS2_Signal = THROTTLE_MAX_2;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_PRECHARGE_COMPLETE) {
		printf("0.16 Failure: ecu state not in precharge complete\n");
		return 16;
	}
	if (stateLump.tssi_fault) {
		printf("0.16 Failure: TSSI reports faulty\n");
		return 16;
	}

	// ##########################
	// ## Step 0.17            ##
	// ##########################
	printf("Release Throttle -> STAY IN Precharge Complete\n");
	stateLump.APPS1_Signal = 0;
	stateLump.APPS2_Signal = 0;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_PRECHARGE_COMPLETE) {
		printf("0.17 Failure: ecu state not in precharge complete\n");
		return 17;
	}
	if (stateLump.tssi_fault) {
		printf("0.17 Failure: TSSI reports faulty\n");
		return 17;
	}

	// ##########################
	// ## Step 0.18            ##
	// ##########################
	printf("Press TS Active Button -> MOVE to TS DISCHARGE\n");
	stateLump.ts_active = true;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_TS_DISCHARGE) {
		printf("0.18 Failure: ecu state not in ts discharge\n");
		return 18;
	}
	if (stateLump.tssi_fault) {
		printf("0.18 Failure: TSSI reports faulty\n");
		return 18;
	}

	printf("Release TS Active Button -> STAY IN TS DISCHARGE\n");
	stateLump.ts_active = false;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_TS_DISCHARGE) {
		printf("0.18 Failure: ecu state not in ts discharge\n");
		return 18;
	}
	if (stateLump.tssi_fault) {
		printf("0.18 Failure: TSSI reports faulty\n");
		return 18;
	}

	// ##########################
	// ## Step 0.19            ##
	// ##########################
	printf("TS Voltage Less than 60 -> MOVE to GLV ON\n");
	stateLump.ts_voltage = 40;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_GLV_ON) {
		printf("0.19 Failure: ecu state not in GLV ON\n");
		return 19;
	}
	if (stateLump.tssi_fault) {
		printf("0.19 Failure: TSSI reports faulty\n");
		return 19;
	}
	ECU_Pseudo_State_Tick();
	stateLump.ir_plus = true;
	ECU_Pseudo_State_Tick();
	stateLump.ts_voltage = 599;
	if (stateLump.ecu_state != GR_PRECHARGE_COMPLETE) {
		printf("0.19 Failure: ecu state not in Precharge Complete\n");
		return 19;
	}
	if (stateLump.tssi_fault) {
		printf("0.19 Failure: TSSI reports faulty\n");
		return 19;
	}

	stateLump.ecu_state = GR_GLV_ON;
	// ##########################
	// ## Step 1.1            ##
	// ##########################
	printf("TS Voltage Greater than 60 -> MOVE to TS DISCHARGE\n");
	stateLump.ts_voltage = 500;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_GLV_ON) {
		printf("1.1 Failure: ecu state not in GLV ON\n");
		return 20;
	}
	if (stateLump.tssi_fault) {
		printf("1.1 Failure: TSSI reports faulty\n");
		return 20;
	}

	// ##########################
	// ## Step 1.2            ##
	// ##########################
	printf("TS Voltage Less than 60 -> MOVE to GLV ON\n");
	stateLump.ts_voltage = 40;
	ECU_Pseudo_State_Tick();
	if (stateLump.ecu_state != GR_GLV_ON) {
		printf("1.2 Failure: ecu state not in GLV ON\n");
		return 21;
	}
	if (stateLump.tssi_fault) {
		printf("1.2 Failure: TSSI reports faulty\n");
		return 21;
	}
}
