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
	UNUSED(GPIOx);
	UNUSED(GPIO_Pin);

	return 1;
}

uint32_t HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
	UNUSED(GPIOx);
	UNUSED(GPIO_Pin);
	UNUSED(PinState);
	return 1;
}

// Dont where else to place this
void SendPrechargeStatus()
{
	LOGOMATIC("Precharge should send\n");
	return;
}

void SendDebugReport()
{
	LOGOMATIC("Debug should send\n");
	return;
}
