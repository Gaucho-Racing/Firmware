#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateTicks.h"
#include "gpio.h"
#include "main.h"

void Check_Button(CCU_StateData *state_data)
{
	if (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_13)) {
		if (state_data->Button_Status == 0) {
			state_data->Button_Status = 1;
			LOGOMATIC("On\n");

		} else {
			state_data->Button_Status = 0;
			LOGOMATIC("Off\n");
		}
	}
}
