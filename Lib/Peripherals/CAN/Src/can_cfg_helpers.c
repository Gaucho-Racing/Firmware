#include <stdint.h>

#include "can.h"
#include "can_cfg.h"
#include "can_tests.h"
#ifndef CAN_CFG_H
#error "can_cfg_helpers.c: Please define CAN_CFG_H and define at least one USECANx and TX_BUFFER_X_SIZE"
#endif

typedef CAN_STATUS can_cfg_helper(FDCAN_GlobalTypeDef*, CAN_RXCallback, CANConfig*, uint32_t, uint32_t, uint32_t);

static can_cfg_helper defaultSTM32G431x8_CANCfg;
static can_cfg_helper defaultSTM32G474xE_CANCfg;

int get_cfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t Mode, uint32_t numStdFilters, uint32_t numExtFilters)
{
#ifdef STM32G474xE
	return defaultSTM32G474xE_CANCfg(instance, callback, out_cfg, Mode, numStdFilters, numExtFilters);
#elif defined(STM32G431x8)
	return defaultSTM32G431x8_CANCfg(instance, callback, out_cfg, Mode, numStdFilters, numExtFilters);
#elif defined(STM32L4)
#error "STM32L4 is untested"
#elif defined(STM32U5)
#error "STM32U5 is untested"
#else
#error "Untested STM32 Platform"
#endif
}

// TODO: Abstract out the system clock calculation
// Abstracts out everything but the mode and callback
CAN_STATUS defaultSTM32G474xE_CANCfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t Mode, uint32_t numStdFilters, uint32_t numExtFilters)
{
	CANConfig canCfg;
	// canCfg.fdcan_instance = FDCAN2;

	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_BRS;
	canCfg.hal_fdcan_init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	canCfg.hal_fdcan_init.Mode = Mode;
	canCfg.hal_fdcan_init.AutoRetransmission = ENABLE;
	canCfg.hal_fdcan_init.TransmitPause = DISABLE;
	canCfg.hal_fdcan_init.ProtocolException = ENABLE;
	canCfg.hal_fdcan_init.NominalPrescaler = 1;
	canCfg.hal_fdcan_init.NominalSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.NominalTimeSeg1 = 127; // Updated for 170MHz: (1+127+42)*1 = 170 ticks -> 1 Mbps
	canCfg.hal_fdcan_init.NominalTimeSeg2 = 42;
	canCfg.hal_fdcan_init.DataPrescaler = 8;
	canCfg.hal_fdcan_init.DataSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.DataTimeSeg1 = 15; // Updated for 170MHz: (1+15+5)*8 = 168 ticks -> ~5 Mbps
	canCfg.hal_fdcan_init.DataTimeSeg2 = 5;
	canCfg.hal_fdcan_init.StdFiltersNbr = numStdFilters;
	canCfg.hal_fdcan_init.ExtFiltersNbr = numExtFilters;

	canCfg.rx_callback = NULL;	   // PLEASE SET
	canCfg.rx_interrupt_priority = 14; // PLEASE SET
	canCfg.tx_interrupt_priority = 14; // PLEASE SET

	// canCfg.rx_gpio = GPIOB;
	// canCfg.init_rx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_rx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_rx_gpio.Pull = GPIO_PULLUP;
	canCfg.init_rx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	// canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

	// canCfg.tx_gpio = GPIOB;
	// canCfg.init_tx_gpio.Pin = GPIO_PIN_13;
	canCfg.init_tx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_tx_gpio.Pull = GPIO_NOPULL;
	canCfg.init_tx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	// canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;

	// Not testing filters at the moment
	// FDCAN_FilterTypeDef filter;

	// can_add_filter(data_can, &filter);
	/* USER CODE END 2 */

	canCfg.rx_callback = callback;

#ifdef USECAN1
	if (instance == FDCAN1) {
		canCfg.fdcan_instance = FDCAN1;
		canCfg.rx_gpio = GPIOA;
		canCfg.init_rx_gpio.Pin = GPIO_PIN_11;
		canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

		canCfg.tx_gpio = GPIOA;
		canCfg.init_tx_gpio.Pin = GPIO_PIN_12;
		canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;

		*out_cfg = canCfg;
		return CAN_SUCCESS;
	}
#endif

#ifdef USECAN2
	if (instance == FDCAN2) {
		canCfg.fdcan_instance = FDCAN2;
		canCfg.rx_gpio = GPIOB;
		canCfg.init_rx_gpio.Pin = GPIO_PIN_12;
		canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

		canCfg.tx_gpio = GPIOB;
		canCfg.init_tx_gpio.Pin = GPIO_PIN_13;
		canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;

		*out_cfg = canCfg;
		return CAN_SUCCESS;
	}
#endif

	// #ifdef USECAN3
	// #endif

	return CAN_ERROR;
}


CAN_STATUS defaultSTM32G431x8_CANCfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t Mode, uint32_t numStdFilters, uint32_t numExtFilters)
{
	CANConfig canCfg;

	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_BRS;
	canCfg.hal_fdcan_init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	canCfg.hal_fdcan_init.Mode = Mode;
	canCfg.hal_fdcan_init.AutoRetransmission = ENABLE;
	canCfg.hal_fdcan_init.TransmitPause = DISABLE;
	canCfg.hal_fdcan_init.ProtocolException = ENABLE;
	canCfg.hal_fdcan_init.NominalPrescaler = 1;
	canCfg.hal_fdcan_init.NominalSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.NominalTimeSeg1 = 127; // Updated for 170MHz: (1+127+42)*1 = 170 ticks -> 1 Mbps
	canCfg.hal_fdcan_init.NominalTimeSeg2 = 42;
	canCfg.hal_fdcan_init.DataPrescaler = 8;
	canCfg.hal_fdcan_init.DataSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.DataTimeSeg1 = 15; // Updated for 170MHz: (1+15+5)*8 = 168 ticks -> ~5 Mbps
	canCfg.hal_fdcan_init.DataTimeSeg2 = 5;
	canCfg.hal_fdcan_init.StdFiltersNbr = numStdFilters;
	canCfg.hal_fdcan_init.ExtFiltersNbr = numExtFilters;

	canCfg.rx_callback = NULL;	   // PLEASE SET
	canCfg.rx_interrupt_priority = 14; // PLEASE SET
	canCfg.tx_interrupt_priority = 14; // PLEASE SET

	// canCfg.rx_gpio = GPIOB;
	// canCfg.init_rx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_rx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_rx_gpio.Pull = GPIO_PULLUP;
	canCfg.init_rx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	// canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

	// canCfg.tx_gpio = GPIOB;
	// canCfg.init_tx_gpio.Pin = GPIO_PIN_13;
	canCfg.init_tx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_tx_gpio.Pull = GPIO_NOPULL;
	canCfg.init_tx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	// canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;

	// Not testing filters at the moment
	// FDCAN_FilterTypeDef filter;

	// can_add_filter(data_can, &filter);
	/* USER CODE END 2 */

	canCfg.rx_callback = callback;

#ifdef USECAN1
	if (instance == FDCAN1) {
		canCfg.fdcan_instance = FDCAN1;
		canCfg.rx_gpio = GPIOA;
		canCfg.init_rx_gpio.Pin = GPIO_PIN_11;
		canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

		canCfg.tx_gpio = GPIOA;
		canCfg.init_tx_gpio.Pin = GPIO_PIN_12;
		canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;

		*out_cfg = canCfg;
		return CAN_SUCCESS;
	}
#endif

	return CAN_ERROR;
}
