[#ftl]
[#--
  Example STM32CubeMX FreeMarker template

  Suggested filename:
    can_cfg_h.ftl

  Expected generated filename:
    can_cfg.h

  Purpose:
    Detect which FDCAN peripherals are enabled in CubeMX and generate
    compile-time configuration macros for the Gaucho Racing CAN library.
--]

[#list configs as dt]

[#assign usedIPs = (dt.usedIPs)![]]

[#assign hasFDCAN1 = false]
[#assign hasFDCAN2 = false]
[#assign hasFDCAN3 = false]

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
[/#list]

#ifndef CAN_CFG_H
#define CAN_CFG_H

/* Generated automatically by STM32CubeMX FreeMarker. */

[#if hasFDCAN1]
#define USECAN1
#define TX_BUFFER_1_SIZE 20U
[/#if]

[#if hasFDCAN2]
#define USECAN2
#define TX_BUFFER_2_SIZE 20U
[/#if]

[#if hasFDCAN3]
#define USECAN3
#define TX_BUFFER_3_SIZE 20U
[/#if]

[#if !hasFDCAN1 && !hasFDCAN2 && !hasFDCAN3]
#error "No FDCAN peripheral is enabled in STM32CubeMX"
[/#if]

#endif /* CAN_CFG_H */

[/#list]
