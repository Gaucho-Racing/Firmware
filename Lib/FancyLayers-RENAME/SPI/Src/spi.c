// Wonderful SPI Abstraction Layer courtesy of Bailey, Colin, William, and Aaryan
#include "spi.h"

#include <stdlib.h>
#include <string.h>

#include "msgBuffer.h"

static GR_SPI_Handler *GR_SPI_HANDLER_LUT[3]; // Stores pointer to the handle structs for SPI1
					      // (0), SPI2 (1), & SPI3 (2)

void GR_SPI_Initialize(GR_SPI_Handler *handle, LL_SPI_InitTypeDef *config, GR_SPI_Pins *pin_config)
{
	if (!handle) {
		return;
	}
	if (!config || !pin_config) {
		handle->error_status = GR_SPI_ERR_BAD_ARGS;
		return;
	}

	// Error status variable
	handle->error_status = GR_SPI_ERR_NONE;

	// Create Circular Buffers and assign the rx and the tx pointers
	GR_MsgBuffer *msg_buffer_ptr = GR_MsgBuffer_Create(GR_SPI_BUFFER_BYTE_CAPACITY);
	if (msg_buffer_ptr == NULL) {
		handle->error_status = GR_SPI_ERR_BAD_INIT_RXBUF;
		return;
	} else {
		handle->rx_buffer = msg_buffer_ptr;
	}
	msg_buffer_ptr = GR_MsgBuffer_Create(GR_SPI_BUFFER_BYTE_CAPACITY);
	if (msg_buffer_ptr == NULL) {
		handle->error_status = GR_SPI_ERR_BAD_INIT_TXBUF;
		free(handle->rx_buffer);
		return;
	} else {
		handle->tx_buffer = msg_buffer_ptr;
	}

	// Copy over config values
	memcpy(&handle->spi_config, config, sizeof(LL_SPI_InitTypeDef));

	// Deep copy of pins config struct to internal handle pin struct
	memcpy(&handle->pins, pin_config, sizeof(GR_SPI_Pins));

	// Store handle in lookup table for interrupts
	if (handle->pins.SPIx == SPI1) {
		GR_SPI_HANDLER_LUT[0] = handle;
	} else if (handle->pins.SPIx == SPI2) {
		GR_SPI_HANDLER_LUT[1] = handle;
	} else if (handle->pins.SPIx == SPI3) {
		GR_SPI_HANDLER_LUT[2] = handle;
	} else {
		handle->error_status = GR_SPI_ERR_BAD_SPIX;
		return;
	}

	// Disable SPI
	LL_SPI_Disable(handle->pins.SPIx);

	// Enable GPIO and SPI clocks
	GR_SPI_Enable_Clocks(handle);

	// Configure GPIOs
	GR_SPI_Configure_Pins(handle);

	// Configure SPI protocol with config values
	LL_SPI_Init(handle->pins.SPIx, config);
	// Transaction size is 8-bits
	if (config->DataWidth <= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2)) {
		handle->transfer_size = GR_SPI_TRANSFER_SIZE_8;
		// Make the RXNE trigger when >= 8 bits are received
		handle->pins.SPIx->CR2 |= SPI_CR2_FRXTH;
	}
	// Transaction size is 16-bits
	else {
		handle->transfer_size = GR_SPI_TRANSFER_SIZE_16;
		// Make the RXNE trigger when >= 16 bits are received
		handle->pins.SPIx->CR2 &= ~SPI_CR2_FRXTH;
	}

	// Set current message variables
	handle->current_msg.size = handle->transfer_size == GR_SPI_TRANSFER_SIZE_16 ? 2 : 1;
	handle->current_msg.data = (uint8_t *)malloc(GR_SPI_MAX_MSG_BYTE_SIZE * sizeof(uint8_t));
	handle->current_rx_msg_index = 0;
	handle->current_tx_msg_index = 0;
	handle->msg_status = GR_SPI_MSG_IDLE;

	// Enable SPI peripheral after BSY flag clears
	while (LL_SPI_IsActiveFlag_BSY(handle->pins.SPIx)) {}
	LL_SPI_Enable(handle->pins.SPIx);

	// Enable interrupts in NVIC
	int SPI_IRQn = GR_SPI_Get_IRQn(handle->pins.SPIx);
	if (SPI_IRQn != GR_SPI_UNKNOWN_IRQN) {
		NVIC_SetPriority(SPI_IRQn, 1);
		NVIC_EnableIRQ(SPI_IRQn);
	} else {
		handle->error_status = GR_SPI_ERR_BAD_INIT_NVIC;
		return; // Throw an error
	}

	// Enable interrupts at peripheral level (TXE is enabled in GR_SPI_Begin_New_Tx and disabled in GR_SPI_Transfer_Tx_Bytes)
	LL_SPI_EnableIT_ERR(handle->pins.SPIx);	 // Error interrupt
	LL_SPI_EnableIT_RXNE(handle->pins.SPIx); // Not empty Rx buffer
}

bool GR_SPI_Send(GR_SPI_Handler *handle, GR_SPI_Message *msg)
{
	if (!handle || !msg || msg->size > GR_SPI_MAX_MSG_BYTE_SIZE) {
		return false;
	}

	// Push the new message (copy) onto the Tx circular buffer. Fails if not enough space or bad args.
	if (!GR_MsgBuffer_Push(handle->tx_buffer, msg->data, msg->size)) {
		return false;
	}

	// Check if there is no message in progress
	if (handle->msg_status != GR_SPI_MSG_IN_PROGRESS) {
		GR_SPI_Begin_New_Tx(handle);
	}

	return true;
}

void GR_SPI_Receive(GR_SPI_Handler *handle, GR_SPI_Message *dest_msg)
{
	if (!handle || !dest_msg || !dest_msg->data || dest_msg->size != GR_MsgBuffer_PeekMsgSize(handle->rx_buffer)) {
		return;
	}

	GR_MsgBuffer_Pop(handle->rx_buffer, dest_msg->data);
}

void SPI1_IRQHandler(void)
{
	GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[0]);
}

void SPI2_IRQHandler(void)
{
	GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[1]);
}

void SPI3_IRQHandler(void)
{
	GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[2]);
}

void GR_SPI_Interrupt_Handler(GR_SPI_Handler *handle)
{
	if (!handle) {
		return;
	}

	// Check if called by error interrupt
	// Frame format error
	if (LL_SPI_IsActiveFlag_FRE(handle->pins.SPIx)) {
		handle->error_status = GR_SPI_ERR_FRE;
		return;
	}
	// Overrun error
	else if (LL_SPI_IsActiveFlag_OVR(handle->pins.SPIx)) {
		handle->error_status = GR_SPI_ERR_OVR;
		return;
	}
	// Fault mode error
	else if (LL_SPI_IsActiveFlag_MODF(handle->pins.SPIx)) {
		handle->error_status = GR_SPI_ERR_MODF;
		return;
	}
	// CRC error
	else if (LL_SPI_IsActiveFlag_CRCERR(handle->pins.SPIx)) {
		handle->error_status = GR_SPI_ERR_CRCERR;
		return;
	}

	// No errors detected...

	// Check if Rx circular buffer is not empty
	if (LL_SPI_IsActiveFlag_RXNE(handle->pins.SPIx)) {
		// Start a transaction when starting to receive data without an ongoing transaction
		if (handle->msg_status != GR_SPI_MSG_IN_PROGRESS) {
			handle->current_msg.size = handle->transfer_size == GR_SPI_TRANSFER_SIZE_16 ? 2 : 1;
			handle->current_tx_msg_index = 0;
			handle->current_rx_msg_index = 0;
			handle->msg_status = GR_SPI_MSG_IN_PROGRESS;
		}
		uint16_t rx_index = handle->current_rx_msg_index, msg_size = handle->current_msg.size;
		// Queue the message into the circular buffer
		if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_16 && rx_index <= msg_size - 2) {
			uint16_t data = LL_SPI_ReceiveData16(handle->pins.SPIx);
			handle->current_msg.data[rx_index + 1] = (uint8_t)(data & 0xFF);
			handle->current_msg.data[rx_index] = (uint8_t)(data >> 8);
			handle->current_rx_msg_index += 2;
		} else if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_8 && rx_index <= msg_size - 1) {
			uint8_t data = LL_SPI_ReceiveData8(handle->pins.SPIx);
			handle->current_msg.data[rx_index] = data;
			handle->current_rx_msg_index += 1;
		} else {
			// ERROR: Current message is full
			handle->error_status = GR_SPI_ERR_RXFULL;
		}

		// Push current message into Rx circular buffer to mark completion
		if (handle->current_rx_msg_index == msg_size) {
			handle->current_rx_msg_index = 0;
			GR_MsgBuffer_Push(handle->rx_buffer, handle->current_msg.data, handle->current_msg.size);
			if (handle->spi_config.Mode == LL_SPI_MODE_MASTER) {
				// Finish transaction
				LL_GPIO_SetOutputPin(handle->pins.NCS_port, handle->pins.NCS_pin);
				// Only go to IDLE when no additional messages are in pipeline
				if (GR_MsgBuffer_IsEmpty(handle->tx_buffer)) {
					handle->msg_status = GR_SPI_MSG_IDLE;
				} else {
					GR_SPI_Begin_New_Tx(handle);
				}
			}
		}
	}
	// Check if Tx is empty
	if (LL_SPI_IsActiveFlag_TXE(handle->pins.SPIx)) {
		// Continue sending bytes in transaction
		if (handle->spi_config.Mode == LL_SPI_MODE_MASTER && handle->current_tx_msg_index != GR_SPI_INVALID_TX_SIZE && handle->msg_status == GR_SPI_MSG_IN_PROGRESS) {
			GR_SPI_Transfer_Tx_Bytes(handle);
		}
	}
}

int GR_SPI_Get_IRQn(SPI_TypeDef *SPIx)
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

uint32_t GR_SPI_Get_GPIO_Clock(GPIO_TypeDef *GPIOx)
{
	if (GPIOx == GPIOA) {
		return LL_AHB2_GRP1_PERIPH_GPIOA;
	} else if (GPIOx == GPIOB) {
		return LL_AHB2_GRP1_PERIPH_GPIOB;
	} else if (GPIOx == GPIOC) {
		return LL_AHB2_GRP1_PERIPH_GPIOC;
	} else if (GPIOx == GPIOD) {
		return LL_AHB2_GRP1_PERIPH_GPIOD;
	} else if (GPIOx == GPIOE) {
		return LL_AHB2_GRP1_PERIPH_GPIOE;
	} else if (GPIOx == GPIOF) {
		return LL_AHB2_GRP1_PERIPH_GPIOF;
	} else if (GPIOx == GPIOG) {
		return LL_AHB2_GRP1_PERIPH_GPIOG;
	}
	// Note: GPIOH does not exist on G4 board
	else {
		return GR_SPI_UNKNOWN_CLOCK;
	}
}

void GR_SPI_Enable_Clocks(GR_SPI_Handler *handle)
{
	if (!handle) {
		return;
	}

	uint32_t COPI_clock, CIPO_clock, SCLK_clock, NCS_clock;

	COPI_clock = GR_SPI_Get_GPIO_Clock(handle->pins.COPI_port);
	CIPO_clock = GR_SPI_Get_GPIO_Clock(handle->pins.CIPO_port);
	SCLK_clock = GR_SPI_Get_GPIO_Clock(handle->pins.SCLK_port);
	NCS_clock = GR_SPI_Get_GPIO_Clock(handle->pins.NCS_port);

	if (!COPI_clock || !CIPO_clock || !SCLK_clock || !NCS_clock) {
		return;
	}

	LL_AHB2_GRP1_EnableClock(COPI_clock);
	LL_AHB2_GRP1_EnableClock(CIPO_clock);
	LL_AHB2_GRP1_EnableClock(SCLK_clock);
	LL_AHB2_GRP1_EnableClock(NCS_clock);

	// Enable SPI clock
	if (handle->pins.SPIx == SPI1) {
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
	} else if (handle->pins.SPIx == SPI2) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
	} else if (handle->pins.SPIx == SPI3) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
	} else {
		// ERROR: Unexpected SPI address
		handle->error_status = GR_SPI_ERR_BAD_ADD;
		return;
	}
}

void GR_SPI_Begin_New_Tx(GR_SPI_Handler *handle)
{
	if (!handle) {
		return;
	}

	handle->current_msg.size = GR_MsgBuffer_PeekMsgSize(handle->tx_buffer);
	GR_MsgBuffer_Pop(handle->tx_buffer, handle->current_msg.data);

	// ERROR: unexpected zero size
	if (handle->current_msg.size < 1) {
		return;
	}

	// Re-initiate a transaction
	handle->msg_status = GR_SPI_MSG_IN_PROGRESS;
	handle->current_tx_msg_index = 0;
	handle->current_rx_msg_index = 0;

	// Pull chip select to active low
	LL_GPIO_ResetOutputPin(handle->pins.NCS_port, handle->pins.NCS_pin);

	// Enable TXE interrupts for loading bytes into TX buffer
	LL_SPI_EnableIT_TXE(handle->pins.SPIx); // Empty Tx buffer
}

void GR_SPI_Transfer_Tx_Bytes(GR_SPI_Handler *handle)
{
	if (!handle) {
		return;
	}

	uint16_t tx_index = handle->current_tx_msg_index, msg_size = handle->current_msg.size;
	// Send two bytes if transferring 16 bits
	if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_16 && tx_index <= msg_size - 2) {
		uint16_t data = (((uint16_t)handle->current_msg.data[tx_index]) << 8) + handle->current_msg.data[tx_index + 1];
		LL_SPI_TransmitData16(handle->pins.SPIx, data);
		handle->current_tx_msg_index += 2;
	}
	// Send one byte if transferring 8 bits or transferring 16 bits with only 8 bits left
	else if (handle->transfer_size == GR_SPI_TRANSFER_SIZE_8 && tx_index <= msg_size - 1) {
		uint8_t data = handle->current_msg.data[tx_index];
		LL_SPI_TransmitData8(handle->pins.SPIx, data);
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
		if (!GR_MsgBuffer_IsEmpty(handle->tx_buffer)) {
			handle->current_msg.size = GR_MsgBuffer_PeekMsgSize(handle->tx_buffer);
			GR_MsgBuffer_Pop(handle->tx_buffer, handle->current_msg.data);
		}
		// No more messages to load into transfer buffer
		else {
			handle->current_tx_msg_index = GR_SPI_INVALID_TX_SIZE;
			LL_SPI_DisableIT_TXE(handle->pins.SPIx);
		}
	}
}

void GR_SPI_Configure_Pins(GR_SPI_Handler *handle)
{
	if (!handle) {
		return;
	}

	LL_GPIO_InitTypeDef pin_config;

	// Universal settings for all SPI pins
	LL_GPIO_StructInit(&pin_config);		 // Default config values
	pin_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH; // Very high output speed
	pin_config.Pull = LL_GPIO_PULL_NO;		 // No pull-up or pull-down
	pin_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL; // Push-pull output (not open-drain)
	pin_config.Mode = LL_GPIO_MODE_ALTERNATE;	 // Alternate pin function mode
	pin_config.Alternate = handle->pins.AFN;	 // Alternate function number

	// COPI
	pin_config.Pin = handle->pins.COPI_pin;
	LL_GPIO_Init(handle->pins.COPI_port, &pin_config);

	// CIPO
	pin_config.Pin = handle->pins.CIPO_pin;
	LL_GPIO_Init(handle->pins.CIPO_port, &pin_config);

	// SCLK
	pin_config.Pin = handle->pins.SCLK_pin;
	LL_GPIO_Init(handle->pins.SCLK_port, &pin_config);

	// NCS
	pin_config.Pin = handle->pins.NCS_pin;
	LL_GPIO_Init(handle->pins.NCS_port, &pin_config);
}

void GR_SPI_Close(GR_SPI_Handler *handle)
{
	if (!handle) {
		return;
	}

	// Safety Checks
	LL_GPIO_SetOutputPin(handle->pins.NCS_port, handle->pins.NCS_pin); // Set CS high

	// Set all the pins analog
	for (int i = 0; i < 3; i++) {
		LL_GPIO_SetPinMode(handle->pins.NCS_port, handle->pins.NCS_pin, LL_GPIO_MODE_ANALOG);
	}

	// Disable and DeInit
	LL_SPI_Disable(handle->pins.SPIx);
	LL_SPI_DeInit(handle->pins.SPIx);

	// Deallocate memory
	if (handle->current_msg.data) {
		free(handle->current_msg.data);
	}
	GR_MsgBuffer_Free(handle->rx_buffer);
	GR_MsgBuffer_Free(handle->tx_buffer);
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

bool GR_SPI_IsRxEmpty(GR_SPI_Handler *handle)
{
	if (!handle) {
		return true;
	}

	return GR_MsgBuffer_IsEmpty(handle->rx_buffer);
}

uint32_t GR_SPI_Get_RxMsgSize(GR_SPI_Handler *handle)
{
	if (!handle || GR_MsgBuffer_IsEmpty(handle->rx_buffer)) {
		return 0;
	}

	return GR_MsgBuffer_PeekMsgSize(handle->rx_buffer);
}

GR_SPI_ERR GR_SPI_Get_ErrorStatus(GR_SPI_Handler *handle)
{
	if (!handle) {
		return GR_SPI_ERR_BAD_ARGS;
	}

	return handle->error_status;
}
