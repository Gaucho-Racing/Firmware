#include "timer.h"

void WHEEL_SPEED_TIMER_INIT(TIM_HandleTypeDef handle, uint32_t period_ms) {
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    // 1. Enable Clock for the Timer
    __HAL_RCC_TIM6_CLK_ENABLE();

    // 2. Configure Timer Parameters
    // Formula: Update_Event = TIM_CLK / ((PSC + 1) * (ARR + 1))
    // To make it easy, we set PSC to (SystemCoreClock / 10000) - 1.
    // This makes the timer frequency 10kHz (1 tick = 0.1ms).

    handle.Instance = TIM6;
    handle.Init.Prescaler = (HAL_RCC_GetPCLK1Freq() / 10000) - 1;
    handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    handle.Init.Period = (period_ms * 10) - 1; // Convert ms to 0.1ms ticks
    handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&handle) != HAL_OK) {
        Error_Handler();
    }

    // 3. Set One-Pulse Mode
    // This is the CRITICAL line that prevents renewal.
    __HAL_TIM_ENABLE_OCxPRELOAD(&handle, TIM_AUTORELOAD_PRELOAD_DISABLE);
    HAL_TIM_OnePulse_Init(&handle, TIM_OPMODE_SINGLE);

    // 4. Configure NVIC for Interrupts
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
