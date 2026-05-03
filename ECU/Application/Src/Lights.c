#include "Lights.h"

#include "CANutils.h"
#include "StateData.h"
#include "StateUtils.h"
#include "adc.h"
#include "bitManipulations.h"
#include "can.h"
#include "main.h"
#include "stm32g4xx_ll_gpio.h"

void BrakeLightControl(ECU_StateData *stateLump)
{
	if (PressingBrake(stateLump)) {
		LL_GPIO_SetOutputPin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin);
	} else {
		LL_GPIO_ResetOutputPin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin);
	}
}

void TSSILightControl(ECU_StateData *stateLump)
{
	// EV.5.11.5: Flash, 2 Hz to 5 Hz, 50% duty cycle
	//     Here we chose a period of 350ms
	if (stateLump->tssi_fault) {
		LL_GPIO_ResetOutputPin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin);
		if (MillisecondsSinceBoot() % 286 < 143) {
			LL_GPIO_SetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
		} else {
			LL_GPIO_ResetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
		}
	} else {
		LL_GPIO_SetOutputPin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin);
		LL_GPIO_ResetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
	}
}

void RTDButtonLightControl(ECU_StateData *stateLump)
{
	if (stateLump->ecu_state == GR_DRIVE_ACTIVE) {
		LL_GPIO_SetOutputPin(RTD_BTN_LED_CONTROL_GPIO_Port, RTD_BTN_LED_CONTROL_Pin);
	} else {
		LL_GPIO_ResetOutputPin(RTD_BTN_LED_CONTROL_GPIO_Port, RTD_BTN_LED_CONTROL_Pin);
	}
}

void TSActiveButtonLightControl(ECU_StateData *stateLump)
{
	if (stateLump->ecu_state == GR_GLV_ON || stateLump->ecu_state == GR_GLV_OFF) {
		LL_GPIO_SetOutputPin(TS_ACTIVE_BTN_LED_CONTROL_GPIO_Port, TS_ACTIVE_BTN_LED_CONTROL_Pin);
	} else {
		LL_GPIO_ResetOutputPin(TS_ACTIVE_BTN_LED_CONTROL_GPIO_Port, TS_ACTIVE_BTN_LED_CONTROL_Pin);
	}
}
/*
void BMSLights(ECU_StateData *stateLump)
{
       bool light = 0;
       light |= stateLump->max_cell_temp_c > CRITICAL_MAX_CELL_TEMP_C;
       light |= stateLump->ts_voltage > CRITICAL_TS_VOLTAGE;
       light |= bmsFailure(stateLump);
       // TODO: interrupted/missing BMS vals
       GRCAN_DASH_CONFIG_MSG message = {.led_bits = SetBitInByte(0, 0, light)};
       ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));
}

void IMDLights(ECU_StateData *stateLump)
{
       uint8_t light = 0;
       // TODO: isolation failure?
       light |= imdFailure(stateLump);
       GRCAN_DASH_CONFIG_MSG message = {.led_bits = SetBitInByte(0, 1, light)};
       ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));
}

void BSPDLights(ECU_StateData *stateLump)
{
       uint8_t light = 0;
       // TODO: isolation failure?
       light |= bspdFailure(stateLump);
       GRCAN_DASH_CONFIG_MSG message = {.led_bits = SetBitInByte(0, 2, light)};
       ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));
}

void DashLights(ECU_StateData *stateLump)
{
       uint8_t light = 0;
       light |= imdFailure(stateLump);
       light |= stateLump->max_cell_temp_c > CRITICAL_MAX_CELL_TEMP_C;
       light |= stateLump->ts_voltage > CRITICAL_TS_VOLTAGE;
       light |= bmsFailure(stateLump);
       GRCAN_DASH_CONFIG_MSG message = {.led_bits = SetBitInByte(0, 0, light) || SetBitInByte(0, 1, light)};
       ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));
}
*/

static uint32_t last_dash_can_send;
void dashLights(ECU_StateData *stateLump)
{
	// BMS
	bool light = 0;
	GRCAN_DASH_CONFIG_MSG message;

	light |= stateLump->max_cell_temp_c > CRITICAL_MAX_CELL_TEMP_C;
	light |= stateLump->ts_voltage > CRITICAL_TS_VOLTAGE;
	light |= bmsFailure(stateLump);
	// TODO: interrupted/missing BMS vals
	message.led_bits = SetBitInByte(0, 0, light);
	light = 0;

	// IMD
	// TODO: isolation failure?
	light |= imdFailure(stateLump);
	message.led_bits = SetBitInByte(0, 1, light);
	light = 0;

	// BSPD
	light |= bspdFailure(stateLump);
	message.led_bits = SetBitInByte(0, 2, light);

	// TODO: determine moving millis_since_boot to statedata?
	uint32_t currTime = MillisecondsSinceBoot();
	if (RATE_LIMIT_10_HZ(currTime, last_dash_can_send)) {
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));
		last_dash_can_send = currTime;
	}
}

void lightControl(ECU_StateData *stateData)
{
	BrakeLightControl(stateData);
	TSSILightControl(stateData);
	RTDButtonLightControl(stateData);
	dashLights(stateData);
	// DashLights(stateData);
	// TSActiveButtonLightControl(stateData);
	// BMSLights(stateData);
	// IMDLights(stateData);
	// BSPDLights(stateData);
}
