#include "Lights.h"

#include "CANutils.h"
#include "StateData.h"
#include "StateUtils.h"
#include "adc.h"
#include "bitManipulations.h"
#include "ecu_can.h"
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

static uint32_t last_dash_can_send;
void dashLights(ECU_StateData *stateLump)
{
	//light control for if signal goog
	GRCAN_DASH_CONFIG_MSG message = {.led_bits = bspdFailure(stateLump) << 2 | imdFailure(stateLump) << 1 | bmsFailure(stateLump)};

	//this is needed for the latch open control
	message.led_bits |= (stateLump->bspd_sense >= 0.6 && stateLump->bspd_sense <= 1.35) << 5 // 0.3, 1.2, 1.6
					| ((stateLump->imd_sense >= 0.5 && stateLump->imd_sense <= 1.6) << 4)	// 0.5 to 1.6
					| ((stateLump->ams_sense >= 0.5 && stateLump->ams_sense <= 1.6) << 3);	// 0.5 to 1.6

	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));
}

void lightControl(ECU_StateData *stateData)
{
	BrakeLightControl(stateData);
	TSSILightControl(stateData);
	RTDButtonLightControl(stateData);
	dashLights(stateData);
}
