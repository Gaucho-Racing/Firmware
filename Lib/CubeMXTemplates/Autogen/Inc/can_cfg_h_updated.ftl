[#ftl]
[#--
  Generates can_cfg.h from the active STM32CubeMX project.

  Detects enabled FDCAN and TIM peripherals and emits compile-time configuration
  for the Gaucho Racing CAN runtime.

  CubeMX remains responsible for:
    - FDCAN clocks, GPIO, bit timing, HAL handles, and MX_FDCANx_Init()
    - TIM clocks, prescaler, NVIC configuration, HAL handle, and MX_TIMx_Init()

  The CAN runtime uses the selected CubeMX-generated TIM handle to periodically
  service its software TX queues.
--]

[#list configs as dt]

[#assign usedIPs = (dt.usedIPs)![]]

[#assign hasFDCAN1 = false]
[#assign hasFDCAN2 = false]
[#assign hasFDCAN3 = false]

[#assign hasTIM1 = false]
[#assign hasTIM2 = false]
[#assign hasTIM3 = false]
[#assign hasTIM4 = false]
[#assign hasTIM5 = false]
[#assign hasTIM6 = false]
[#assign hasTIM7 = false]
[#assign hasTIM8 = false]
[#assign hasTIM15 = false]
[#assign hasTIM16 = false]
[#assign hasTIM17 = false]

[#list usedIPs as ip]
    [#assign ipName = ip?string]

    [#if ipName == "FDCAN1"]
        [#assign hasFDCAN1 = true]
    [/#if]
    [#if ipName == "FDCAN2"]
        [#assign hasFDCAN2 = true]
    [/#if]
    [#if ipName == "FDCAN3"]
        [#assign hasFDCAN3 = true]
    [/#if]

    [#if ipName == "TIM1"]
        [#assign hasTIM1 = true]
    [/#if]
    [#if ipName == "TIM2"]
        [#assign hasTIM2 = true]
    [/#if]
    [#if ipName == "TIM3"]
        [#assign hasTIM3 = true]
    [/#if]
    [#if ipName == "TIM4"]
        [#assign hasTIM4 = true]
    [/#if]
    [#if ipName == "TIM5"]
        [#assign hasTIM5 = true]
    [/#if]
    [#if ipName == "TIM6"]
        [#assign hasTIM6 = true]
    [/#if]
    [#if ipName == "TIM7"]
        [#assign hasTIM7 = true]
    [/#if]
    [#if ipName == "TIM8"]
        [#assign hasTIM8 = true]
    [/#if]
    [#if ipName == "TIM15"]
        [#assign hasTIM15 = true]
    [/#if]
    [#if ipName == "TIM16"]
        [#assign hasTIM16 = true]
    [/#if]
    [#if ipName == "TIM17"]
        [#assign hasTIM17 = true]
    [/#if]
[/#list]

#ifndef CAN_CFG_H
#define CAN_CFG_H

/* Generated automatically by STM32CubeMX FreeMarker. */

/*
 * User-editable CAN runtime configuration.
 *
 * CAN_TIMER_INDEX:
 *   Selects the CubeMX-generated timer handle. Example: 5 selects htim5.
 *
 * CAN_TIMER_TICK_US:
 *   Duration of one timer counter tick, determined by the CubeMX prescaler.
 *   For a 160 MHz TIM clock and Prescaler = 15999, one tick is 100 us.
 *
 * CAN_DEQUEUE_PERIOD_US:
 *   Time between attempts to dequeue and send one software-queued CAN message.
 */
/* USER CODE BEGIN CAN_USER_CONFIG */

#ifndef TX_BUFFER_1_SIZE
#define TX_BUFFER_1_SIZE 20U
#endif

#ifndef TX_BUFFER_2_SIZE
#define TX_BUFFER_2_SIZE 20U
#endif

#ifndef TX_BUFFER_3_SIZE
#define TX_BUFFER_3_SIZE 20U
#endif

#ifndef CAN_TIMER_INDEX
#define CAN_TIMER_INDEX 5
#endif

#ifndef CAN_TIMER_TICK_US
#define CAN_TIMER_TICK_US 100U
#endif

#ifndef CAN_DEQUEUE_PERIOD_US
#define CAN_DEQUEUE_PERIOD_US 500U
#endif

/* USER CODE END CAN_USER_CONFIG */

/* Enabled FDCAN peripherals. */

[#if hasFDCAN1]
#define USECAN1
[/#if]

[#if hasFDCAN2]
#define USECAN2
[/#if]

[#if hasFDCAN3]
#define USECAN3
[/#if]

/* Timers enabled in CubeMX. */

[#if hasTIM1]
#define CUBEMX_HAS_TIM1
[/#if]
[#if hasTIM2]
#define CUBEMX_HAS_TIM2
[/#if]
[#if hasTIM3]
#define CUBEMX_HAS_TIM3
[/#if]
[#if hasTIM4]
#define CUBEMX_HAS_TIM4
[/#if]
[#if hasTIM5]
#define CUBEMX_HAS_TIM5
[/#if]
[#if hasTIM6]
#define CUBEMX_HAS_TIM6
[/#if]
[#if hasTIM7]
#define CUBEMX_HAS_TIM7
[/#if]
[#if hasTIM8]
#define CUBEMX_HAS_TIM8
[/#if]
[#if hasTIM15]
#define CUBEMX_HAS_TIM15
[/#if]
[#if hasTIM16]
#define CUBEMX_HAS_TIM16
[/#if]
[#if hasTIM17]
#define CUBEMX_HAS_TIM17
[/#if]

/* Configuration validation. */

#if defined(USECAN1) && (TX_BUFFER_1_SIZE == 0U)
#error "TX_BUFFER_1_SIZE must be greater than zero"
#endif

#if defined(USECAN2) && (TX_BUFFER_2_SIZE == 0U)
#error "TX_BUFFER_2_SIZE must be greater than zero"
#endif

#if defined(USECAN3) && (TX_BUFFER_3_SIZE == 0U)
#error "TX_BUFFER_3_SIZE must be greater than zero"
#endif

#if CAN_TIMER_TICK_US == 0U
#error "CAN_TIMER_TICK_US must be greater than zero"
#endif

#if CAN_DEQUEUE_PERIOD_US == 0U
#error "CAN_DEQUEUE_PERIOD_US must be greater than zero"
#endif

#if CAN_DEQUEUE_PERIOD_US < CAN_TIMER_TICK_US
#error "CAN_DEQUEUE_PERIOD_US cannot be shorter than one timer tick"
#endif

#if (CAN_DEQUEUE_PERIOD_US % CAN_TIMER_TICK_US) != 0U
#error "CAN_DEQUEUE_PERIOD_US must be divisible by CAN_TIMER_TICK_US"
#endif

#if ((CAN_DEQUEUE_PERIOD_US / CAN_TIMER_TICK_US) - 1U) > 0xFFFFFFFFU
#error "CAN dequeue period exceeds the supported timer auto-reload range"
#endif

#if (CAN_TIMER_INDEX == 1) && !defined(CUBEMX_HAS_TIM1)
#error "CAN_TIMER_INDEX selects TIM1, but TIM1 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 2) && !defined(CUBEMX_HAS_TIM2)
#error "CAN_TIMER_INDEX selects TIM2, but TIM2 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 3) && !defined(CUBEMX_HAS_TIM3)
#error "CAN_TIMER_INDEX selects TIM3, but TIM3 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 4) && !defined(CUBEMX_HAS_TIM4)
#error "CAN_TIMER_INDEX selects TIM4, but TIM4 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 5) && !defined(CUBEMX_HAS_TIM5)
#error "CAN_TIMER_INDEX selects TIM5, but TIM5 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 6) && !defined(CUBEMX_HAS_TIM6)
#error "CAN_TIMER_INDEX selects TIM6, but TIM6 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 7) && !defined(CUBEMX_HAS_TIM7)
#error "CAN_TIMER_INDEX selects TIM7, but TIM7 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 8) && !defined(CUBEMX_HAS_TIM8)
#error "CAN_TIMER_INDEX selects TIM8, but TIM8 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 15) && !defined(CUBEMX_HAS_TIM15)
#error "CAN_TIMER_INDEX selects TIM15, but TIM15 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 16) && !defined(CUBEMX_HAS_TIM16)
#error "CAN_TIMER_INDEX selects TIM16, but TIM16 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX == 17) && !defined(CUBEMX_HAS_TIM17)
#error "CAN_TIMER_INDEX selects TIM17, but TIM17 is not enabled in CubeMX"
#elif (CAN_TIMER_INDEX != 1)  && \
      (CAN_TIMER_INDEX != 2)  && \
      (CAN_TIMER_INDEX != 3)  && \
      (CAN_TIMER_INDEX != 4)  && \
      (CAN_TIMER_INDEX != 5)  && \
      (CAN_TIMER_INDEX != 6)  && \
      (CAN_TIMER_INDEX != 7)  && \
      (CAN_TIMER_INDEX != 8)  && \
      (CAN_TIMER_INDEX != 15) && \
      (CAN_TIMER_INDEX != 16) && \
      (CAN_TIMER_INDEX != 17)
#error "CAN_TIMER_INDEX selects an unsupported timer"
#endif

#define CAN_TIMER_PERIOD_COUNTS \
    (CAN_DEQUEUE_PERIOD_US / CAN_TIMER_TICK_US)

#define CAN_TIMER_AUTORELOAD \
    (CAN_TIMER_PERIOD_COUNTS - 1U)

/*
 * Convert CAN_TIMER_INDEX into CubeMX's generated HAL timer handle.
 *
 * CAN_TIMER_INDEX = 5
 * CAN_TIMER_HANDLE = htim5
 */
#define CAN_CFG_JOIN_INNER(a, b) a##b
#define CAN_CFG_JOIN(a, b) CAN_CFG_JOIN_INNER(a, b)
#define CAN_TIMER_HANDLE CAN_CFG_JOIN(htim, CAN_TIMER_INDEX)

#endif /* CAN_CFG_H */

[/#list]
