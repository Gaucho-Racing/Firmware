#include "grcan_utils.h"

#include <stdint.h>
#include <string.h>

#include "GRCAN_FancyLayer.h"
#include "Logomatic.h"
#include "main.h"

GRCAN_BusMode GRCAN_BusModeForBus(GRCAN_BUS_ID bus)
{
	switch (bus) {
		case GRCAN_BUS_PRIMARY:
			return GRCAN_MODE_FD;
		case GRCAN_BUS_TESTING:
			return GRCAN_MODE_FD;
		case GRCAN_BUS_DATA:
			return GRCAN_MODE_CLASSIC;
		case GRCAN_BUS_CHARGER:
			return GRCAN_MODE_CLASSIC; // update later
		default:
			LOGOMATIC("GRCAN_BusModeForBus: unknown bus %d\n", bus);
			return GRCAN_MODE_CLASSIC;
	}
}

uint32_t GRCAN_ToHAL_ClockSource(GRCAN_ClockSource src)
{
	switch (src) {
		case GRCAN_CLKSRC_PCLK1:
			return LL_RCC_FDCAN_CLKSOURCE_PCLK1;
		case GRCAN_CLKSRC_PLL:
			return LL_RCC_FDCAN_CLKSOURCE_PLL;
		case GRCAN_CLKSRC_HSE:
			return LL_RCC_FDCAN_CLKSOURCE_HSE;
		default:
			LOGOMATIC("GRCAN_ToHAL_ClockSource: default source %d, defaulting to PCLK1\n", src);
			return LL_RCC_FDCAN_CLKSOURCE_PCLK1;
	}
}

uint32_t GRCAN_ToHAL_ClockDivider(GRCAN_ClockDivider div)
{
	switch (div) {
		case GRCAN_CLK_DIV1:
			return FDCAN_CLOCK_DIV1;
		case GRCAN_CLK_DIV2:
			return FDCAN_CLOCK_DIV2;
		case GRCAN_CLK_DIV4:
			return FDCAN_CLOCK_DIV4;
		case GRCAN_CLK_DIV8:
			return FDCAN_CLOCK_DIV8;
		case GRCAN_CLK_DIV16:
			return FDCAN_CLOCK_DIV16;
		default:
			LOGOMATIC("GRCAN_ToHAL_ClockDivider: default divider %d, defaulting to DIV1\n", div);
			return FDCAN_CLOCK_DIV1;
	}
}

uint32_t GRCAN_ToHAL_FrameFormat(GRCAN_FrameFormat fmt)
{
	switch (fmt) {
		case GRCAN_FRAME_CLASSIC:
			return FDCAN_FRAME_CLASSIC;
		case GRCAN_FRAME_FD_NO_BRS:
			return FDCAN_FRAME_FD_NO_BRS;
		case GRCAN_FRAME_FD_BRS:
			return FDCAN_FRAME_FD_BRS;
		default:
			LOGOMATIC("GRCAN_ToHAL_FrameFormat: default frame format %d, defaulting to FD_NO_BRS\n", fmt);
			return FDCAN_FRAME_FD_NO_BRS;
	}
}

uint32_t GRCAN_ToHAL_OperatingMode(GRCAN_OperatingMode mode)
{
	switch (mode) {
		case GRCAN_OPMODE_NORMAL:
			return FDCAN_MODE_NORMAL;
		case GRCAN_OPMODE_RESTRICTED:
			return FDCAN_MODE_RESTRICTED_OPERATION;
		case GRCAN_OPMODE_BUS_MONITORING:
			return FDCAN_MODE_BUS_MONITORING;
		case GRCAN_OPMODE_INTERNAL_LOOPBACK:
			return FDCAN_MODE_INTERNAL_LOOPBACK;
		case GRCAN_OPMODE_EXTERNAL_LOOPBACK:
			return FDCAN_MODE_EXTERNAL_LOOPBACK;
		default:
			LOGOMATIC("GRCAN_ToHAL_OperatingMode: default mode %d, defaulting to NORMAL\n", mode);
			return FDCAN_MODE_NORMAL;
	}

	// internal loopback for testing
}

void GRCAN_SetDefaultBitTiming(GRCAN_BitTiming *timing)
{
	if (timing == NULL) {
		LOGOMATIC("GRCAN_SetDefaultBitTiming: NULL timing pointer\n");
		return;
	}

	timing->nominal.prescaler = 1;
	timing->nominal.sjw = 16;
	timing->nominal.seg1 = 127;
	timing->nominal.seg2 = 42;

	timing->data.prescaler = 8;
	timing->data.sjw = 16;
	timing->data.seg1 = 15;
	timing->data.seg2 = 5;
}

uint32_t GRCAN_ToHAL_FeatureState(GRCAN_FeatureState state) {
	switch (state) {
		case GRCAN_Feature_DISABLE:
			return 0;
		case GRCAN_Feature_ENABLE:
			return 1;
		default:
			LOGOMATIC("GRCAN_ToHAL_FeatureState: default state %d, defaulting to DISABLE\n", state);
			return 0;
	}
}

void GRCAN_SetDefaultBusConfig(GRCAN_BusConfig *busCfg, GRCAN_BUS_ID bus)
{
	if (busCfg == NULL) {
		LOGOMATIC("GRCAN_SetDefaultBusConfig: NULL busCfg pointer\n");
		return;
	}

	busCfg->fdcan_instance = FDCAN2;

	memset(busCfg, 0, sizeof(*busCfg));

	busCfg->bus = bus;

	busCfg->clock_source = GRCAN_CLKSRC_PCLK1;
	busCfg->clock_divider = GRCAN_CLK_DIV1;
	busCfg->frame_format = GRCAN_FRAME_FD_NO_BRS;
	busCfg->operating_mode = GRCAN_OPMODE_NORMAL;

	busCfg->auto_retransmission = GRCAN_Feature_ENABLE;
	busCfg->transmit_pause = GRCAN_Feature_DISABLE;
	busCfg->protocol_exception = GRCAN_Feature_ENABLE;

	GRCAN_SetDefaultBitTiming(&busCfg->bit_timing);

	busCfg->std_filters_nbr = 1;
	busCfg->ext_filters_nbr = 0;

	busCfg->rx_interrupt_priority = 15;
	busCfg->tx_interrupt_priority = 15;
	busCfg->tx_buffer_length = 5;

	busCfg->filter_config = NULL;
}

uint32_t GRCAN_to_DLC(uint32_t size)
{
	switch (size) {
		case 0:
			return FDCAN_DLC_BYTES_0;
		case 1:
			return FDCAN_DLC_BYTES_1;
		case 2:
			return FDCAN_DLC_BYTES_2;
		case 3:
			return FDCAN_DLC_BYTES_3;
		case 4:
			return FDCAN_DLC_BYTES_4;
		case 5:
			return FDCAN_DLC_BYTES_5;
		case 6:
			return FDCAN_DLC_BYTES_6;
		case 7:
			return FDCAN_DLC_BYTES_7;
		case 8:
			return FDCAN_DLC_BYTES_8;
		case 12:
			return FDCAN_DLC_BYTES_12;
		case 16:
			return FDCAN_DLC_BYTES_16;
		case 20:
			return FDCAN_DLC_BYTES_20;
		case 24:
			return FDCAN_DLC_BYTES_24;
		case 32:
			return FDCAN_DLC_BYTES_32;
		case 48:
			return FDCAN_DLC_BYTES_48;
		case 64:
			return FDCAN_DLC_BYTES_64;

		default:
			LOGOMATIC("Invalid CAN FD size: %lu\n, defaulting to 8 \n", size);
			return FDCAN_DLC_BYTES_8;
	}
}

uint32_t DLC_to_GRCAN(uint32_t dlc)
{
	switch (dlc) {
		case FDCAN_DLC_BYTES_0:
			return 0;
		case FDCAN_DLC_BYTES_1:
			return 1;
		case FDCAN_DLC_BYTES_2:
			return 2;
		case FDCAN_DLC_BYTES_3:
			return 3;
		case FDCAN_DLC_BYTES_4:
			return 4;
		case FDCAN_DLC_BYTES_5:
			return 5;
		case FDCAN_DLC_BYTES_6:
			return 6;
		case FDCAN_DLC_BYTES_7:
			return 7;
		case FDCAN_DLC_BYTES_8:
			return 8;
		case FDCAN_DLC_BYTES_12:
			return 12;
		case FDCAN_DLC_BYTES_16:
			return 16;
		case FDCAN_DLC_BYTES_20:
			return 20;
		case FDCAN_DLC_BYTES_24:
			return 24;
		case FDCAN_DLC_BYTES_32:
			return 32;
		case FDCAN_DLC_BYTES_48:
			return 48;
		case FDCAN_DLC_BYTES_64:
			return 64;
		default:
			LOGOMATIC("Invalid CAN FD DLC code: %lu\n, defaulting to 8 \n", dlc);
			return 8;
	}
}
