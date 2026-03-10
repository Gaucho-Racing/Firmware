#include <stdint.h>

#include "can.h"
#include "can_tests.h"

int get_cfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t Mode)
{
#ifdef STM32G4
	return defaultSTM32G4_CANCfg(instance, callback, out_cfg, Mode);
#elif defined(STM32L4)
#elif defined(STM32U5)
#error "STM32U5 is untested"#else
#else
#error "Untested STM32 Family"
#endif
}

// TODO: Abstract out the system clock calculation
// Abstracts out everything but the mode and callback
int defaultSTM32G4_CANCfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t Mode)
{
	CANConfig canCfg;
	// canCfg.fdcan_instance = FDCAN2;

	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
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
	canCfg.hal_fdcan_init.StdFiltersNbr = 1;
	canCfg.hal_fdcan_init.ExtFiltersNbr = 0;

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

	if (instance == FDCAN1) {
		canCfg.fdcan_instance = FDCAN1;
		canCfg.rx_gpio = GPIOA;
		canCfg.init_rx_gpio.Pin = GPIO_PIN_11;
		canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

		canCfg.tx_gpio = GPIOA;
		canCfg.init_tx_gpio.Pin = GPIO_PIN_12;
		canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;

		*out_cfg = canCfg;
		return SUCCESS;
	}

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
		return SUCCESS;
	}
#endif

#ifdef USECAN3
#endif

	return ERROR;
}
