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

static bool SDCStartupCondition = true; // prevent false positive TSSI on startup
// PRECONDITION: IMD assumed to give valid readings before this is run
void TSSILightControl(ECU_StateData *stateLump)
{
	// EV.5.11.5: Flash, 2 Hz to 5 Hz, 50% duty cycle
	//     Here we chose a period of 286ms

	bool redCar;

	// if we are before SDC is reset, don't red car unless there is an active failure
	if (SDCStartupCondition) {
		redCar = false;

		SDC_Level bms = bmsLevel(stateLump);
		SDC_Level imd = imdLevel(stateLump);

		if (bms == SDC_OK && imd == SDC_OK) {
			SDCStartupCondition = false;
		} else if (bms == SDC_ONGOING_FAILURE || imd == SDC_ONGOING_FAILURE) {
			SDCStartupCondition = false;
			redCar = true;
		}
	} else {
		redCar = bmsFailure(stateLump) || imdFailure(stateLump);
	}


	// if red car blink tssi
	if (redCar) {
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

void dashLights(ECU_StateData *stateLump)
{
	uint8_t timeState = (MillisecondsSinceBoot() >> 8) % 16;						      // counter from 0 to 15 that increments every 256 ms:
	bool powerLevelLight = (stateLump->ecu_state == GR_GLV_ON) && (timeState == (stateLump->powerlevel + 1) * 2); // 2 Hz light: # of blinks is power level plus one

	// light control for if signal goog
	GRCAN_DASH_CONFIG_MSG message = {.led_latch_flags = (bspdFailure(stateLump) || powerLevelLight) << 2 | stateLump->imd_light << 1 | stateLump->bms_light};

	// this is needed for the latch open control
	message.led_latch_flags |= ((uint8_t) !(bspdFailure(stateLump) || powerLevelLight) << 5) | ((uint8_t)!stateLump->imd_light << 4) | ((uint8_t)!stateLump->bms_light << 3);

	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));
}

void lightControl(ECU_StateData *stateData)
{
	BrakeLightControl(stateData);
	TSSILightControl(stateData);
	RTDButtonLightControl(stateData);
	dashLights(stateData);
}
