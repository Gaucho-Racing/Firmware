// Wonderful SPI Abstraction Layer courtesy of Bailey, Colin, and Aaryan
#include "spi.h"

#include <stdlib.h>

// Transfer sizes
#define GR_SPI_TRANSFER_SIZE_8 8
#define GR_SPI_TRANSFER_SIZE_16 16

// Current message status codes
#define GR_SPI_MSG_IN_PROGRESS 1
#define GR_SPI_MSG_IDLE 0
#define GR_SPI_INVALID_TX_SIZE 65535 // max message size is 2^16 bytes

#define GR_SPI_ERR_NONE 0
#define GR_SPI_ERR_FRE -1
#define GR_SPI_ERR_OVR -2
#define GR_SPI_ERR_MODF -3
#define GR_SPI_ERR_CRCERR -4
#define GR_SPI_ERR_RXFULL -5
#define GR_SPI_ERR_BAD_INIT_NVIC -6
#define GR_SPI_ERR_BAD_INIT_RXBUF -7
#define GR_SPI_ERR_BAD_INIT_TXBUF -8
#define GR_SPI_ERR_BAD_SPIX -9
#define GR_SPI_ERR_BAD_ADD -10
#define GR_SPI_ERR_FULL_TRANSMIT -11;

static GR_SPI_Handler *GR_SPI_HANDLER_LUT[3]; // Stores pointer to the handle structs for SPI1
					      // (0), SPI2 (1), & SPI3 (2)

void GR_SPI_Initialize(GR_SPI_Handler *handle, LL_SPI_InitTypeDef *config, GR_SPI_Pins *pin_config)
{
	// Create Circular Buffers
	CircularBuffer *circular_buffer_ptr;
	circular_buffer_ptr = GR_CircularBuffer_Create(GR_SPI_BUFFER_MESSAGE_CAPACITY);
	if (circular_buffer_ptr == NULL) {
		handle->error_status = GR_SPI_ERR_BAD_INIT_RXBUF;
		return;
	} else {
		handle->rx_buffer = circular_buffer_ptr;
	}
	circular_buffer_ptr = GR_CircularBuffer_Create(GR_SPI_BUFFER_MESSAGE_CAPACITY);
	if (circular_buffer_ptr == NULL) {
		handle->error_status = GR_SPI_ERR_BAD_INIT_TXBUF;
		return;
	} else {
		handle->tx_buffer = circular_buffer_ptr;
	}

	// Copy over config values
	handle->spi_config = (LL_SPI_InitTypeDef *)malloc(sizeof(LL_SPI_InitTypeDef)); // Make memory for LL_SPI_InitTypeDef
										       // config struct
	*handle->spi_config = *config;

	// Deep copy of pins struct
	handle->pins = (GR_SPI_Pins *)malloc(sizeof(GR_SPI_Pins));				    // Make memory for GR_SPI_Pins struct
	handle->pins->pin_nums = (uint32_t *)malloc(pin_config->num_pins * sizeof(uint32_t));	    // Make memory for pin_nums[num_pins]
	handle->pins->GPIOx = (GPIO_TypeDef **)malloc(pin_config->num_pins * sizeof(GPIO_TypeDef)); // Make memory for GPIOx[num_pins]
	for (uint32_t i = 0; i < pin_config->num_pins; i++) {
		handle->pins->pin_nums[i] = pin_config->pin_nums[i];
		handle->pins->GPIOx[i] = pin_config->GPIOx[i];
	}
	handle->pins->SPIx = pin_config->SPIx;
	handle->pins->num_pins = pin_config->num_pins;
	handle->pins->alternate_function_number = pin_config->alternate_function_number;

	// Set current message variables
	handle->current_msg = NULL;
	handle->current_rx_msg_index = 0;
	handle->current_tx_msg_index = 0;
	handle->msg_status = GR_SPI_MSG_IDLE;

	// Error status variable
	handle->error_status = GR_SPI_ERR_NONE;

	// Store handle in lookup table for interrupts
	if (handle->pins->SPIx == SPI1) {
		GR_SPI_HANDLER_LUT[0] = handle;
	} else if (handle->pins->SPIx == SPI2) {
		GR_SPI_HANDLER_LUT[1] = handle;
	} else if (handle->pins->SPIx == SPI3) {
		GR_SPI_HANDLER_LUT[2] = handle;
	} else {
		handle->error_status = GR_SPI_ERR_BAD_SPIX;
		return;
	}

	// Disable SPI
	LL_SPI_Disable(handle->pins->SPIx);

	// Enable GPIO and SPI clocks
	GR_SPI_Enable_Clocks(handle);

	// Configure GPIOs
	LL_GPIO_InitTypeDef gpio_pin_config;
	GR_SPI_Configure_Pins(handle, &gpio_pin_config);

	// Configure SPI protocol with config values
	LL_SPI_Init(handle->pins->SPIx, config);
	// Transaction size is 8-bits
	if (config->DataWidth <= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2)) {
		handle->transfer_size = GR_SPI_TRANSFER_SIZE_8;
		// Make the RXNE trigger when >= 8 bits are received
		handle->pins->SPIx->CR2 |= SPI_CR2_FRXTH;
	}
	// Transaction size is 16-bits
	else {
		handle->transfer_size = GR_SPI_TRANSFER_SIZE_16;
		// Make the RXNE trigger when >= 16 bits are received
		handle->pins->SPIx->CR2 &= ~SPI_CR2_FRXTH;
	}

	// Enable SPI peripheral after BSY flag clears
	while (LL_SPI_IsActiveFlag_BSY(handle->pins->SPIx)) {}
	LL_SPI_Enable(handle->pins->SPIx);

	// Enable interrupts in NVIC
	int SPI_IRQn = GR_SPI_Get_IRQn(handle->pins->SPIx);
	if (SPI_IRQn != GR_SPI_UNKNOWN_IRQN) {
		NVIC_SetPriority(SPI_IRQn, 1);
		NVIC_EnableIRQ(SPI_IRQn);
	} else {
		handle->error_status = GR_SPI_ERR_BAD_INIT_NVIC;
		return; // Throw an error
	}

	// Enable interrupts at peripheral level (TXE is enabled in GR_SPI_Begin_New_Tx and disabled in GR_SPI_Transfer_Tx_Bytes)
	LL_SPI_EnableIT_ERR(handle->pins->SPIx);  // Error interrupt
	LL_SPI_EnableIT_RXNE(handle->pins->SPIx); // Not empty Rx buffer
}

void GR_SPI_Send(GR_SPI_Handler *handle, GR_SPI_Message *msg)
{
	GR_SPI_Message temp_msg;
	temp_msg.size = msg->size;
	temp_msg.data = malloc(temp_msg.size * sizeof(uint8_t));
	for (int i = 0; i < temp_msg.size; i++) {
		temp_msg.data[i] = msg->data[i];
	}

	// Push the new message (copy) onto the Tx circular buffer
	GR_CircularBuffer_Push(handle->tx_buffer, &temp_msg, sizeof(GR_SPI_Message));

	// Check if there is no message in progress
	if (handle->msg_status != GR_SPI_MSG_IN_PROGRESS) {
		GR_SPI_Begin_New_Tx(handle);
	}
}

void GR_SPI_Receive(GR_SPI_Handler *handle, GR_SPI_Message *dest_msg)
{
	GR_SPI_Message *rx_msg = GR_CircularBuffer_Pop(handle->rx_buffer);

	// Check if there was a message returned by buffer pop
	if (rx_msg) {
		// If sizes don't match, re-malloc correct size inside destination message
		if (dest_msg->size != rx_msg->size) {
			free(dest_msg->data);
			dest_msg->size = rx_msg->size;
			dest_msg->data = malloc(rx_msg->size * sizeof(uint8_t));
		}

		// Copy over data into the destination message
		for (int i = 0; i < dest_msg->size; i++) {
			dest_msg->data[i] = rx_msg->data[i];
		}

		// Deallocate rx_msg
		GR_SPI_Msg_Free(rx_msg);
	}
}

void SPI1_IRQHandler(void) { GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[0]); }

void SPI2_IRQHandler(void) { GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[1]); }

void SPI3_IRQHandler(void) { GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[2]); }

void GR_SPI_Interrupt_Handler(GR_SPI_Handler *handle)
{
	// Check if called by error interrupt
	// Frame format error
	if (LL_SPI_IsActiveFlag_FRE(handle->pins->SPIx)) {
		handle->error_status = GR_SPI_ERR_FRE;
		return;
	}
	// Overrun error
	else if (LL_SPI_IsActiveFlag_OVR(handle->pins->SPIx)) {
		handle->error_status = GR_SPI_ERR_OVR;
		return;
	}
	// Fault mode error
	else if (LL_SPI_IsActiveFlag_MODF(handle->pins->SPIx)) {
		handle->error_status = GR_SPI_ERR_MODF;
		return;
	}
	// CRC error
	else if (LL_SPI_IsActiveFlag_CRCERR(handle->pins->SPIx)) {
		handle->error_status = GR_SPI_ERR_CRCERR;
		return;
	}

	// No errors detected...

	// Check if Rx circular buffer is not empty
	if (LL_SPI_IsActiveFlag_RXNE(handle->pins->SPIx)) {
		uint16_t rx_index = handle->current_rx_msg_index, msg_size = handle->current_msg->size;
		// Queue the message into the circular buffer
		if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_16 && rx_index <= msg_size - 2) {
			uint16_t data = LL_SPI_ReceiveData16(handle->pins->SPIx);
			handle->current_msg->data[rx_index + 1] = (uint8_t)(data & 0xFF);
			handle->current_msg->data[rx_index] = (uint8_t)(data >> 8);
			handle->current_rx_msg_index += 2;
		} else if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_8 && rx_index <= msg_size - 1) {
			uint8_t data = LL_SPI_ReceiveData8(handle->pins->SPIx);
			handle->current_msg->data[rx_index] = data;
			handle->current_rx_msg_index += 1;
		} else {
			// ERROR: Current message is full
			handle->error_status = GR_SPI_ERR_RXFULL;
		}

		// Push current message into Rx circular buffer to mark completion
		if (handle->current_rx_msg_index == msg_size) {
			handle->current_rx_msg_index = 0;
			GR_CircularBuffer_Push(handle->rx_buffer, (void *)handle->current_msg, sizeof(GR_SPI_Message));
			// Free message struct but not the byte array within the message (pointed to by the message on the rx_buffer)
			free(handle->current_msg);
			handle->current_msg = NULL;
			// Finish transaction
			LL_GPIO_SetOutputPin(handle->pins->GPIOx[3], handle->pins->pin_nums[3]);
			// Only go to IDLE when no additional messages are in pipeline
			if (GR_CircularBuffer_IsEmpty(handle->tx_buffer)) {
				handle->msg_status = GR_SPI_MSG_IDLE;
			} else {
				GR_SPI_Begin_New_Tx(handle);
			}
		}
	}
	// Check if Tx is empty
	if (LL_SPI_IsActiveFlag_TXE(handle->pins->SPIx)) {
		// Continue sending bytes in transaction
		if (handle->current_tx_msg_index != GR_SPI_INVALID_TX_SIZE) {
			GR_SPI_Transfer_Tx_Bytes(handle);
		}
	}
}

// SPIx_IRQn is defined in stm32 libraries
uint32_t GR_SPI_Get_IRQn(SPI_TypeDef *SPIx)
{
	if (SPIx == SPI1) {
		return SPI1_IRQn; // 35
	} else if (SPIx == SPI2) {
		return SPI2_IRQn; // 36
	} else if (SPIx == SPI3) {
		return SPI3_IRQn; // 51
	} else {
		return GR_SPI_UNKNOWN_IRQN;
	}
}

void GR_SPI_Enable_Clocks(GR_SPI_Handler *handle)
{
	uint32_t GPIOx_Port;

	for (uint32_t i = 0; i < handle->pins->num_pins; i++) {
		GPIO_TypeDef *gpio = handle->pins->GPIOx[i];

		if (gpio == GPIOA) {
			GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOA;
		} else if (gpio == GPIOB) {
			GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOB;
		} else if (gpio == GPIOC) {
			GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOC;
		} else if (gpio == GPIOD) {
			GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOD;
		} else if (gpio == GPIOE) {
			GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOE;
		} else if (gpio == GPIOF) {
			GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOF;
		} else if (gpio == GPIOG) {
			GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOG;
		}
		// Note: GPIOH does not exist on G4 board
		else {
			continue; // unknown GPIOx
		}

		LL_AHB2_GRP1_EnableClock(GPIOx_Port);
	}

	// Enable SPI clock
	if (handle->pins->SPIx == SPI1) {
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
	} else if (handle->pins->SPIx == SPI2) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
	} else if (handle->pins->SPIx == SPI3) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
	} else {
		// ERROR: Unexpected SPI address
		handle->error_status = GR_SPI_ERR_BAD_ADD;
		return;
	}
}

void GR_SPI_Begin_New_Tx(GR_SPI_Handler *handle)
{
	// Re-initiate a transaction
	handle->msg_status = GR_SPI_MSG_IN_PROGRESS;
	handle->current_tx_msg_index = 0;
	handle->current_rx_msg_index = 0;
	handle->current_msg = GR_CircularBuffer_Pop(handle->tx_buffer);

	// Pull chip select to active low
	LL_GPIO_ResetOutputPin(handle->pins->GPIOx[3], handle->pins->pin_nums[3]);

	// Enable TXE interrupts for loading bytes into TX buffer
	LL_SPI_EnableIT_TXE(handle->pins->SPIx); // Empty Tx buffer
}

void GR_SPI_Transfer_Tx_Bytes(GR_SPI_Handler *handle)
{
	uint16_t tx_index = handle->current_tx_msg_index, msg_size = handle->current_msg->size;
	// Send two bytes if transferring 16 bits
	if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_16 && tx_index <= msg_size - 2) {
		uint16_t data = (((uint16_t)handle->current_msg->data[tx_index]) << 8) + handle->current_msg->data[tx_index + 1];
		LL_SPI_TransmitData16(handle->pins->SPIx, data);
		handle->current_tx_msg_index += 2;
	}
	// Send one byte if transferring 8 bits or transferring 16 bits with only 8 bits left
	else if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_8 && tx_index <= msg_size - 1) {
		uint8_t data = handle->current_msg->data[tx_index];
		LL_SPI_TransmitData8(handle->pins->SPIx, data);
		handle->current_tx_msg_index += 1;
	} else {
		// ERROR: Message was already fully transmitted
		handle->error_status = GR_SPI_ERR_FULL_TRANSMIT;
		return;
	}

	// Mark message send complete
	if (handle->current_tx_msg_index == msg_size) {
		handle->current_tx_msg_index = 0;
		// Queue up next message to be sent
		if (!GR_CircularBuffer_IsEmpty(handle->tx_buffer)) {
			handle->current_msg = GR_CircularBuffer_Pop(handle->tx_buffer);
		}
		// No more messages to load into transfer buffer
		else {
			handle->current_tx_msg_index = GR_SPI_INVALID_TX_SIZE;
			LL_SPI_DisableIT_TXE(handle->pins->SPIx);
		}
	}
}

void GR_SPI_Configure_Pins(GR_SPI_Handler *handle, LL_GPIO_InitTypeDef *pin_config)
{
	LL_GPIO_StructInit(pin_config);					 // Default config values
	pin_config->Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;		 // Very high output speed
	pin_config->Pull = LL_GPIO_PULL_NO;				 // No pull-up or pull-down
	pin_config->OutputType = LL_GPIO_OUTPUT_PUSHPULL;		 // Push-pull output (not open-drain)
	pin_config->Mode = LL_GPIO_MODE_ALTERNATE;			 // Alternate pin function mode
	pin_config->Alternate = handle->pins->alternate_function_number; // Alternate function number
	for (uint32_t i = 0; i < handle->pins->num_pins; i++) {
		pin_config->Pin = handle->pins->pin_nums[i];
		LL_GPIO_Init(handle->pins->GPIOx[i], pin_config);
	}
}

void GR_SPI_Close(GR_SPI_Handler *handle)
{
	// Safety Checks
	LL_GPIO_SetOutputPin(handle->pins->GPIOx[3], handle->pins->pin_nums[3]); // Set CS high

	// Set all the pins analog
	for (int i = 0; i < 3; i++) {
		LL_GPIO_SetPinMode(handle->pins->GPIOx[i], handle->pins->pin_nums[i], LL_GPIO_MODE_ANALOG);
	}

	// Disable and DeInit
	LL_SPI_Disable(handle->pins->SPIx);
	LL_SPI_DeInit(handle->pins->SPIx);

	// Deallocate memory
	if (handle->spi_config) {
		free(handle->spi_config);
	}
	if (handle->pins->GPIOx) {
		free(handle->pins->GPIOx);
	}
	if (handle->pins->pin_nums) {
		free(handle->pins->pin_nums);
	}
	if (handle->pins) {
		free(handle->pins);
	}
	GR_SPI_Msg_Free(handle->current_msg);
	GR_CircularBuffer_Free(&handle->rx_buffer);
	GR_CircularBuffer_Free(&handle->tx_buffer);
}

void GR_SPI_Msg_Free(GR_SPI_Message *msg)
{
	if (msg) {
		if (msg->data) {
			free(msg->data);
		}
		free(msg);
	}
}

bool GR_SPI_IsRxEmpty(GR_SPI_Handler *handle) { return GR_CircularBuffer_IsEmpty(handle->rx_buffer); }
