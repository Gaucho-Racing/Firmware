#include "loop.h"

#include "Logomatic.h"
#include "main.h"

void MainLoop(void)
{
	LL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);

	if (LL_GPIO_IsInputPinSet(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin)) {
		LL_GPIO_SetOutputPin(USER_LED_GPIO_Port, USER_LED_Pin);
	} else {
		LL_GPIO_ResetOutputPin(USER_LED_GPIO_Port, USER_LED_Pin);
	}

	LOGOMATIC("Doing things!\n");
}
