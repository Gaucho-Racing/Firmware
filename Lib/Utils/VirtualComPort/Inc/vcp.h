#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#ifndef VCP_H

#define BAD_OP (0xFF)

/**
 * @brief Clock source options for the USART peripheral used by VCP.
 * These options correspond to the clock sources available for USART2 and USART3.
 */
typedef enum {
	VCP_CLOCK_PCLK,
	VCP_CLOCK_SYSCLK,
	VCP_CLOCK_HSI,
	VCP_CLOCK_LSE,
} VCP_ClockSource;

/**
 * @brief FIFO threshold options for VCP.
 * These options correspond to the FIFO threshold levels available in the USART peripheral.
 */
typedef enum {
	VCP_Threshold_1_8 = LL_USART_FIFOTHRESHOLD_1_8,
	VCP_Threshold_1_4 = LL_USART_FIFOTHRESHOLD_1_4,
	VCP_Threshold_1_2 = LL_USART_FIFOTHRESHOLD_1_2,
	VCP_Threshold_3_4 = LL_USART_FIFOTHRESHOLD_3_4,
	VCP_Threshold_7_8 = LL_USART_FIFOTHRESHOLD_7_8,
	VCP_Threshold_8_8 = LL_USART_FIFOTHRESHOLD_8_8,
} VCP_FifoThreshold;

/**
 * @brief Prescaler options for VCP.
 * These options correspond to the prescaler division factors available in the USART peripheral.
 */
typedef enum {
	VCP_Prescalar_Div1 = LL_USART_PRESCALER_DIV1,
	VCP_Prescalar_Div2 = LL_USART_PRESCALER_DIV2,
	VCP_Prescalar_Div4 = LL_USART_PRESCALER_DIV4,
	VCP_Prescalar_Div6 = LL_USART_PRESCALER_DIV6,
	VCP_Prescalar_Div8 = LL_USART_PRESCALER_DIV8,
	VCP_Prescalar_Div10 = LL_USART_PRESCALER_DIV10,
	VCP_Prescalar_Div12 = LL_USART_PRESCALER_DIV12,
	VCP_Prescalar_Div16 = LL_USART_PRESCALER_DIV16,
	VCP_Prescalar_Div32 = LL_USART_PRESCALER_DIV32,
	VCP_Prescalar_Div64 = LL_USART_PRESCALER_DIV64,
	VCP_Prescalar_Div128 = LL_USART_PRESCALER_DIV128,
	VCP_Prescalar_Div256 = LL_USART_PRESCALER_DIV256,
} VCP_Prescalar;

/**
 * @brief Stop bits options for VCP.
 * These options correspond to the stop bits settings available in the USART peripheral.
 */
typedef enum {
	VCP_StopBits_0_5 = LL_USART_STOPBITS_0_5,
	VCP_StopBits_1 = LL_USART_STOPBITS_1,
	VCP_StopBits_1_5 = LL_USART_STOPBITS_1_5,
	VCP_StopBits_2 = LL_USART_STOPBITS_2,
} VCP_StopBits;

/**
 * @brief Parity options for VCP.
 * These options correspond to the parity settings available in the USART peripheral.
 */
typedef enum {
	VCP_Parity_None = LL_USART_PARITY_NONE,
	VCP_Parity_Even = LL_USART_PARITY_EVEN,
	VCP_Parity_Odd = LL_USART_PARITY_ODD,
} VCP_Parity;

/**
 * @brief Configuration structure for VCP.
 * This structure contains all the necessary parameters to initialize and configure the USART peripheral for virtual COM port functionality.
 */
typedef enum {
	VCP_Oversampling_16 = LL_USART_OVERSAMPLING_16,
	VCP_Oversampling_8 = LL_USART_OVERSAMPLING_8,
} VCP_Oversampling;

/**
 * @brief Configuration structure for VCP.
 * This structure contains all the necessary parameters to initialize and configure the USART peripheral for virtual COM port functionality.
 */
typedef enum {
	VCP_Port_A = LL_AHB2_GRP1_PERIPH_GPIOA,
	VCP_Port_B = LL_AHB2_GRP1_PERIPH_GPIOB,
	VCP_Port_C = LL_AHB2_GRP1_PERIPH_GPIOC,
	VCP_Port_D = LL_AHB2_GRP1_PERIPH_GPIOD,
	VCP_Port_E = LL_AHB2_GRP1_PERIPH_GPIOE,
	VCP_Port_F = LL_AHB2_GRP1_PERIPH_GPIOF,
	VCP_Port_G = LL_AHB2_GRP1_PERIPH_GPIOG,
} VCP_GPIO_Port;

/**
 * @brief Configuration structure for VCP.
 * This structure contains all the necessary parameters to initialize and configure the USART peripheral for virtual COM port functionality.
 */
typedef struct {
	/* USART instance to use for VCP */
	USART_TypeDef *usart_instance;
	/* Clock source for the USART peripheral */
	VCP_ClockSource clock_source;
	/* GPIO port for TX/RX pins */
	VCP_GPIO_Port bus_port;
	/* GPIO pin mask for TX/RX pins */
	uint32_t gpio_tx_rx_pin_mask;
	/* Baud rate for USART communication */
	uint32_t baud_rate;
	/* Prescaler for USART clock */
	VCP_Prescalar prescaler;
	/* Oversampling mode for USART */
	VCP_Oversampling oversampling;
	/* Stop bits for USART communication */
	VCP_StopBits stop_bits;
	/* Parity for USART communication */
	VCP_Parity parity;
	/* FIFO threshold for TX */
	VCP_FifoThreshold tx_fifo_threshold;
	/* FIFO threshold for RX */
	VCP_FifoThreshold rx_fifo_threshold;
	/* Alternate function for use on pins */
	uint32_t alternate_function;
} VCP_Config;

void VCP_Send(uint8_t *data, uint32_t length);
bool VCP_IsDataAvailable();
uint8_t VCP_Receive(void);
uint8_t VCP_ReceiveLine(uint8_t *buffer, uint8_t buffer_size);
void Setup_VCP(VCP_Config *input_config);

#endif
