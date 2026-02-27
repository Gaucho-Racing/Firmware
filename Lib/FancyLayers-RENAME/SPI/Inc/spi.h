#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#include "circularBuffer.h"
#include "main.h"
#include "msgBuffer.h"

#define GR_SPI_UNKNOWN_IRQN -64
#define GR_SPI_BUFFER_MESSAGE_CAPACITY 16

// Generic type
typedef struct {
	uint8_t *data; // byte array
	uint16_t size; // size of byte array
} GR_SPI_Message;

typedef struct {
	SPI_TypeDef *SPIx;    // Pointer to SPI register wrapper (e.g. SPI1, SPI2, SPI3 macro defines)
	GPIO_TypeDef **GPIOx; // Pointer to GPIO port register wrapper (e.g. GPIOA, GPIOB, GPIOC, etc. macro defines)
			      // COPI, CIPO, SCLK, CS
	uint32_t *pin_nums;		    // SPI pin numbers (e.g. LL_GPIO_PIN_0, LL_GPIO_PIN_1, LL_GPIO_PIN_2 macro defines)
	uint32_t num_pins;		    // Number of SPI pins
	uint32_t alternate_function_number; // Refer to the datasheet for the correct number (based on SPIx)
} GR_SPI_Pins;

typedef struct GR_SPI_Handler_struct {
	// Contains all configuration information
	LL_SPI_InitTypeDef *spi_config;
	GR_SPI_Pins *pins;
	// GR structs
	GR_MsgBuffer *rx_buffer;
	GR_MsgBuffer *tx_buffer;
	// Tx-Rx parameters
	uint8_t transfer_size;
	// Tx-Rx current messages
	GR_SPI_Message *current_msg;
	volatile uint16_t current_tx_msg_index, current_rx_msg_index;
	volatile uint8_t msg_status;
	volatile int8_t error_status;
} GR_SPI_Handler;

// ============================= handle Functions =============================

/**
 * @brief Initializes SPI with config values and alternate function number.
 * Creates circular buffer structs.
 *
 * @param handle
 * @param config
 * @param pin_config
 * @return
 */
void GR_SPI_Initialize(GR_SPI_Handler *handle, LL_SPI_InitTypeDef *config, GR_SPI_Pins *pin_config);

/**
 * @brief Tear down the SPI handle and frees all relevant memory
 * @param handle
 */
void GR_SPI_Close(GR_SPI_Handler *handle);

/**
 * @brief Deallocate the memory in a GR_SPI_Message
 * @param msg
 */
void GR_SPI_Msg_Free(GR_SPI_Message *msg);

/**
 * @brief Handles SPI interrupts
 * @param handle
 */
void GR_SPI_Interrupt_Handler(GR_SPI_Handler *handle);

// Map SPI1-3 IRQHandlers to custom interrupt handle

/**
 * @brief Interrupt Request Handler for SPI1; defers to GR_SPI_Interrupt_Handler
 */
void SPI1_IRQHandler(void);
/**
 * @brief Interrupt Request Handler for SPI2; defers to GR_SPI_Interrupt_Handler
 */
void SPI2_IRQHandler(void);
/**
 * @brief Interrupt Request Handler for SPI3; defers to GR_SPI_Interrupt_Handler
 */
void SPI3_IRQHandler(void);

// ============================= Tx/Rx =============================

/**
 * @brief The SPI message is sent via SPI and will likely interrupt the program immediately to finish the transaction. Msg is untouched and should be reused in GR_SPI_Receive or freed.
 * @param handle
 * @param data
 */
void GR_SPI_Send(GR_SPI_Handler *handle, GR_SPI_Message *msg);

/**
 * @brief The dest_msg will be destroyed and overwritten if there is a message to receive; otherwise, nothing happens.
 * @param handle
 * @param dest_msg
 */
void GR_SPI_Receive(GR_SPI_Handler *handle, GR_SPI_Message *dest_msg);

/**
 * @brief Returns whether a message was received and is waiting in the Rx buffer
 * @param handle
 * @return 1 if Rx is empty, 0 if Rx is not empty
 */
bool GR_SPI_IsRxEmpty(GR_SPI_Handler *handle);

// ============================= Helper Functions =============================

/**
 * @brief Returns the interrupt request number for a given SPIx peripheral
 * @param SPIx
 * @return IRQ Number for SPIx
 */
uint32_t GR_SPI_Get_IRQn(SPI_TypeDef *SPIx);

/**
 * @brief Enables the GPIO port clocks and SPI clock needed for the given pins
 * @param handle
 */
void GR_SPI_Enable_Clocks(GR_SPI_Handler *handle);

/**
 * @brief Configures GPIO pins for SPI
 * @param handle
 * @param pin_config
 */
void GR_SPI_Configure_Pins(GR_SPI_Handler *handle, LL_GPIO_InitTypeDef *pin_config);

/**
 * @brief Continues sending the next byte(s) within an SPI message
 * @param handle
 */
void GR_SPI_Transfer_Tx_Bytes(GR_SPI_Handler *handle);

/**
 * @brief Pops off the next Tx message and initiates its transaction
 * @param handle
 */
void GR_SPI_Begin_New_Tx(GR_SPI_Handler *handle);

#endif // SPI_H
