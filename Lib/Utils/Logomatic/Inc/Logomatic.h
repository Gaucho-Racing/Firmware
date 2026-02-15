#include <stdint.h>
#include <stdio.h>

#include "main.h"

#ifndef _LOGOMATIC_H_
#define _LOGOMATIC_H_

#if defined(ITM) && defined(LL_GPIO_MODE_ALTERNATE)
typedef enum {
	LOGOMATIC_PCLK1 = LL_RCC_LPUART1_CLKSOURCE_PCLK1,
	LOGOMATIC_SYSCLK = LL_RCC_LPUART1_CLKSOURCE_SYSCLK,
	LOGOMATIC_HSI = LL_RCC_LPUART1_CLKSOURCE_HSI,
	LOGOMATIC_LSE = LL_RCC_LPUART1_CLKSOURCE_LSE
} LPUART_Clock_Source;

typedef enum {
	LOGOMATIC_BUS = LL_APB1_GRP2_PERIPH_LPUART1,
} LPUART_Bus;

typedef enum {
	LOGOMATIC_GPIOA = LL_AHB2_GRP1_PERIPH_GPIOA,
	LOGOMATIC_GPIOB = LL_AHB2_GRP1_PERIPH_GPIOB,
	LOGOMATIC_GPIOC = LL_AHB2_GRP1_PERIPH_GPIOC,
	LOGOMATIC_GPIOD = LL_AHB2_GRP1_PERIPH_GPIOD,
	LOGOMATIC_GPIOE = LL_AHB2_GRP1_PERIPH_GPIOE,
	LOGOMATIC_GPIOF = LL_AHB2_GRP1_PERIPH_GPIOF,
	LOGOMATIC_GPIOG = LL_AHB2_GRP1_PERIPH_GPIOG,
} LPUART_GPIO_Port;

typedef enum { LOGOMATIC_DATAWIDTH_7B = LL_LPUART_DATAWIDTH_7B, LOGOMATIC_DATAWIDTH_8B = LL_LPUART_DATAWIDTH_8B, LOGOMATIC_DATAWIDTH_9B = LL_LPUART_DATAWIDTH_9B } LPUART_DataWidth;

typedef enum { LOGOMATIC_STOPBITS_1 = LL_LPUART_STOPBITS_1, LOGOMATIC_STOPBITS_2 = LL_LPUART_STOPBITS_2 } LPUART_StopBits;

typedef enum { LOGOMATIC_PARITY_NONE = LL_LPUART_PARITY_NONE, LOGOMATIC_PARITY_EVEN = LL_LPUART_PARITY_EVEN, LOGOMATIC_PARITY_ODD = LL_LPUART_PARITY_ODD } LPUART_Parity;

typedef enum {
	LOGOMATIC_DIRECTION_NONE = LL_LPUART_DIRECTION_NONE,
	LOGOMATIC_DIRECTION_RX = LL_LPUART_DIRECTION_RX,
	LOGOMATIC_DIRECTION_TX = LL_LPUART_DIRECTION_TX,
	LOGOMATIC_DIRECTION_TX_RX = LL_LPUART_DIRECTION_TX_RX
} LPUART_TransferDirection;

typedef enum {
	LOGOMATIC_HWCONTROL_NONE = LL_LPUART_HWCONTROL_NONE,
	LOGOMATIC_HWCONTROL_RTS = LL_LPUART_HWCONTROL_RTS,
	LOGOMATIC_HWCONTROL_CTS = LL_LPUART_HWCONTROL_CTS,
	LOGOMATIC_HWCONTROL_RTS_CTS = LL_LPUART_HWCONTROL_RTS_CTS
} LPUART_HardwareFlowControl;

typedef enum {
	LOGOMATIC_PRESCALER_DIV1 = LL_LPUART_PRESCALER_DIV1,
	LOGOMATIC_PRESCALER_DIV2 = LL_LPUART_PRESCALER_DIV2,
	LOGOMATIC_PRESCALER_DIV4 = LL_LPUART_PRESCALER_DIV4,
	LOGOMATIC_PRESCALER_DIV6 = LL_LPUART_PRESCALER_DIV6,
	LOGOMATIC_PRESCALER_DIV8 = LL_LPUART_PRESCALER_DIV8,
	LOGOMATIC_PRESCALER_DIV10 = LL_LPUART_PRESCALER_DIV10,
	LOGOMATIC_PRESCALER_DIV12 = LL_LPUART_PRESCALER_DIV12,
	LOGOMATIC_PRESCALER_DIV16 = LL_LPUART_PRESCALER_DIV16,
	LOGOMATIC_PRESCALER_DIV32 = LL_LPUART_PRESCALER_DIV32,
	LOGOMATIC_PRESCALER_DIV64 = LL_LPUART_PRESCALER_DIV64,
	LOGOMATIC_PRESCALER_DIV128 = LL_LPUART_PRESCALER_DIV128,
	LOGOMATIC_PRESCALER_DIV256 = LL_LPUART_PRESCALER_DIV256
} LPUART_Prescaler;

typedef enum {
	LOGOMATIC_FIFOTHRESHOLD_1_8 = LL_LPUART_FIFOTHRESHOLD_1_8,
	LOGOMATIC_FIFOTHRESHOLD_1_4 = LL_LPUART_FIFOTHRESHOLD_1_4,
	LOGOMATIC_FIFOTHRESHOLD_1_2 = LL_LPUART_FIFOTHRESHOLD_1_2,
	LOGOMATIC_FIFOTHRESHOLD_3_4 = LL_LPUART_FIFOTHRESHOLD_3_4,
	LOGOMATIC_FIFOTHRESHOLD_7_8 = LL_LPUART_FIFOTHRESHOLD_7_8,
	LOGOMATIC_FIFOTHRESHOLD_8_8 = LL_LPUART_FIFOTHRESHOLD_8_8
} LPUART_FifoThreshold;

typedef struct {
	LPUART_Clock_Source clock_source;
	LPUART_Bus bus;
	LPUART_GPIO_Port gpio_port;
	uint32_t gpio_pin_rx_tx_mask;
	uint32_t baud_rate;
	LPUART_DataWidth data_width;
	LPUART_StopBits stop_bits;
	LPUART_Parity parity;
	LPUART_TransferDirection transfer_direction;
	LPUART_HardwareFlowControl hardware_flow_control;
	LPUART_Prescaler prescaler;
	LPUART_FifoThreshold tx_fifo_threshold;
	LPUART_FifoThreshold rx_fifo_threshold;
} LogomaticConfig;
#else
typedef uint8_t LogomaticConfig;
#endif

void Setup_Logomatic(LogomaticConfig *config);

#ifdef LOGOMATIC_ENABLED
/**
 * @brief ENABLED - Logs a message to the ITM console.
 * @param ... The format string and arguments to log.
 * @note This function is only enabled when LOGOMATIC_ENABLED is defined. Otherwise, it will be compiled out.
 * @note This function is designed to be used with the ITM console, as a result printing floats is an intensive operation.
 */
#define LOGOMATIC(...)                                                                                                                                                                                 \
	do {                                                                                                                                                                                           \
		printf(__VA_ARGS__);                                                                                                                                                                   \
	} while (0)

#else
/**
 * @brief DISABLED - Logs a message to the ITM console.
 * @param ... The format string and arguments to log.
 * @note This function is only enabled when LOGOMATIC_ENABLED is defined. Otherwise, it will be compiled out
 * @note This function is designed to be used with the ITM console, as a result printing floats is an intensive operation.
 */
#define LOGOMATIC(...)                                                                                                                                                                                 \
	do {                                                                                                                                                                                           \
		if (0) {                                                                                                                                                                               \
			printf(__VA_ARGS__);                                                                                                                                                           \
		}                                                                                                                                                                                      \
	} while (0)
#endif

#endif
