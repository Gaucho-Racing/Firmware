#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_gpio_ex.h"
#include "stm32g4xx_hal_fdcan.h"
#include "stm32g4xx_hal_rcc.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g474xx.h"



#ifdef STM32L476xx

#elif defined(STM32G474xx)

#elif defined(STM32U5A9xx)
#else
#error "Unsupported STM32 family"
#endif

