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

#define ECU_STATUS_MSG_PERIOD (100)
#define TRACTIVE_SYSTEM_MAX_DISCHARGE_TIME (10000) // TODO: determine an appropriate wait time

void ECU_State_Tick()
{
	if (stateLump.lastECUStatusMsgTick >= ECU_STATUS_MSG_PERIOD) {
		LOGOMATIC("ECU Current State: %d\n", stateLump.ecu_state);
		stateLump.lastECUStatusMsgTick = 0;
	} else {
		stateLump.lastECUStatusMsgTick++;
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

/*

TODO: implement state functionality when loading INTO the state, not just
transitioning state

*/

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
		setSoftwareLatch(0);
		return;
	}

	if (PressingBrake(stateData) && stateData->rtd) {
		stateData->ecu_state = GR_DRIVE_ACTIVE;
		GR_OLD_INVERTER_CONFIG_MSG message = {.max_ac_current = 0xFFFF, .max_dc_current = 0xFFFF, .abs_max_motor_rpm = 0xFFFF, .motor_direction = 0};
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_GR_INVERTER_1, MSG_INVERTER_CONFIG, &message, sizeof(message));
		return;
	}
}

void ECU_Drive_Active(ECU_StateData *stateData)
{
	// TODO Implement functionality
	/*
		If APPS/BSE Violation --> Don't drive until resolved (no state
		change) If Tractive System (TS) active/Critical Error -->
	   Tractive System Discharge
			--> pressed again
		If RTD (Ready to Drive) --> Precharge Complete
	*/

	// Pseudocode
	/*
		if (!TSActive || criticalError(stateData)) {
			stateData->currentState = GR_TS_DISCHARGE
			emit an error
			break
		}
		if (!RTD) {
			stateData->currentState = GR_PRECHARGE_COMPLETE
			emit a warning if moving
			break
		}
		and then we drive the car
		 - calcPedalTravel func :p
		 - make tuna-ble function
	*/

	if (!stateData->ts_active || CriticalError(stateData)) {
		ECU_Tractive_System_Discharge_Start(stateData);
		LOGOMATIC("Error: Critical Error Occured. Discharging Tractive System.");
		setSoftwareLatch(0);
		return;
	}

	if (!stateData->rtd) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		if (vehicle_is_moving(stateData)) {
			LOGOMATIC("Warning: Vehicle is moving during state transition.");
		}
		return;
	}

	float torque_request = CalcPedalTravel(stateData) * MAX_CURRENT_AMPS;
	// If you are pressing the brake, then you have the negativetorque request calculated
	bool brakePressed = PressingBrake(stateData);
	torque_request -= brakePressed * CalcBrakePercent(stateData) * MAX_REVERSE_CURRENT_AMPS; // This is negative current
	
	GR_OLD_INVERTER_COMMAND_MSG message = {.ac_current = torque_request * 100 + 32768, .dc_current = torque_request * 100 + 32768, .drive_enable = 1, .rpm_limit = 0};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_GR_INVERTER_1, MSG_INVERTER_COMMAND, &message, sizeof(message));
	if (brakePressed) {
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_4);
	}
	else {
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);
	}
}

void ECU_Tractive_System_Discharge_Start(ECU_StateData *stateData)
{
	stateData->ecu_state = GR_TS_DISCHARGE;
	LOGOMATIC("ECU: BCU discharge Tractive System");
	GR_OLD_BCU_PRECHARGE_MSG message = {.precharge = 0};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_BCU, MSG_BCU_PRECHARGE, &message, sizeof(message));
	stateData->dischargeStartMillis = 0;
}

void ECU_Tractive_System_Discharge(ECU_StateData *stateData)
{
	// TODO Implement functionality of state itself
	/*
		Discharge the tractive system to below 60 volts
		If TS voltage < 60 --> stateData->GLV_ON
	*/
	if (stateData->ts_voltage < SAFE_VOLTAGE_LIMIT) {
		stateData->ecu_state = GR_GLV_ON;
		stateData->dischargeStartMillis = 0;
		return;
	}
	/*
		If TS fails to discharge over time then stay and emit a warning,
	   see #129
	*/
	if (stateData->dischargeStartMillis > TRACTIVE_SYSTEM_MAX_DISCHARGE_TIME) {
		LOGOMATIC("Tractive System fails to discharge in time.");
	}

	if (stateData->dischargeStartMillis < INT32_MAX) {
		stateData->dischargeStartMillis++;
	}
}
