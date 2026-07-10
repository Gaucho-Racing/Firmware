#include "CubeCAN_Config.h"
#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Private/common.h"
#include "main.h"

void FDCAN1_IT0_IRQHandler(void)
{
#ifdef USECAN1
	HAL_FDCAN_IRQHandler(&hal_fdcan1);
#endif
}

void FDCAN1_IT1_IRQHandler(void)
{
#ifdef USECAN1
	HAL_FDCAN_IRQHandler(&hal_fdcan1);
#endif
}

void FDCAN2_IT0_IRQHandler(void)
{
#ifdef USECAN2
	HAL_FDCAN_IRQHandler(&hal_fdcan2);
#endif
}
void FDCAN2_IT1_IRQHandler(void)
{
#ifdef USECAN2
	HAL_FDCAN_IRQHandler(&hal_fdcan2);
#endif
}

void FDCAN3_IT0_IRQHandler(void)
{
#ifdef USECAN3
	HAL_FDCAN_IRQHandler(&hal_fdcan3);
#endif
}
void FDCAN3_IT1_IRQHandler(void)
{
#ifdef USECAN3
	HAL_FDCAN_IRQHandler(&hal_fdcan3);
#endif
}

void CAN_Timer_Start(void)
{
	static bool initialized = false;

	if (initialized) {
		LOGOMATIC("CAN_Timer_Start: timer is already initialized\n");
		return;
	}

	RCC_ClkInitTypeDef clkconfig = {0};
	uint32_t latency = 0;
	HAL_RCC_GetClockConfig(&clkconfig, &latency);

	uint32_t apb1_div = clkconfig.APB1CLKDivider;
	uint32_t tim_clock = (apb1_div == RCC_HCLK_DIV1) ? HAL_RCC_GetPCLK1Freq() : (2U * HAL_RCC_GetPCLK1Freq());

	// 10 kHz counter clock keeps both PSC/ARR in range for a 1 second period.
	const uint32_t counter_hz = 10000U;
	if (tim_clock < counter_hz) {
		LOGOMATIC("CAN_Timer_Start: APB1 clock is too slow to achieve desired timer frequency\n");
		return;
	}

	uint32_t prescaler = (tim_clock / counter_hz) - 1U;
	if (prescaler > 0xFFFFU) {
		LOGOMATIC("CAN_Timer_Start: failed to initialize timer prescaler\n");
		return;
	}

	uint32_t autoreload = ((CAN_TIMER_SEND_PERIOD_US * counter_hz) / 1000000U) - 1U;
	if (autoreload > 0xFFFFU) {
		LOGOMATIC("CAN_Timer_Start: failed to initialize timer autoreload\n");
		return;
	}

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);

	LL_TIM_InitTypeDef tim_init = {0};
	tim_init.Prescaler = prescaler;
	tim_init.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim_init.Autoreload = autoreload;
	tim_init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;

	if (LL_TIM_Init(TIM5, &tim_init) != SUCCESS) {
		LOGOMATIC("CAN_Timer_Start: failed to initialize timer\n");
		return;
	}

	LL_TIM_SetClockSource(TIM5, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_ClearFlag_UPDATE(TIM5);
	LL_TIM_EnableIT_UPDATE(TIM5);

	HAL_NVIC_SetPriority(TIM5_IRQn, 15U, 0U);
	HAL_NVIC_EnableIRQ(TIM5_IRQn);

	LL_TIM_EnableCounter(TIM5);

	initialized = true;
	LOGOMATIC("CAN_Timer_Start: timer initialized\n");
	return;
}

void TIM5_IRQHandler(void)
{
	if (LL_TIM_IsActiveFlag_UPDATE(TIM5)) {
		LL_TIM_ClearFlag_UPDATE(TIM5);
#ifdef USECAN1
		can_tx_dequeue_helper(can_get_handle(&hal_fdcan1));
#endif
#ifdef USECAN2
		can_tx_dequeue_helper(can_get_handle(&hal_fdcan2));
#endif
#ifdef USECAN3
		can_tx_dequeue_helper(can_get_handle(&hal_fdcan3));
#endif
	}
}
