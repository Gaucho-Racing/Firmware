
#define STM32G474xx
#ifdef STM32L476xx

#elif defined(STM32G474xx)
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_gpio_ex.h"
#include "stm32g4xx_hal_fdcan.h"
#include "stm32g4xx_hal_rcc.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_gpio.h"
#elif defined(STM32U5A9xx)
#else
#error "Unsupported STM32 family"
#endif

