#include "StateTicks.h"

#include <stdint.h>

#include "CANutils.h"
#include "ComparatorMacros.h"
#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_DAT.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateMachine.h"
#include "StateUtils.h"
#include "Unused.h"
#include "adc.h"
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

ECU_StateData stateLump = {.ecu_state = GR_GLV_ON, .bcu_software_latch = 1};

CANHandle *primary_can;
CANHandle *data_can;

static uint32_t millis_since_boot;
void ECU_State_Tick(void)
{
	millis_since_boot = MillisecondsSinceBoot();

	static uint32_t last_ECU_status_msg_millis;
	if (millis_since_boot - last_ECU_status_msg_millis >= ECU_STATUS_MSG_PERIOD_MILLIS) {
		LOGOMATIC("ECU Current State: %d\n", stateLump.ecu_state);
		last_ECU_status_msg_millis = millis_since_boot;
	}

	if (bmsFailure(&stateLump) || imdFailure(&stateLump)) {
		stateLump.tssi_fault = true;
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
	UNUSED(stateData);
	LOGOMATIC("ECU_GLV_Off state reached... this should never happen!\n");
	// TODO ERROR --> GLV_OFF should never be reached
}

void ECU_GLV_On(ECU_StateData *stateData)
{
	if (stateData->ts_voltage >= SAFE_VOLTAGE_LIMIT) {
		LOGOMATIC("Error: TS Voltage >= %d!\n", SAFE_VOLTAGE_LIMIT);
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_DEBUGGER, MSG_DEBUG_2_0, "TS-Runwy", 8);
		return;
	}

	if (stateData->ts_active_button_active /* && stateData->ir_plus*/) { // TODO: Talk to Owen if this is correct for precharge start confirmation
		LOGOMATIC("GLV ON to PRECHARGE START!\n");
		ECU_Transition_To_Precharge_Engaged(stateData);
		return;
	}
}

static uint32_t time_start_precharge; // for potential comms errors while precharging

void ECU_Transition_To_Precharge_Engaged(ECU_StateData *stateData)
{
	/*send message to BCU to start precharging*/
	GR_OLD_BCU_PRECHARGE_MSG message = {.precharge = 1}; // Go TS Active/Precharge
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_BCU, MSG_BCU_PRECHARGE, &message, sizeof(message));
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

	if (!stateData->ts_active_button_active || CriticalError(stateData) || (millis_since_boot - time_start_precharge) >= MAX_PRECHARGE_TIME) {
		LOGOMATIC("ERROR or ts_active OFF! PRECHARGE ENGAGED to TS DISCHARGE START!\n");
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_DEBUGGER, MSG_DEBUG_2_0, "TS-P-ITR", 8);
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		return;
	}
}

// TODO: change for CAN button messenging
void ECU_Precharge_Complete(ECU_StateData *stateData)
{
	if (!stateData->ts_active_button_active) {
		LOGOMATIC("TS Active Toggled Off. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		return;
	}
	if (CriticalError(stateData)) {
		LOGOMATIC("Error: Critical Error Occurred. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_DEBUGGER, MSG_DEBUG_2_0, "HV-CritE", 8);
		return;
	}

	if (PressingBrake(stateData) && stateData->rtd_button_active) {
		GR_OLD_INVERTER_CONFIG_MSG message = {.max_ac_current = 0xFFFF, .max_dc_current = 0xFFFF, .abs_max_motor_rpm = 0xFFFF, .motor_direction = 0};
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_GR_INVERTER_1, MSG_INVERTER_CONFIG, &message, sizeof(message));
		LOGOMATIC("PRECHARGE COMPLETE to DRIVE START/ACTIVE!\n");
		ECU_Transition_To_Drive_Active(stateData);
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
	if (!stateData->ts_active_button_active || CriticalError(stateData)) {
		LOGOMATIC("Error: Critical Error Occured. Discharging Tractive System.\n");
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_DEBUGGER, MSG_DEBUG_2_0, "DA-CritE", 8);
		return;
	}

	if (millis_since_boot - buzzer_start_millis > 2000) {
		LL_GPIO_ResetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
	} else {
		LL_GPIO_SetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
		LOGOMATIC("buzz!\n");
	}

	if (!stateData->rtd_button_active) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		if (vehicle_is_moving(stateData)) {
			LOGOMATIC("Warning: Vehicle is moving during state transition.\n");
		}
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

	if (stateData->apps_bse_violation) {
		torque_request = 0;
	}

	static uint32_t last_can_inverter_request_millis;
	if (millis_since_boot - last_can_inverter_request_millis > 10) {
		GR_OLD_INVERTER_COMMAND_MSG message = {.ac_current = torque_request * 100 + 32768, .dc_current = torque_request * 100 + 32768, .drive_enable = 1, .rpm_limit = 0};
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_GR_INVERTER_1, MSG_INVERTER_COMMAND, &message, sizeof(message));
		last_can_inverter_request_millis = millis_since_boot;
	}
}

static uint32_t discharge_start_millis;
void ECU_Transition_To_Tractive_System_Discharge(ECU_StateData *stateData)
{
	stateData->ecu_state = GR_TS_DISCHARGE;
	LOGOMATIC("ECU: BCU discharge Tractive System\n");
	GR_OLD_BCU_PRECHARGE_MSG message = {.precharge = 0};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_BCU, MSG_BCU_PRECHARGE, &message, sizeof(message));
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
	if (millis_since_boot - discharge_start_millis > TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS) {
		LOGOMATIC("Warning: Tractive System fails to discharge in %d seconds.\n", TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS);
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_DEBUGGER, MSG_DEBUG_2_0, "TS-D-TLE", 8);
	}

	// Discharge the car @ 100 Hz
	static uint32_t last_discharge_request_millis;
	if (millis_since_boot - last_discharge_request_millis > 10) {
		GR_OLD_BCU_PRECHARGE_MSG message = {.precharge = 0};
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_BCU, MSG_BCU_PRECHARGE, &message, sizeof(message));
		last_discharge_request_millis = millis_since_boot;
	}
}
