#include "Lights.h"

#include "CANutils.h"
#include "StateData.h"
#include "StateUtils.h"
#include "adc.h"
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

void SoftwareOKLightControl(ECU_StateData *stateLump)
{
	if (stateLump->ecu_state == GR_GLV_ON) {
		LL_GPIO_SetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
	}
	if (stateLump->ecu_state == GR_PRECHARGE_COMPLETE && (!stateLump->ts_active_button_active || CriticalError(stateLump))) {
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
	}
	if (stateLump->ecu_state == GR_DRIVE_ACTIVE && (!stateLump->ts_active_button_active || CriticalError(stateLump))) {
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
	}
}

// TODO: add BCU, IMS, and BSPD lights

void BMSLights(ECU_StateData *stateLump)
{
	bool light = 0;
	light |= stateLump->max_cell_temp_c > CRITICAL_MAX_CELL_TEMP_C;
	light |= stateLump->ts_voltage > CRITICAL_TS_VOLTAGE;
	light |= bmsFailure(stateLump);
	// TODO: interrupted/missing BMS vals
	GR_OLD_DASH_CONFIG_MSG message = {.led_bits = light};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_DASH_PANEL, MSG_DASH_CONFIG, &message, sizeof(message));
}

void IMDLights(ECU_StateData *stateLump)
{
	uint8_t light = 0;
	// TODO: isolation failure?
	light |= imdFailure(stateLump);
	GR_OLD_DASH_CONFIG_MSG message = {.led_bits = (light << 1)};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_DASH_PANEL, MSG_DASH_CONFIG, &message, sizeof(message));
}

void BSPDLights(ECU_StateData *stateLump)
{
	uint8_t light = 0;
	// TODO: isolation failure?
	light |= bspdFailure(stateLump);
	GR_OLD_DASH_CONFIG_MSG message = {.led_bits = (light << 2)};
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_DASH_PANEL, MSG_DASH_CONFIG, &message, sizeof(message));
}

void lightControl(ECU_StateData *stateData)
{
	BrakeLightControl(stateData);
	TSSILightControl(stateData);
	RTDButtonLightControl(stateData);
	TSActiveButtonLightControl(stateData);
	SoftwareOKLightControl(stateData);
}
