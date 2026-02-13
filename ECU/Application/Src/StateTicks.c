#include "StateTicks.h"

#include <stdint.h>

#include "CANutils.h"
#include "ConvenienceMacros.h"
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
static uint32_t buzzer_start_millis;
static uint32_t last_can_inverter_request_millis;

CANHandle *primary_can;
CANHandle *data_can;

#define ECU_STATUS_MSG_PERIOD_MILLIS (1000)
// EV.5.6.3: The Discharge Circuit must be designed to handle the maximum Tractive System voltage for minimum 15 seconds
#define TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS (15000)

void ECU_State_Tick(void)
{
	stateLump.millisSinceBoot = MillisecondsSinceBoot();

	if (stateLump.millisSinceBoot - stateLump.lastECUStatusMsgMillis >= ECU_STATUS_MSG_PERIOD_MILLIS) {
		LOGOMATIC("ECU Current State: %d\n", stateLump.ecu_state);
		stateLump.lastECUStatusMsgMillis = stateLump.millisSinceBoot;
	}

	if (bmsFailure(&stateLump) || imdFailure(&stateLump)) {
		stateLump.tssi_fault = true;
	}

	// EV.5.11.5: Flash, 2 Hz to 5 Hz, 50% duty cycle
	//     Here we chose a period of 350ms
	if (stateLump.tssi_fault) {
		if (stateLump.millisSinceBoot % 350 < 175) {
			LL_GPIO_SetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
		} else {
			LL_GPIO_ResetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
		}
	} else {
		LL_GPIO_SetOutputPin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin);
		LL_GPIO_ResetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
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
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		LOGOMATIC("Error: TS Voltage >= %d!\n", SAFE_VOLTAGE_LIMIT);
		return;
	}

	if (stateData->ts_active_button_active /* && stateData->ir_plus*/) { // TODO: Talk to Owen if this is correct for precharge start confirmation
		ECU_Transition_To_Precharge_Engaged(stateData);
		LOGOMATIC("GLV ON to PRECHARGE START!\n");
		return;
	}
}

void ECU_Transition_To_Precharge_Engaged(ECU_StateData *stateData)
{
	/*send message to BCU to start precharging*/
	GR_OLD_BCU_PRECHARGE_MSG message = {.precharge = 1}; // Go TS Active/Precharge
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_BCU, MSG_BCU_PRECHARGE, &message, sizeof(message));
	stateData->ecu_state = GR_PRECHARGE_ENGAGED;
	LOGOMATIC("PRECHARGE START to PRECHARGE ENGAGED!\n");
	return;
}

void ECU_Precharge_Engaged(ECU_StateData *stateData)
{
	if (stateData->ir_plus) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		LOGOMATIC("PRECHARGE ENGAGED to PRECHARGE COMPLETE!\n");
		return;
	}

	if (!stateData->ts_active_button_active || CommunicationError(stateData)) {
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		LOGOMATIC("ERROR or ts_active OFF! PRECHARGE ENGAGED to TS DISCHARGE START!\n");
		return;
	}
}

// TODO: change for CAN button messenging
void ECU_Precharge_Complete(ECU_StateData *stateData)
{
	if (!stateData->ts_active_button_active) {
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
		LOGOMATIC("TS Active Toggled Off. Discharging Tractive System.\n");
		return;
	}
	if (CriticalError(stateData)) {
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		LOGOMATIC("Error: Critical Error Occurred. Discharging Tractive System.\n");
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);

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

void ECU_Transition_To_Drive_Active(ECU_StateData *stateData)
{
	buzzer_start_millis = stateData->millisSinceBoot;
	last_can_inverter_request_millis = stateData->millisSinceBoot;
	stateData->ecu_state = GR_DRIVE_ACTIVE;
}

void ECU_Drive_Active(ECU_StateData *stateData)
{
	if (!stateData->ts_active_button_active || CriticalError(stateData)) {
		ECU_Transition_To_Tractive_System_Discharge(stateData);
		LOGOMATIC("Error: Critical Error Occured. Discharging Tractive System.\n");
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
		return;
	}

	if (stateData->millisSinceBoot - buzzer_start_millis > 2000) {
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

	if (stateData->millisSinceBoot - last_can_inverter_request_millis > 10) {
		GR_OLD_INVERTER_COMMAND_MSG message = {.ac_current = torque_request * 100 + 32768, .dc_current = torque_request * 100 + 32768, .drive_enable = 1, .rpm_limit = 0};
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_GR_INVERTER_1, MSG_INVERTER_COMMAND, &message, sizeof(message));
		last_can_inverter_request_millis = stateData->millisSinceBoot;
	}
}

void ECU_Transition_To_Tractive_System_Discharge(ECU_StateData *stateData)
{
	stateData->ecu_state = GR_TS_DISCHARGE;
	LOGOMATIC("ECU: BCU discharge Tractive System\n");
	GR_OLD_BCU_PRECHARGE_MSG message = {.precharge = 0};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_BCU, MSG_BCU_PRECHARGE, &message, sizeof(message));
	stateData->dischargeStartMillis = stateData->millisSinceBoot;
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
	if (stateData->millisSinceBoot - stateData->dischargeStartMillis > TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS) {
		LOGOMATIC("Warning: Tractive System fails to discharge in time.\n");
	}
}
