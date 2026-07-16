[#ftl]
[#--
  Generates can_cfg.h from the active STM32CubeMX project.

  Detects enabled FDCAN instances and emits the corresponding USECANx macros.
  Software TX queue sizes and CAN timer settings remain user-configurable and
  are preserved across normal CubeMX regeneration.

  CubeMX remains responsible for configuring and generating MX_FDCANx_Init()
  and MX_TIMx_Init(). This file only selects the CubeMX-generated timer handle
  used to service the CAN software TX queues.
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

    [#if ipName?contains("FDCAN1")][#assign hasFDCAN1 = true][/#if]
    [#if ipName?contains("FDCAN2")][#assign hasFDCAN2 = true][/#if]
    [#if ipName?contains("FDCAN3")][#assign hasFDCAN3 = true][/#if]

    [#if ipName?contains("TIM1")][#assign hasTIM1 = true][/#if]
    [#if ipName?contains("TIM2")][#assign hasTIM2 = true][/#if]
    [#if ipName?contains("TIM3")][#assign hasTIM3 = true][/#if]
    [#if ipName?contains("TIM4")][#assign hasTIM4 = true][/#if]
    [#if ipName?contains("TIM5")][#assign hasTIM5 = true][/#if]
    [#if ipName?contains("TIM6")][#assign hasTIM6 = true][/#if]
    [#if ipName?contains("TIM7")][#assign hasTIM7 = true][/#if]
    [#if ipName?contains("TIM8")][#assign hasTIM8 = true][/#if]
    [#if ipName?contains("TIM15")][#assign hasTIM15 = true][/#if]
    [#if ipName?contains("TIM16")][#assign hasTIM16 = true][/#if]
    [#if ipName?contains("TIM17")][#assign hasTIM17 = true][/#if]
[/#list]

#ifndef CAN_CFG_H
#define CAN_CFG_H

/* Generated automatically by STM32CubeMX FreeMarker. */

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

/* Selects htim5 when set to 5. */
#ifndef CAN_TIMER_INDEX
#define CAN_TIMER_INDEX 5
#endif

/* Dequeue one CAN message every X milliseconds. */
#ifndef CAN_DEQUEUE_PERIOD_MS
#define CAN_DEQUEUE_PERIOD_MS 2U
#endif

/* USER CODE END CAN_USER_CONFIG */

[#if hasFDCAN1]
#define USECAN1
[/#if]
[#if hasFDCAN2]
#define USECAN2
[/#if]
[#if hasFDCAN3]
#define USECAN3
[/#if]

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

#if defined(USECAN1) && (TX_BUFFER_1_SIZE == 0U)
#error "TX_BUFFER_1_SIZE must be greater than zero"
#endif
#if defined(USECAN2) && (TX_BUFFER_2_SIZE == 0U)
#error "TX_BUFFER_2_SIZE must be greater than zero"
#endif
#if defined(USECAN3) && (TX_BUFFER_3_SIZE == 0U)
#error "TX_BUFFER_3_SIZE must be greater than zero"
#endif
#if CAN_DEQUEUE_PERIOD_MS == 0U
#error "CAN_DEQUEUE_PERIOD_MS must be greater than zero"
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
#elif (CAN_TIMER_INDEX != 1) && (CAN_TIMER_INDEX != 2) && \
      (CAN_TIMER_INDEX != 3) && (CAN_TIMER_INDEX != 4) && \
      (CAN_TIMER_INDEX != 5) && (CAN_TIMER_INDEX != 6) && \
      (CAN_TIMER_INDEX != 7) && (CAN_TIMER_INDEX != 8) && \
      (CAN_TIMER_INDEX != 15) && (CAN_TIMER_INDEX != 16) && \
      (CAN_TIMER_INDEX != 17)
#error "CAN_TIMER_INDEX selects an unsupported timer"
#endif

#define CAN_CFG_JOIN_INNER(a, b) a##b
#define CAN_CFG_JOIN(a, b) CAN_CFG_JOIN_INNER(a, b)
#define CAN_TIMER_HANDLE CAN_CFG_JOIN(htim, CAN_TIMER_INDEX)

#endif /* CAN_CFG_H */

[/#list]
