#ifdef STM32L476xx
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_usart.h"
#elif defined(STM32G474xx)
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_usart.h"
#elif defined(STM32U5A9xx)
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_gpio.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_usart.h"
#else
#error "Unsupported STM32 family"
#endif
