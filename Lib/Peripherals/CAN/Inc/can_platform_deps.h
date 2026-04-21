#ifndef CAN_PLATFORM_DEPS_H
#define CAN_PLATFORM_DEPS_H

#if defined(STM32G4)
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_gpio_ex.h"
#include "stm32g4xx_hal_rcc.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_rcc.h"
#elif defined(STM32H5)
#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_fdcan.h"
#include "stm32h5xx_hal_gpio.h"
#include "stm32h5xx_hal_gpio_ex.h"
#include "stm32h5xx_hal_rcc.h"
#include "stm32h5xx_hal_rcc_ex.h"
#include "stm32h5xx_ll_gpio.h"
#include "stm32h5xx_ll_rcc.h"
// #elif defined(STM32L4)
// #elif defined(STM32U5)
#else
#error "Unsupported STM32 family"
#endif

#endif // end header guard
