#include "StateTicks.h"

#include <math.h>
#include <stdint.h>

#include "CANutils.h"
#include "ComparatorMacros.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateMachine.h"
#include "StateUtils.h"
#include "Unused.h"
#include "can.h"
#include "stm32g4xx_ll_gpio.h"

/**
 * @brief The ECU state data lump.
 *
 * Lumped state data for the ECU state machine passed to each state function on
 * tick.
 *
 * @remark Intentionally not a globally accessible variable
 */

ECU_StateData stateLump = {
    // Start on GLV On
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
    .enable_regen = false,
    .SDC_startup_condition = true};

static uint32_t millis_since_boot;
void ECU_State_Tick(void)
{
	millis_since_boot = MillisecondsSinceBoot();

	static uint32_t last_ECU_status_msg_millis;
	if (millis_since_boot - last_ECU_status_msg_millis >= ECU_STATUS_MSG_PERIOD_MILLIS) {
		LOGOMATIC("ECU Current State: %d\n", stateLump.ecu_state);
		last_ECU_status_msg_millis = millis_since_boot;
	}

	if (stateLump.ts_active_button_press_interrupt) {
		stateLump.ts_active_button_press_interrupt = false;
		stateLump.ts_active_button_pressed = true;
	} else {
		stateLump.ts_active_button_pressed = false;
	}

	if (stateLump.rtd_button_press_interrupt) {
		stateLump.rtd_button_press_interrupt = false;
		stateLump.rtd_button_pressed = true;
	} else {
		stateLump.rtd_button_pressed = false;
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
			LOGOMATIC("ECU Current State Unknown: %d\n", stateLump.ecu_state);
			LOGOMATIC("ECU: Resetting to GLV On\n");
			stateLump.ecu_state = GR_GLV_ON;
			break;
	}
}

void ECU_GLV_Off(ECU_StateData *stateData)
{
	disable_inverter();
	UNUSED(stateData);
	LOGOMATIC("ECU_GLV_Off state reached... this should never happen!\n");
	// TODO ERROR --> GLV_OFF should never be reached
}

void ECU_GLV_On(ECU_StateData *stateData)
{
	disable_inverter();

	if (stateData->ts_voltage >= SAFE_VOLTAGE_LIMIT) {
		LOGOMATIC("Error: TS Voltage >= %d!\n", SAFE_VOLTAGE_LIMIT);
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "TS-Runwy", 8);
		return;
	}

	if (stateData->ts_active_button_pressed /* && stateData->ir_plus*/) { // TODO: Talk to Owen if this is correct for precharge start confirmation
		LOGOMATIC("GLV ON to PRECHARGE START!\n");
		ECU_Transition_To_Precharge_Engaged(stateData);
		return;
	}

	if (stateData->rtd_button_pressed) {
		stateData->powerlevel = (stateData->powerlevel + 1) % 6;
		LOGOMATIC("Power level now at %d\n", stateData->powerlevel);
	}
}

static uint32_t time_start_precharge; // for potential comms errors while precharging

void ECU_Transition_To_Precharge_Engaged(ECU_StateData *stateData)
{
	/*send message to ACU to start precharging*/
	GRCAN_ACU_PRECHARGE_MSG message = {.set_ts_active = 1}; // Go TS Active/Precharge
	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_ACU, GRCAN_ACU_PRECHARGE, &message, sizeof(message));
	stateData->ecu_state = GR_PRECHARGE_ENGAGED;
	LOGOMATIC("PRECHARGE START to PRECHARGE ENGAGED!\n");
	time_start_precharge = millis_since_boot;
	return;
}

void ECU_Precharge_Engaged(ECU_StateData *stateData)
{
	disable_inverter();

	if (stateData->ir_plus) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		LOGOMATIC("PRECHARGE ENGAGED to PRECHARGE COMPLETE!\n");
		return;
	}

	if (CriticalError(stateData) || (millis_since_boot - time_start_precharge) >= stateData->max_precharge_time_ms) {
		LOGOMATIC("CRITICAL ERROR! PRECHARGE ENGAGED to TS DISCHARGE START!\n");
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "TS-P-ITR", 8);
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		return;
	}
	if (stateData->ts_active_button_pressed) {
		LOGOMATIC("ERROR: ts_active PRESSED! PRECHARGE ENGAGED to TS DISCHARGE START!\n");
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "TS-P-ITR", 8);
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		return;
	}
}

// TODO: change for CAN button messenging
void ECU_Precharge_Complete(ECU_StateData *stateData)
{
	disable_inverter();

	if (stateData->ts_active_button_pressed) {
		LOGOMATIC("TS Active Toggled Off. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		return;
	}
	if (CriticalError(stateData)) {
		LOGOMATIC("Error: Critical Error Occurred. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "HV-CritE", 8);
		return;
	}

	if (PressingBrake(stateData) && stateData->rtd_button_pressed && (CalcAccPedalTravel(stateData) < stateData->apps_deadzone)) {
		GRCAN_INV_CONFIG_MSG inverter_message = {.max_ac_current = 0xFFFF, .max_dc_current = 0xFFFF, .absolute_max_rpm_limit = 0xFFFF, .motor_direction = 0};
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_GR_Inv, GRCAN_INV_CONFIG, &inverter_message, sizeof(inverter_message));
		LOGOMATIC("PRECHARGE COMPLETE to DRIVE START/ACTIVE!\n");
		ECU_Transition_To_Drive_Active(stateData);
	}
}

static uint32_t buzzer_start_millis;

void ECU_Transition_To_Drive_Active(ECU_StateData *stateData)
{
	buzzer_start_millis = millis_since_boot;
	stateData->ecu_state = GR_DRIVE_ACTIVE;
}

void ECU_Drive_Active(ECU_StateData *stateData)
{
	if (CriticalError(stateData)) {
		LOGOMATIC("Error: Critical Error Occured. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "DA-CritE", 8);
		return;
	}
	if (stateData->ts_active_button_pressed) {
		LOGOMATIC("Error: TS active button pressed in Drive Active state. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "DA-CritE", 8);
		return;
	}

	if (millis_since_boot - buzzer_start_millis > MAX_BUZZER_TIME_MS) {
		LL_GPIO_ResetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
	} else {
		LL_GPIO_SetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
	}

	if (stateData->rtd_button_pressed) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		if (stateData->vehicle_speed_mph > 0) {
			LOGOMATIC("Warning: Vehicle is moving during state transition.\n");
		}
		return;
	}

	if (APPS_BSE_Violation(stateData)) {
		stateData->apps_bse_violation = true;
	} else if (CalcAccPedalTravel(stateData) < (stateData->apps_deadzone + 0.05f)) {
		stateData->apps_bse_violation = false;
	}

	static uint32_t last_apps_plausible_frame_millis;
	if (APPS_Plausible(stateData)) {
		last_apps_plausible_frame_millis = millis_since_boot;
	}

	float torque_request;
	bool apps_plausible = (millis_since_boot - last_apps_plausible_frame_millis) <= MAX_APPS_IMPLAUSIBLE_TIME_MS;

	if (stateData->apps_bse_violation || !apps_plausible) {
		torque_request = 0;
	} else if (stateData->enable_regen && (PressingBrake(stateData) || CalcAccPedalTravel(stateData) < (stateData->apps_deadzone + 0.05f)) && stateData->vehicle_speed_mph > REGEN_MIN_SPEED_MPH) {
		torque_request = -MAX_REVERSE_CURRENT_AMPS;
	} else {
		uint16_t max_current = 0;
		// Chosen max current for different power level / torque maps
		switch (stateData->powerlevel) {
			case 0:
				max_current = 100;
				break;
			case 1:
				max_current = 200;
				break;
			case 2:
				max_current = 250;
				break;
			case 3:
				max_current = 300;
				break;
			case 4:
				max_current = 325;
				break;
			case 5:
				max_current = 350;
				break;
			default:
				LOGOMATIC("Invalid power level: %d. Defaulting to no current.\n", stateData->powerlevel);
				max_current = 0;
				break;
		}
		torque_request = fminf(CalcAccPedalTravel(stateData) * max_current, MAX_CURRENT_AMPS);
	}

	static uint32_t last_can_inverter_request_millis = 0;
	if (RATE_LIMIT_100_HZ(millis_since_boot, last_can_inverter_request_millis)) {
		GRCAN_INV_CMD_MSG message = {.set_ac_current = torque_request * 100 + 32768, .set_dc_current = torque_request * 100 + 32768, .drive_enable = 1, .rpm_limit = 0};
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_GR_Inv, GRCAN_INV_CMD, &message, sizeof(message));
		ECU_CAN_Send_DTI(DTI_CONTROL_12_CAN_ID, &message.drive_enable, 1);
		message.set_ac_current = torque_request * 10;
		ECU_CAN_Send_DTI(DTI_CONTROL_1_CAN_ID, &message.set_ac_current, 2);
		last_can_inverter_request_millis = millis_since_boot;
	}
}

static uint32_t discharge_start_millis;
void ECU_Transition_To_Tractive_System_Discharge(ECU_StateData *stateData)
{
	stateData->ecu_state = GR_TS_DISCHARGE;
	LOGOMATIC("ECU: ACU discharge Tractive System\n");
	GRCAN_ACU_PRECHARGE_MSG message = {.set_ts_active = 0};
	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_ACU, GRCAN_ACU_PRECHARGE, &message, sizeof(message));
	discharge_start_millis = millis_since_boot;
}

void ECU_Tractive_System_Discharge(ECU_StateData *stateData)
{
	disable_inverter();

	/*
		Discharge the tractive system to below 60(SAFE_VOLTAGE_LIMIT) volts
	*/
	if (stateData->ts_voltage < SAFE_VOLTAGE_LIMIT) {
		stateData->ecu_state = GR_GLV_ON;
		LOGOMATIC("TS DISCHARGE to GLV ON!\n");
		return;
	}
	/*
		If TS fails to discharge over time then stay and emit a warning,
	   see #129
	*/

	// Discharge the car @ 100 Hz
	static uint32_t last_discharge_request_millis;
	if (RATE_LIMIT_100_HZ(millis_since_boot, last_discharge_request_millis)) {
		if (millis_since_boot - discharge_start_millis > TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS) {
			LOGOMATIC("Warning: Tractive System fails to discharge in %d ms.\n", TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS);
			ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "TS-D-TLE", 8);
		}

		GRCAN_ACU_PRECHARGE_MSG message = {.set_ts_active = 0};
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_ACU, GRCAN_ACU_PRECHARGE, &message, sizeof(message));
		last_discharge_request_millis = millis_since_boot;
	}
}
