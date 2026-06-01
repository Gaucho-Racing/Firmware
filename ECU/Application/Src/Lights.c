#include "Lights.h"

#include "CANutils.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateMachine.h"
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

void TSActiveButtonLightControl(ECU_StateData *stateLump)
{
	if (stateLump->ecu_state == GR_GLV_ON || stateLump->ecu_state == GR_GLV_OFF) {
		LL_GPIO_SetOutputPin(TS_ACTIVE_BTN_LED_CONTROL_GPIO_Port, TS_ACTIVE_BTN_LED_CONTROL_Pin);
	} else {
		LL_GPIO_ResetOutputPin(TS_ACTIVE_BTN_LED_CONTROL_GPIO_Port, TS_ACTIVE_BTN_LED_CONTROL_Pin);
	}
}

void RTD_ButtonLightControl(ECU_StateData *stateLump)
{
	// Ignored anyway if not in GLV On
	if (stateLump->ecu_state != GR_GLV_ON) {
		return;
	}

	// Send every 10 ms
	static uint32_t last_rtd_light_update_millis;
	if (MillisecondsSinceBoot() - last_rtd_light_update_millis < 10) {
		return;
	}
	last_rtd_light_update_millis = MillisecondsSinceBoot();

	GRCAN_RTD_LIGHT_CTRL_MSG light_control = {0};

	if (stateLump->torquemap == 0) {
		light_control.red = 0;
		light_control.green = 0;
		light_control.blue = 0;

		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_RTD_LIGHT_CTRL, &light_control, sizeof(light_control));
	} else if (stateLump->torquemap == 1) {
		float multiplier = 1.0f;
		switch (stateLump->powerlevel) {
			case 0:
				multiplier = 0.05f;
				break;
			case 1:
				multiplier = 0.1f;
				break;
			case 2:
				multiplier = 0.2f;
				break;
			case 3:
				multiplier = 0.4f;
				break;
			case 4:
				multiplier = 0.7f;
				break;
			case 5:
				multiplier = 1.0f;
				break;
			default:
				LOGOMATIC("Invalid powerlevel: %d. Defaulting to 0.\n", stateLump->powerlevel);
				multiplier = 0.0f;
		}

		light_control.red = (uint8_t)(255 * multiplier);
		light_control.green = (uint8_t)(63 * multiplier);
		light_control.blue = (uint8_t)(0 * multiplier);

		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_RTD_LIGHT_CTRL, &light_control, sizeof(light_control));
	} else {
		LOGOMATIC("Invalid torquemap: %d. Defaulting to off.\n", stateLump->torquemap);
		light_control.red = 0;
		light_control.green = 0;
		light_control.blue = 0;
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_RTD_LIGHT_CTRL, &light_control, sizeof(light_control));
	}
}

void dashLights(ECU_StateData *stateLump)
{
	/*
	bool bms_nonlatch = stateLump->bms_light;
	bool imd_nonlatch = stateLump->imd_light;
	bool bspd_nonlatch = bspdFailure(stateLump);
	bool bms_latch = !stateLump->bms_light;
	bool imd_latch = !stateLump->imd_light;
	bool bspd_latch = !bspdFailure(stateLump);

	GRCAN_DASH_CONFIG_MSG message = {.led_latch_flags = (bms_nonlatch << 5) | (imd_nonlatch << 4) | (bspd_nonlatch << 3) | (bms_latch << 2) | (imd_latch << 1) | (bspd_latch << 0)};
	*/
	GRCAN_DASH_CONFIG_MSG message = {.led_latch_flags = (!bspdFailure(stateLump) << 5) | (!imdFailure(stateLump) << 4) | (!bmsFailure(stateLump) << 3) | (bspdFailure(stateLump) << 2) |
							    (imdFailure(stateLump) << 1) | (bmsFailure(stateLump) << 0)};

	message.led_latch_flags = ~message.led_latch_flags; // not to spec, needed as of the current iteration of the dash panel code
	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));
}

void lightControl(ECU_StateData *stateData)
{
	BrakeLightControl(stateData);
	TSSILightControl(stateData);
	RTD_ButtonLightControl(stateData);
	dashLights(stateData);
}
