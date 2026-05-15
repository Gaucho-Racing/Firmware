#include "StateTicks.h"

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

ECU_StateData stateLump = {.ecu_state = GR_GLV_ON, .acu_software_latch = 1};

static uint32_t millis_since_boot;
void ECU_State_Tick(void)
{
	millis_since_boot = MillisecondsSinceBoot();

	static uint32_t last_ECU_status_msg_millis;
	if (millis_since_boot - last_ECU_status_msg_millis >= ECU_STATUS_MSG_PERIOD_MILLIS) {
		LOGOMATIC("ECU Current State: %d\n", stateLump.ecu_state);
		last_ECU_status_msg_millis = millis_since_boot;
	}

	// light control
	//  false or imd_sense  > 0.5 -> false -> no light
	//  false or imd_sense < 0.5 -> true -> light
	//  true or imd_sense > 0.5 -> true -> light stays on
	//  stateLump.bms_light |= !(stateLump.ams_sense > 0.5f);
	//  stateLump.imd_light |= !(stateLump.imd_sense > 0.5f);
	// lgoht control reset

	// true and no failiure -> flase
	// true and failiure -> true
	// false and failure -> false
	// flase and no failure -> false
	// stateLump.bms_light &= (bmsFailure(&stateLump));
	// stateLump.imd_light &= (imdFailure(&stateLump));

	stateLump.bms_light = (stateLump.ams_sense <= 0.5f) || (stateLump.bms_light && bmsFailure(&stateLump));
	stateLump.imd_light = (stateLump.ams_sense <= 0.5f) || (stateLump.imd_light && imdFailure(&stateLump));

	stateLump.tssi_fault = stateLump.bms_light || stateLump.imd_light;

	// bmsFailure(&stateLump) || imdFailure(&stateLump);

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
	UNUSED(stateData);
	LOGOMATIC("ECU_GLV_Off state reached... this should never happen!\n");
	// TODO ERROR --> GLV_OFF should never be reached
}

void ECU_GLV_On(ECU_StateData *stateData)
{
	if (stateData->ts_voltage >= SAFE_VOLTAGE_LIMIT) {
		LOGOMATIC("Error: TS Voltage >= %d!\n", SAFE_VOLTAGE_LIMIT);
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "TS-Runwy", 8);
		return;
	}

	if (stateData->ts_active_button_pressed /* && stateData->ir_plus*/) { // TODO: Talk to Owen if this is correct for precharge start confirmation
		LOGOMATIC("GLV ON to PRECHARGE START!\n");
		ECU_Transition_To_Precharge_Engaged(stateData);
		stateData->ts_active_button_pressed = false;
		return;
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
	if (stateData->ir_plus) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		LOGOMATIC("PRECHARGE ENGAGED to PRECHARGE COMPLETE!\n");
		return;
	}

	if (CriticalError(stateData) || (millis_since_boot - time_start_precharge) >= MAX_PRECHARGE_TIME) {
		LOGOMATIC("CRITICAL ERROR! PRECHARGE ENGAGED to TS DISCHARGE START!\n");
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "TS-P-ITR", 8);
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		return;
	}
	if (stateData->ts_active_button_pressed) {
		LOGOMATIC("ERROR: ts_active PRESSED! PRECHARGE ENGAGED to TS DISCHARGE START!\n");
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "TS-P-ITR", 8);
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		stateData->ts_active_button_pressed = false;
		return;
	}
}

// TODO: change for CAN button messenging
void ECU_Precharge_Complete(ECU_StateData *stateData)
{
	if (stateData->ts_active_button_pressed) {
		LOGOMATIC("TS Active Toggled Off. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		stateData->ts_active_button_pressed = false;
		return;
	}
	if (CriticalError(stateData)) {
		LOGOMATIC("Error: Critical Error Occurred. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "HV-CritE", 8);
		return;
	}

	if (PressingBrake(stateData) && stateData->rtd_button_pressed) {
		GRCAN_INVERTER_CONFIG_MSG inverter_message = {.max_ac_current = 0xFFFF, .max_dc_current = 0xFFFF, .absolute_max_rpm_limit = 0xFFFF, .motor_direction = 0};
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_GR_Inverter, GRCAN_INVERTER_CONFIG, &inverter_message, sizeof(inverter_message));
		GRCAN_ECU_ANALOG_DATA_MSG pedals_message = {.bspd_signal = stateData->bspd_signal,
							    .bse_signal = stateData->bse_signal,
							    .apps_1_signal = stateData->APPS1_Signal,
							    .apps_2_signal = stateData->APPS2_Signal,
							    .brakeline_f_signal = stateData->Brake_F_Signal,
							    .brakeline_r_signal = stateData->Brake_R_Signal,
							    .steering_angle_signal = stateData->steering_angle_signal,
							    .aux_signal = stateData->aux_signal};
		ECU_CAN_Send(GRCAN_BUS_DATA, GRCAN_TCM, GRCAN_ECU_ANALOG_DATA, &pedals_message, sizeof(pedals_message));
		LOGOMATIC("PRECHARGE COMPLETE to DRIVE START/ACTIVE!\n");
		ECU_Transition_To_Drive_Active(stateData);
		stateData->rtd_button_pressed = false;
		return;
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
		stateData->ts_active_button_pressed = false;
		return;
	}

	if (millis_since_boot - buzzer_start_millis > MAX_BUZZER_TIME_MS) {
		LL_GPIO_ResetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
	} else {
		LL_GPIO_SetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
		// LOGOMATIC("buzz!\n");
	}

	if (stateData->rtd_button_pressed) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		if (vehicle_is_moving(stateData)) {
			LOGOMATIC("Warning: Vehicle is moving during state transition.\n");
		}
		stateData->rtd_button_pressed = false;
		return;
	}

	float torque_request = PressingBrake(stateData) && stateData->vehicle_speed_mph > REGEN_MIN_SPEED_MPH
				   ? -MIN_WITH_TYPES(CalcBrakePercent(stateData) * REGEN_STRENGTH, 1.0f) * MAX_REVERSE_CURRENT_AMPS
				   : CalcAccPedalTravel(stateData) * MAX_CURRENT_AMPS;

	if (APPS_BSE_Violation(stateData)) {
		stateData->apps_bse_violation = true;
	} else if (CalcAccPedalTravel(stateData) < 0.05f) {
		stateData->apps_bse_violation = false;
	}

	static uint32_t last_apps_plausible_frame_millis;
	if (APPS_Plausible(stateData)) {
		last_apps_plausible_frame_millis = millis_since_boot;
	}

	static uint32_t last_bse_plausible_millis;
	if (BSE_Plausible(stateData)) {
		last_bse_plausible_millis = millis_since_boot;
	}

	// Stop throttle if implausible for > 100ms
	if (stateData->apps_bse_violation || millis_since_boot - last_apps_plausible_frame_millis > 100 || millis_since_boot - last_bse_plausible_millis > MAX_BSE_FAILURE_TIME) {
		torque_request = 0;
	}

	static uint32_t last_can_inverter_request_millis;
	if (RATE_LIMIT_100_HZ(millis_since_boot, last_can_inverter_request_millis)) {
		GRCAN_INVERTER_COMMAND_MSG message = {.set_ac_current = torque_request * 100 + 32768, .set_dc_current = torque_request * 100 + 32768, .drive_enable = 1, .rpm_limit = 0};
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_GR_Inverter, GRCAN_INVERTER_COMMAND, &message, sizeof(message));
		ECU_CAN_Send_DTI(DTI_CONTROL_12_CAN_ID, &message.drive_enable, 2);
		ECU_CAN_Send_DTI(DTI_CONTROL_1_CAN_ID, &message.set_ac_current, 2);
		last_can_inverter_request_millis = millis_since_boot;
	}

	// placeholder for pedal data
	// TODO: determine send time (15, 20 ms?)

	static uint32_t last_can_tcm_request_millis;
	if (RATE_LIMIT_100_HZ(millis_since_boot, last_can_tcm_request_millis)) {
		GRCAN_ECU_ANALOG_DATA_MSG message = {.bspd_signal = stateData->bspd_signal,
						     .bse_signal = stateData->bse_signal,
						     .apps_1_signal = stateData->APPS1_Signal,
						     .apps_2_signal = stateData->APPS2_Signal,
						     .brakeline_f_signal = stateData->Brake_F_Signal,
						     .brakeline_r_signal = stateData->Brake_R_Signal,
						     .steering_angle_signal = stateData->steering_angle_signal,
						     .aux_signal = stateData->aux_signal};
		ECU_CAN_Send(GRCAN_BUS_DATA, GRCAN_TCM, GRCAN_ECU_ANALOG_DATA, &message, sizeof(message));
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
	// TODO: Rate limit
	if (millis_since_boot - discharge_start_millis > TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS) {
		LOGOMATIC("Warning: Tractive System fails to discharge in %d seconds.\n", TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS);
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "TS-D-TLE", 8);
	}

	// Discharge the car @ 100 Hz
	static uint32_t last_discharge_request_millis;
	if (RATE_LIMIT_100_HZ(millis_since_boot, last_discharge_request_millis)) {
		GRCAN_ACU_PRECHARGE_MSG message = {.set_ts_active = 0};
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_ACU, GRCAN_ACU_PRECHARGE, &message, sizeof(message));
		last_discharge_request_millis = millis_since_boot;
	}
} // init
