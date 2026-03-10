#include "stm32g4xx_ll_gpio.h"

#include "Unused.h"
uint32_t LL_GPIO_IsInputPinSet(GPIO_TypeDef GPIOx, uint32_t PinMask)
{
	UNUSED(GPIOx);
	UNUSED(PinMask);
	return 1;
}

uint32_t LL_GPIO_ResetOutputPin(GPIO_TypeDef GPIOx, uint32_t PinMask)
{
	UNUSED(GPIOx);
	UNUSED(PinMask);
	return 1;
}
