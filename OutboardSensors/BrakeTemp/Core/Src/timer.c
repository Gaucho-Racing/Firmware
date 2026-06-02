#include "timer.h"

// Operates in terms of multiples of 0.01ms=10us !!
// period_multiple_of_10us must be between [2, 65535] (inclusive) --> if it's 1 then the timer is ALWAYS active
void WHEEL_SPEED_TIMER_INIT(TIM_HandleTypeDef *handle, uint32_t period_multiple_of_10us)
{

	// 1. Enable Clock for Basic Timer 6
	__HAL_RCC_TIM6_CLK_ENABLE();

	// 2. Fetch the true Timer Clock Frequency (handles the x2 multiplier automatically)
	// For STM32G4, TIM6 is on APB1.
	uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
	uint32_t tim_clk = pclk1;

	// If APB1 prescaler != 1, clock is multiplied by 2
	if ((RCC->CFGR & RCC_CFGR_PPRE1) != 0) {
		tim_clk = pclk1 * 2;
	}

	// Configure Timer Parameters to achieve 100kHz (10us per tick)
	handle->Instance = TIM6;
	handle->Init.Prescaler = (tim_clk / 100000) - 1;
	handle->Init.CounterMode = TIM_COUNTERMODE_UP;
	handle->Init.Period = period_multiple_of_10us - 1; // 0.1ms ticks
	handle->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(handle) != HAL_OK) {
		// Replace with your project's error handler
		while (1)
			;
	}

	// 3. Set One-Pulse Mode directly via the Control Register 1 (CR1)
	// This tells TIM6 to automatically stop itself (clear CEN bit) when the update event fires.
	handle->Instance->CR1 |= TIM_CR1_OPM;

	// Enable update interrupts (when the counter finishes a cycle)
	htim6.Instance->DIER |= TIM_DIER_UIE;

	// 4. Configure NVIC for Interrupts
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
