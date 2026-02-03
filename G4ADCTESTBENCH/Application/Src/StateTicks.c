#include "StateTicks.h"

#include "CANutils.h"
#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_DAT.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateMachine.h"
#include "StateUtils.h"
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

CANHandle *primary_can;
CANHandle *data_can;

ECU_StateData stateLump = {0};

#define ECU_STATUS_MSG_PERIOD_MILLIS (1000)
// EV.5.6.3: The Discharge Circuit must be designed to handle the maximum Tractive System voltage for minimum 15 seconds
#define TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS (15000)

void ECU_State_Tick()
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
		LL_GPIO_ResetOutputPin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin);
		if (stateLump.millisSinceBoot - stateLump.tssi_red_blinking_current_cycle_starting_millis < 175) {
			LL_GPIO_SetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
		} else if (stateLump.millisSinceBoot - stateLump.tssi_red_blinking_current_cycle_starting_millis < 350) {
			LL_GPIO_ResetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
		} else {
			stateLump.tssi_red_blinking_current_cycle_starting_millis = stateLump.millisSinceBoot;
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
	LOGOMATIC("ECU_GLV_Off state reached... this should never happen!");
	setSoftwareLatch(0); // TODO: need???
			     // TODO ERROR --> GLV_OFF should never be reached
}

void ECU_GLV_On(ECU_StateData *stateData)
{
	if (stateData->ts_voltage >= SAFE_VOLTAGE_LIMIT) {
		ECU_Tractive_System_Discharge_Start(stateData);
		LOGOMATIC("Error: TS Voltage >= 60!");
		return;
	}

	if (stateData->ts_active /* && stateData->ir_plus*/) { // TOOD Talk to Owen if this is correct for precharge start confirmation
		ECU_Precharge_Start(stateData);
		return;
	}
}

void ECU_Precharge_Start(ECU_StateData *stateData)
{
	/*send message to BCU to start precharging*/
	FDCANTxMessage msg = {.tx_header = {.Identifier = 0x00A,
					    .IdType = FDCAN_STANDARD_ID,
					    .TxFrameType = FDCAN_DATA_FRAME,
					    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
					    .DataLength = 1,
					    .BitRateSwitch = FDCAN_BRS_OFF,
					    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
					    .MessageMarker = 0}};
	msg.data[0] = 1; // Go TS Active/Precharge
	can_send(primary_can, &msg);
	stateData->ecu_state = GR_PRECHARGE_ENGAGED;
	return;
}

void ECU_Precharge_Engaged(ECU_StateData *stateData)
{
	if (stateData->ir_plus) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		return;
	}

	if (!stateData->ts_active || CommunicationError(stateData)) {
		ECU_Tractive_System_Discharge_Start(stateData);
		return;
	}
}

void ECU_Precharge_Complete(ECU_StateData *stateData)
{
	if (!stateData->ts_active || CriticalError(stateData)) {
		ECU_Tractive_System_Discharge_Start(stateData);
		LOGOMATIC("Error: Critical Error Occurred. Discharging Tractive System.");
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);

		return;
	}

	if (PressingBrake(stateData) && stateData->rtd) {
		GR_OLD_INVERTER_CONFIG_MSG message = {.max_ac_current = 0xFFFF, .max_dc_current = 0xFFFF, .abs_max_motor_rpm = 0xFFFF, .motor_direction = 0};
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_GR_INVERTER_1, MSG_INVERTER_CONFIG, &message, sizeof(message));
		ECU_Drive_Start(stateData);
		return;
	}
}

static uint32_t buzzer_start_millis;

void ECU_Drive_Start(ECU_StateData *stateData)
{
	buzzer_start_millis = stateData->millisSinceBoot;
	stateData->ecu_state = GR_DRIVE_ACTIVE;
}

void ECU_Drive_Active(ECU_StateData *stateData)
{
	if (!stateData->ts_active || CriticalError(stateData)) {
		ECU_Tractive_System_Discharge_Start(stateData);
		LOGOMATIC("Error: Critical Error Occured. Discharging Tractive System.");
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
		return;
	}

	if (stateData->millisSinceBoot - buzzer_start_millis > 2000) {
		LL_GPIO_ResetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
	} else {
		LL_GPIO_SetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
	}

	if (!stateData->rtd) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		if (vehicle_is_moving(stateData)) {
			LOGOMATIC("Warning: Vehicle is moving during state transition.");
		}
		return;
	}

	float torque_request = PressingBrake(stateData) && stateData->vehicle_speed > REGEN_MIN_SPEED ? -MIN(CalcBrakePercent(stateData) * REGEN_STRENGTH, 1.0f) * MAX_REVERSE_CURRENT_AMPS
												      : CalcPedalTravel(stateData) * MAX_CURRENT_AMPS;

	if (APPS_BSE_Violation(stateData)) {
		stateData->apps_bse_violation = true;
	} else if (CalcPedalTravel(stateData) < 0.05f) {
		stateData->apps_bse_violation = false;
	}

	if (stateData->apps_bse_violation) {
		torque_request = 0;
	}

	GR_OLD_INVERTER_COMMAND_MSG message = {.ac_current = torque_request * 100 + 32768, .dc_current = torque_request * 100 + 32768, .drive_enable = 1, .rpm_limit = 0};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_GR_INVERTER_1, MSG_INVERTER_COMMAND, &message, sizeof(message));
}

void ECU_Tractive_System_Discharge_Start(ECU_StateData *stateData)
{
	stateData->ecu_state = GR_TS_DISCHARGE;
	LOGOMATIC("ECU: BCU discharge Tractive System");
	GR_OLD_BCU_PRECHARGE_MSG message = {.precharge = 0};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_BCU, MSG_BCU_PRECHARGE, &message, sizeof(message));
	stateData->dischargeStartMillis = stateData->millisSinceBoot;
}

void ECU_Tractive_System_Discharge(ECU_StateData *stateData)
{
	/*
		Discharge the tractive system to below 60 volts
		If TS voltage < 60 --> stateData->GLV_ON
	*/
	if (stateData->ts_voltage < SAFE_VOLTAGE_LIMIT) {
		stateData->ecu_state = GR_GLV_ON;
		return;
	}
	/*
		If TS fails to discharge over time then stay and emit a warning,
	   see #129
	*/
	if (stateData->millisSinceBoot - stateData->dischargeStartMillis > TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS) {
		LOGOMATIC("Warning: Tractive System fails to discharge in time.");
	}
}
