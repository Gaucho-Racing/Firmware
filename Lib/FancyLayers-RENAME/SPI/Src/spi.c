// Wonderful SPI Abstraction Layer courtesy of Bailey
#include "spi.h"

#include <stdlib.h>

// Defines hidden from user space
#define GR_SPI_TRANSFER_SIZE_8 8
#define GR_SPI_TRANSFER_SIZE_16 16
#define GR_SPI_STATUS 0
#define GR_SPI_MSG_IN_PROGRESS -1
#define GR_SPI_MSG_IDLE -1

void GR_SPI_Initialize(GR_SPI_Handler *handle, LL_SPI_InitTypeDef *config,
		       GR_SPI_Pins *pin_config)
{
	// Create Circular Buffers
	CircularBuffer *circular_buffer_ptr;
	circular_buffer_ptr =
	    GR_CircularBuffer_Create(GR_SPI_BUFFER_MESSAGE_CAPACITY);
	if (circular_buffer_ptr == NULL) {
		// Attempt to Create Rx Buffer Error
	} else {
		handle->rx_buffer = circular_buffer_ptr;
	}
	circular_buffer_ptr =
	    GR_CircularBuffer_Create(GR_SPI_BUFFER_MESSAGE_CAPACITY);
	if (circular_buffer_ptr == NULL) {
		// Attempt to Create Tx Buffer Error
	} else {
		handle->tx_buffer = circular_buffer_ptr;
	}

	// Copy over config values
	handle->spi_config = (LL_SPI_InitTypeDef *)malloc(
	    sizeof(LL_SPI_InitTypeDef)); // Make memory for LL_SPI_InitTypeDef
					 // config struct
	*handle->spi_config = *config;

	// Deep copy of pins struct
	handle->pins = (GR_SPI_Pins *)malloc(
	    sizeof(GR_SPI_Pins)); // Make memory for GR_SPI_Pins struct
	handle->pins->pin_nums = (uint32_t *)malloc(
	    pin_config->num_pins *
	    sizeof(uint32_t)); // Make memory for pin_nums[num_pins]
	handle->pins->GPIOx = (GPIO_TypeDef **)malloc(
	    pin_config->num_pins *
	    sizeof(GPIO_TypeDef)); // Make memory for GPIOx[num_pins]
	for (uint32_t i = 0; i < pin_config->num_pins; i++) {
		handle->pins->pin_nums[i] = pin_config->pin_nums[i];
		handle->pins->GPIOx[i] = pin_config->GPIOx[i];
	}
	handle->pins->SPIx = pin_config->SPIx;
	handle->pins->num_pins = pin_config->num_pins;
	handle->pins->alternate_function_number =
	    pin_config->alternate_function_number;

	// Store handler in lookup table for interrupts
	switch ((uint32_t)handle->pins->SPIx) {
		case (uint32_t)SPI1: {
			GR_SPI_HANDLER_LUT[0] = handle;
			break;
		}
		case (uint32_t)SPI2: {
			GR_SPI_HANDLER_LUT[1] = handle;
			break;
		}
		case (uint32_t)SPI3: {
			GR_SPI_HANDLER_LUT[2] = handle;
			break;
		}
	}

	// Enable GPIO and SPI clocks
	GR_SPI_Enable_Clocks(handle);

	// Configure GPIOs
	LL_GPIO_InitTypeDef gpio_pin_config;
	GR_SPI_Configure_Pins(handle, &gpio_pin_config);

	// Configure SPI protocol with config values
	LL_SPI_Init(handle->pins->SPIx, config);
	// Transaction size is 8-bits
	if (config->DataWidth <= 8) {
		handle->transfer_size = GR_SPI_TRANSFER_SIZE_8;
	}
	// Transaction size is 16-bits
	else {
		handle->transfer_size = GR_SPI_TRANSFER_SIZE_16;
	}

	// Enable SPI peripheral
	LL_SPI_Enable(handle->pins->SPIx);

	// Enable interrupts in NVIC
	int SPI_IRQn = GR_SPI_Get_IRQn(handle->pins->SPIx);
	if (SPI_IRQn != GR_SPI_UNKNOWN_IRQN) {
		NVIC_SetPriority(SPI_IRQn, 1);
		NVIC_EnableIRQ(SPI_IRQn);
	} else {
		return; // Throw an error
	}

	// Enable interrupts at peripheral level
	LL_SPI_EnableIT_ERR(handle->pins->SPIx);  // Error interrupt
	LL_SPI_EnableIT_RXNE(handle->pins->SPIx); // Not empty Rx buffer
	LL_SPI_EnableIT_TXE(handle->pins->SPIx);  // Empty Tx buffer
}

void GR_SPI_Interrupt_Handler(GR_SPI_Handler *handle)
{
	// Check if valid handle
	if (handle == NULL) {
		// Throw an error
		return;
	}

	// Check if called by error interrupt
	// Frame format error
	if (LL_SPI_IsActiveFlag_FRE(handle->pins->SPIx)) {
		// Log an error
		return;
	}
	// Overrun error
	else if (LL_SPI_IsActiveFlag_OVR(handle->pins->SPIx)) {
		// Log an error
		return;
	}
	// Fault mode error
	else if (LL_SPI_IsActiveFlag_MODF(handle->pins->SPIx)) {
		// Log an error
		return;
	}
	// CRC error
	else if (LL_SPI_IsActiveFlag_CRCERR(handle->pins->SPIx)) {
		// Log an error
		return;
	}

	// No errors detected...

	// Check if Rx circular buffer is not empty
	if (LL_SPI_IsActiveFlag_RXNE(handle->pins->SPIx)) {
		uint16_t rx_index = handle->current_rx_msg_index,
			 msg_size = handle->current_msg->size;
		// Queue the message into the circular buffer
		if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_16 &&
		    rx_index <= msg_size - 2) {
			uint16_t data =
			    LL_SPI_ReceiveData16(handle->pins->SPIx);
			handle->current_msg->data[rx_index + 1] =
			    (uint8_t)(data & 0xFF);
			handle->current_msg->data[rx_index] =
			    (uint8_t)(data >> 8);
		} else if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_8 &&
			   rx_index <= msg_size - 1) {
			uint8_t data = LL_SPI_ReceiveData8(handle->pins->SPIx);
			handle->current_msg->data[rx_index] = data;
		} else {
			// ERROR: Current message is full
		}

		// Push current message into Rx circular buffer to mark
		// completion
		if (rx_index == msg_size) {
			GR_CircularBuffer_Push(handle->rx_buffer,
					       (void *)handle->current_msg,
					       sizeof(GR_SPI_Message *));
			handle->current_msg = NULL;
			handle->current_rx_msg_index = 0;
			// Finish transaction
			LL_GPIO_SetOutputPin(handle->pins->GPIOx[3],
					     LL_GPIO_PIN_0);
			// Only go to IDLE when no additional messages are in
			// pipeline
			if (!GR_CircularBuffer_Peek(handle->tx_buffer) &&
			    !handle->current_msg) {
				handle->msg_status = GR_SPI_MSG_IDLE;
			} else {
				// Re-initiate a transaction
				handle->msg_status = GR_SPI_MSG_IN_PROGRESS;
				handle->current_tx_msg_index = 0;
				handle->current_rx_msg_index = 0;
				handle->current_msg =
				    GR_CircularBuffer_Pop(handle->tx_buffer);

				// PIN mask needs to be fixed
				LL_GPIO_ResetOutputPin(handle->pins->GPIOx[3],
						       LL_GPIO_PIN_0);

				GR_SPI_Transfer_Tx_Bytes(handle);
			}
		}
	}
	// Check if Tx is empty
	if (LL_SPI_IsActiveFlag_TXE(handle->pins->SPIx)) {
		// Check if there is no ongoing message
		if (handle->msg_status != GR_SPI_MSG_IN_PROGRESS) {
			// Check if there is a message in the Tx circular buffer
			if (GR_CircularBuffer_Peek(handle->tx_buffer) != NULL) {
				// Pop off the message
				handle->current_msg =
				    GR_CircularBuffer_Pop(handle->tx_buffer);
				handle->msg_status = GR_SPI_MSG_IN_PROGRESS;
				handle->current_tx_msg_index = 0;
			}
		}
		// Now check if there is an ongoing message
		if (handle->msg_status == GR_SPI_MSG_IN_PROGRESS) {
			GR_SPI_Transfer_Tx_Bytes(handle);
		}
	}
}

// SPIx_IRQn is defined in stm32 libraries
uint32_t GR_SPI_Get_IRQn(SPI_TypeDef *SPIx)
{
	switch ((uint32_t)SPIx) {
		case (uint32_t)SPI1:
			return SPI1_IRQn; // 35
		case (uint32_t)SPI2:
			return SPI2_IRQn; // 36
		case (uint32_t)SPI3:
			return SPI3_IRQn; // 51
		default:
			return GR_SPI_UNKNOWN_IRQN;
	}
}

void GR_SPI_Enable_Clocks(GR_SPI_Handler *handle)
{
	// Enable GPIO clock
	uint32_t GPIOx_Port;
	for (uint32_t i = 0; i < handle->pins->num_pins; i++) {
		switch ((uint32_t)handle->pins->GPIOx[i]) {
			case (uint32_t)GPIOA:
				GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOA;
				break;
			case (uint32_t)GPIOB:
				GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOB;
				break;
			case (uint32_t)GPIOC:
				GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOC;
				break;
			case (uint32_t)GPIOD:
				GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOD;
				break;
			case (uint32_t)GPIOE:
				GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOE;
				break;
			case (uint32_t)GPIOF:
				GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOF;
				break;
			case (uint32_t)GPIOG:
				GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOG;
				break;
			// Doesn't exist on G4 board
			// case (uint32_t) GPIOH:
			//     GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOH;
			//     break;
			default: // Do nothing (unknown GPIOx)
				continue;
		}
		LL_AHB2_GRP1_EnableClock(GPIOx_Port);
	}

	// Enable SPI clock
	switch ((uint32_t)handle->pins->SPIx) {
		case (uint32_t)SPI1:
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
			break;
		case (uint32_t)SPI2:
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
			break;
		case (uint32_t)SPI3:
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
			break;
		default: // Do nothing (unknown SPIx)
	}
}

void GR_SPI_Send(GR_SPI_Handler *handle, GR_SPI_Message *msg)
{
	// Check if a message is currently in progress
	if (handle->msg_status == GR_SPI_MSG_IN_PROGRESS) {
		// Push the new message onto the Tx circular buffer
		GR_CircularBuffer_Push(handle->tx_buffer, msg, msg->size);
	} else {
		handle->msg_status = GR_SPI_MSG_IN_PROGRESS;
		handle->current_tx_msg_index = 0;
		handle->current_rx_msg_index = 0;
		handle->current_msg = msg;

		LL_GPIO_ResetOutputPin(handle->pins->GPIOx[3], LL_GPIO_PIN_0);

		GR_SPI_Transfer_Tx_Bytes(handle);
	}
}

GR_SPI_Message *GR_SPI_Receive(GR_SPI_Handler *handle)
{
	// Returns NULL if there is no message to receive
	GR_SPI_Message *data_ptr = GR_CircularBuffer_Pop(handle->rx_buffer);
	return data_ptr;
}

void GR_SPI_Configure_Pins(GR_SPI_Handler *handle,
			   LL_GPIO_InitTypeDef *pin_config)
{
	LL_GPIO_StructInit(pin_config); // Default config values
	pin_config->Speed =
	    LL_GPIO_SPEED_FREQ_VERY_HIGH;   // Very high output speed
	pin_config->Pull = LL_GPIO_PULL_NO; // No pull-up or pull-down
					    // resistance
	pin_config->OutputType =
	    LL_GPIO_OUTPUT_PUSHPULL; // Push-pull output (not open-drain)
	pin_config->Mode =
	    LL_GPIO_MODE_ALTERNATE; // Alternate pin function mode
	pin_config->Alternate =
	    handle->pins
		->alternate_function_number; // Alternate function number
	for (uint32_t i = 0; i < handle->pins->num_pins; i++) {
		pin_config->Pin = handle->pins->pin_nums[i];
		LL_GPIO_Init(handle->pins->GPIOx[i], pin_config);
	}
}

void GR_SPI_Transfer_Tx_Bytes(GR_SPI_Handler *handle)
{
	// Make this function atomic
	LL_SPI_DisableIT_TXE(handle->pins->SPIx); // Empty Tx buffer

	uint16_t tx_index = handle->current_tx_msg_index,
		 msg_size = handle->current_msg->size;
	// Send two bytes if transferring 16 bits
	if (handle->transfer_size && tx_index <= msg_size - 2) {
		uint16_t data =
		    (((uint16_t)handle->current_msg->data[tx_index]) << 8) +
		    handle->current_msg->data[tx_index + 1];
		LL_SPI_TransmitData16(handle->pins->SPIx, data);
		handle->current_tx_msg_index += 2;
	}
	// Send one byte if transferring 8 bits or transferring 16 bits with
	// only 8 bits left
	else if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_8 &&
		 tx_index <= msg_size - 1) {
		uint8_t data = handle->current_msg->data[tx_index];
		LL_SPI_TransmitData8(handle->pins->SPIx, data);
		handle->current_tx_msg_index += 1;
	} else {
		// ERROR: Message was already fully transmitted
	}

	// Mark message send complete
	if (tx_index == msg_size) {
		handle->current_tx_msg_index = 0;
		// Queue up next message to be sent
		if (GR_CircularBuffer_Peek(handle->tx_buffer)) {
			handle->current_msg =
			    GR_CircularBuffer_Pop(handle->tx_buffer);
		}
	}

	// Resume TXE interrupts
	LL_SPI_EnableIT_TXE(handle->pins->SPIx); // Empty Tx buffer
}

void GR_SPI_Close(GR_SPI_Handler *handler)
{
	return; // STUB
}