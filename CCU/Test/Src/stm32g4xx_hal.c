#include "stm32g4xx_hal.h"

#include "Logomatic.h"
#include "Unused.h"
#include "stm32g4xx_ll_gpio.h"

uint32_t HAL_GetTick(void)
{
	return 0;
}
uint32_t HAL_GetTickFreq(void)
{
	return 1;
}

uint32_t HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	(void *)GPIOx;
	(void *)GPIO_Pin;

	return 1;
}

uint32_t HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
	(void *)GPIOx;
	(void *)GPIO_Pin;
	UNUSED(PinState);
	return 1;
}

// Dont where else to place this
void SendPrechargeStatus(CCU_StateData *state_data)
{
	UNUSED(state_data);
	LOGOMATIC("Precharge should send\n");
	return;
}

void SendDebugReport(char *data)
{
	UNUSED(data);
	LOGOMATIC("Debug should send\n");
	return;
}
