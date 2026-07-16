[#ftl]

[#--
Generates can_cfg.h from the active STM32CubeMX project.

Detects enabled FDCAN instances and emits the corresponding USECANx macros.
TX buffer sizes remain user-configurable and are preserved across normal
CubeMX regeneration.

STM32 hardware configuration remains owned by CubeMX.
--]

[#list configs as dt]

[#assign usedIPs = (dt.usedIPs)![]]

[#assign hasFDCAN1 = false]
[#assign hasFDCAN2 = false]
[#assign hasFDCAN3 = false]

[#list usedIPs as ip]
    [#assign ipName = ip?string]

    [#if ipName?contains("FDCAN1")]
        [#assign hasFDCAN1 = true]
    [/#if]

    [#if ipName?contains("FDCAN2")]
        [#assign hasFDCAN2 = true]
    [/#if]

    [#if ipName?contains("FDCAN3")]
        [#assign hasFDCAN3 = true]
    [/#if]
[/#list]
#ifndef CAN_CFG_H
#define CAN_CFG_H

/* Generated automatically by STM32CubeMX FreeMarker. */

/* USER CODE BEGIN CAN_USER_CONFIG */

#define TX_BUFFER_1_SIZE 20U
#define TX_BUFFER_2_SIZE 20U
#define TX_BUFFER_3_SIZE 20U

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

#endif /* CAN_CFG_H */

[/#list]
