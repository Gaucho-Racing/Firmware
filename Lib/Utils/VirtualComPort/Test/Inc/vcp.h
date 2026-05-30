#include <stdbool.h>
#include <stdint.h>

#ifndef VCP_H
#define VCP_H

typedef enum {
	VCP_CLOCK_PCLK,
	VCP_CLOCK_SYSCLK,
	VCP_CLOCK_HSI,
	VCP_CLOCK_LSE,
} VCP_ClockSource;

typedef enum {
	VCP_Threshold_1_8,
	VCP_Threshold_1_4,
	VCP_Threshold_1_2,
	VCP_Threshold_3_4,
	VCP_Threshold_7_8,
	VCP_Threshold_8_8,
} VCP_FifoThreshold;

typedef enum {
	VCP_Prescalar_Div1,
	VCP_Prescalar_Div2,
	VCP_Prescalar_Div4,
	VCP_Prescalar_Div6,
	VCP_Prescalar_Div8,
	VCP_Prescalar_Div10,
	VCP_Prescalar_Div12,
	VCP_Prescalar_Div16,
	VCP_Prescalar_Div32,
	VCP_Prescalar_Div64,
	VCP_Prescalar_Div128,
	VCP_Prescalar_Div256,
} VCP_Prescalar;

typedef enum {
	VCP_StopBits_0_5,
	VCP_StopBits_1,
	VCP_StopBits_1_5,
	VCP_StopBits_2,
} VCP_StopBits;

typedef enum {
	VCP_Parity_None,
	VCP_Parity_Even,
	VCP_Parity_Odd,
} VCP_Parity;

typedef enum {
	VCP_Oversampling_16,
	VCP_Oversampling_8,
} VCP_Oversampling;

typedef enum {
	VCP_Port_A,
	VCP_Port_B,
	VCP_Port_C,
	VCP_Port_D,
	VCP_Port_E,
	VCP_Port_F,
	VCP_Port_G,
} VCP_GPIO_Port;

typedef struct {
	VCP_ClockSource clock_source;
	VCP_GPIO_Port bus_port;
	uint32_t gpio_tx_rx_pin_mask;
	uint32_t baud_rate;
	VCP_Prescalar prescaler;
	VCP_Oversampling oversampling;
	VCP_StopBits stop_bits;
	VCP_Parity parity;
	VCP_FifoThreshold tx_fifo_threshold;
	VCP_FifoThreshold rx_fifo_threshold;
	uint32_t alternate_function;
	void (*rx_callback)(const char data);
} VCP_Config;

void VCP_Send(const char *data, uint32_t length);
void Setup_VCP(VCP_Config *input_config);

#endif
