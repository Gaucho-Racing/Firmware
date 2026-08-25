#ifndef TIMED_CAN_OVERRIDE_H
#define TIMED_CAN_OVERRIDE_H

#ifdef SKETCHY_OVERRIDE
// Janky but the STM32G431 does not support TIM5, and we do not want to touch code that touches the ECU at this stage
#undef TIM5
#undef TIM5_IRQn
#undef TIM5_IRQHandler
#undef LL_APB1_GRP1_PERIPH_TIM5

#define TIM5 TIM2
#define TIM5_IRQn TIM2_IRQn
#define TIM5_IRQHandler TIM2_IRQHandler
#define LL_APB1_GRP1_PERIPH_TIM5 LL_APB1_GRP1_PERIPH_TIM2

#define RCC_APB1ENR1_TIM5EN RCC_APB1ENR1_TIM2EN
// FIXME After this development cycle, replace this janky workaround with a proper manner to configure the timer instance
#endif

#endif
