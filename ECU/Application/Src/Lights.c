#include "Lights.h"
#include "StateData.h"
#include "adc.h"
#include "can.h"
#include "stm32g4xx_ll_gpio.h"
#include "main.h"


void brakeLights(ECU_StateData* stateLump) {
    if (stateLump->Brake_F_Signal > 0 || stateLump->Brake_R_Signal > 0) { // TODO: dummy values, fine tune l8r
        LL_GPIO_SetOutputPin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin);
    }
    else {
        LL_GPIO_ResetOutputPin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin);
    }
}

void TSSILight(ECU_StateData* stateLump) {
	// EV.5.11.5: Flash, 2 Hz to 5 Hz, 50% duty cycle
	//     Here we chose a period of 350ms
	if (stateLump->tssi_fault) {
		if (stateLump->millisSinceBoot % 350 < 175) {
			LL_GPIO_SetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
		} else {
			LL_GPIO_ResetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
		}
	} else {
		LL_GPIO_SetOutputPin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin);
		LL_GPIO_ResetOutputPin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin);
	}
}

void RTDButtonLight(ECU_StateData* stateLump) {
    if (stateLump->ecu_state == GR_DRIVE_ACTIVE) {
        LL_GPIO_SetOutputPin(RTD_BTN_LED_CONTROL_GPIO_Port, RTD_BTN_LED_CONTROL_Pin);
    } else {
        LL_GPIO_ResetOutputPin(RTD_BTN_LED_CONTROL_GPIO_Port, RTD_BTN_LED_CONTROL_Pin);
    }
}

void TSActiveButtonLight(ECU_StateData* stateLump) {
    if(stateLump->ecu_state == GR_GLV_ON || stateLump->ecu_state == GR_GLV_OFF) {
        LL_GPIO_SetOutputPin(TS_ACTIVE_BTN_LED_CONTROL_GPIO_Port, TS_ACTIVE_BTN_LED_CONTROL_Pin);
    } else {
        LL_GPIO_ResetOutputPin(TS_ACTIVE_BTN_LED_CONTROL_GPIO_Port, TS_ACTIVE_BTN_LED_CONTROL_Pin);
    }
}

void SoftwareOKLight(ECU_StateData* stateLump) {
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
