#include <stdint.h>

#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"

#ifndef GRCAN_UTILS_H
#define GRCAN_UTILS_H

typedef enum {
    GRCAN_MODE_CLASSIC,
    GRCAN_MODE_FD
} GRCAN_BusMode;

typedef enum {
    GRCAN_CLK_DIV1,
    GRCAN_CLK_DIV2,
    GRCAN_CLK_DIV4,
    GRCAN_CLK_DIV8,
    GRCAN_CLK_DIV16
} GRCAN_ClockDivider;

typedef enum {
    GRCAN_FRAME_CLASSIC,
    GRCAN_FRAME_FD_NO_BRS, //every FD CAN uses this right now
    GRCAN_FRAME_FD_BRS
} GRCAN_FrameFormat;
typedef enum {
    GRCAN_CLKSRC_PCLK1,
    GRCAN_CLKSRC_PLL,
    GRCAN_CLKSRC_HSE
} GRCAN_ClockSource;
typedef enum {
    GRCAN_OPMODE_NORMAL,
    GRCAN_OPMODE_RESTRICTED,
    GRCAN_OPMODE_BUS_MONITORING,
    GRCAN_OPMODE_INTERNAL_LOOPBACK,
    GRCAN_OPMODE_EXTERNAL_LOOPBACK
} GRCAN_OperatingMode;

typedef struct {
    uint32_t nominal_prescaler;
    uint32_t nominal_sjw;
    uint32_t nominal_seg1;
    uint32_t nominal_seg2;

    uint32_t data_prescaler;
    uint32_t data_sjw;
    uint32_t data_seg1;
    uint32_t data_seg2;
} GRCAN_BitTiming;
typedef struct {
    uint32_t prescaler;
    uint32_t sjw;
    uint32_t seg1;
    uint32_t seg2;
} GRCAN_PhaseTiming;

typedef struct {
    GPIO_TypeDef *port;
    uint32_t pin;
    uint32_t alternate_function;
} GRCAN_PinConfig;

typedef struct {
    GRCAN_PhaseTiming nominal;
    GRCAN_PhaseTiming data;
} GRCAN_BitTiming;

typedef struct {
    uint32_t id_type;
    uint32_t filter_index;
    uint32_t filter_type;
    uint32_t filter_config;
    uint32_t filter_id1;
    uint32_t filter_id2;
} GRCAN_FilterConfig;

typedef struct {
    GR_OLD_BUS_ID bus;
    FDCAN_GlobalTypeDef *fdcan_instance;

    GRCAN_ClockSource clock_source;
    GRCAN_ClockDivider clock_divider;
    GRCAN_FrameFormat frame_format;
    GRCAN_OperatingMode operating_mode;

    GRCAN_FeatureState auto_retransmission;
    GRCAN_FeatureState transmit_pause;
    GRCAN_FeatureState protocol_exception;

    GRCAN_BitTiming bit_timing;

    uint32_t std_filters_nbr;
    uint32_t ext_filters_nbr;

    GRCAN_PinConfig rx_pin;
    GRCAN_PinConfig tx_pin;

    CAN_RXCallback rx_callback;

    uint32_t rx_interrupt_priority;
    uint32_t tx_interrupt_priority;
    uint32_t tx_buffer_length;

    GRCAN_FilterConfigFn filter_config_fn;
} GRCAN_BusConfig;

uint32_t GRCAN_ToHAL_ClockSource(GRCAN_ClockSource src);
uint32_t GRCAN_ToHAL_ClockDivider(GRCAN_ClockDivider div);
uint32_t GRCAN_ToHAL_FrameFormat(GRCAN_FrameFormat fmt);
uint32_t GRCAN_ToHAL_OperatingMode(GRCAN_OperatingMode mode);
uint32_t GRCAN_ToHAL_FeatureState(GRCAN_FeatureState state);

uint32_t GRCAN_to_DLC(uint32_t size);
uint32_t DLC_to_GRCAN(uint32_t dlc);

#endif
