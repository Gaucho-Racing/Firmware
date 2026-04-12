#include <stdint.h>

#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "can.h"
#include "can_cfg.h"

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
	GRCAN_FRAME_FD_NO_BRS, // every FD CAN uses this right now
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
	uint32_t prescaler;
	uint32_t sjw;
	uint32_t seg1;
	uint32_t seg2;
} GRCAN_BitTimingPhase;

typedef struct {
	GRCAN_BitTimingPhase nominal;
	GRCAN_BitTimingPhase data;
} GRCAN_BitTiming;

typedef struct {
	GPIO_TypeDef *port;
	uint32_t pin;
	uint32_t alternate_function;
} GRCAN_PinConfig;

typedef struct {
	uint32_t id_type;
	uint32_t filter_index;
	uint32_t filter_type;
	uint32_t filter_config;
	uint32_t filter_id1;
	uint32_t filter_id2;
} GRCAN_FilterConfig;

typedef enum {
	GRCAN_Feature_DISABLE = 0,
	GRCAN_Feature_ENABLE = 1
} GRCAN_FeatureState;

typedef struct {
	GRCAN_BUS_ID bus;
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

	GRCAN_FilterConfig *filter_config;
} GRCAN_BusConfig;

uint32_t GRCAN_ToHAL_ClockSource(GRCAN_ClockSource src);
uint32_t GRCAN_ToHAL_ClockDivider(GRCAN_ClockDivider div);
uint32_t GRCAN_ToHAL_FrameFormat(GRCAN_FrameFormat fmt);
uint32_t GRCAN_ToHAL_OperatingMode(GRCAN_OperatingMode mode);
uint32_t GRCAN_ToHAL_FeatureState(GRCAN_FeatureState state);
GRCAN_BusMode GRCAN_BusModeForBus(GRCAN_BUS_ID bus);
CANHandle *GRCAN_GetHandle(GRCAN_BUS_ID bus);
uint32_t GRCAN_to_DLC(uint32_t size);
uint32_t DLC_to_GRCAN(uint32_t dlc);
void GRCAN_SetDefaultBitTiming(GRCAN_BitTiming *timing);
void GRCAN_SetDefaultBusConfig(GRCAN_BusConfig *busCfg, GRCAN_BUS_ID bus);
void GRCAN_SetLocalNodeID(GRCAN_NODE_ID localID);

#endif
