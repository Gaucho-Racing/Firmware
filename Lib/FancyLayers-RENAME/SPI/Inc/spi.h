#ifndef SPI_H
#define SPI_H

#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "msgBuffer.h"

#define GR_SPI_UNKNOWN_IRQN -64
#define GR_SPI_UNKNOWN_CLOCK 0
#define GR_SPI_BUFFER_BYTE_CAPACITY 256
#define GR_SPI_MAX_MSG_BYTE_SIZE 64

// Generic type
typedef struct {
	uint8_t *data; // byte array
	uint16_t size; // size of byte array
} GR_SPI_Message;

typedef struct {
	SPI_TypeDef *SPIx;    // Pointer to SPI register wrapper (e.g. SPI1, SPI2, SPI3 macro defines)
	// Pointers to GPIO port register wrapper (e.g. GPIOA, GPIOB, GPIOC, etc. macro defines)
	GPIO_TypeDef *COPI_port;  // Controller Out Peripheral In (same as MOSI)
	GPIO_TypeDef *CIPO_port;  // Controller In Peripheral Out (same as MISO)
	GPIO_TypeDef *SCLK_port;  // Serial (SPI) Clock
	GPIO_TypeDef *NCS_port;	  // Negative Chip Select (same as NSS) --> active low
	// SPI pin numbers (e.g. LL_GPIO_PIN_0, LL_GPIO_PIN_1, LL_GPIO_PIN_2 macro defines)
	uint32_t COPI_pin;
	uint32_t CIPO_pin;
	uint32_t SCLK_pin;
	uint32_t NCS_pin;
	// Refer to the datasheet for the correct number (based on SPIx)
	uint32_t AFN; // Alternate Function Number
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
 * @param msg
 * @return 1 if successful and 0 if failed (i.e. not enough space or invalid args)
 */
bool GR_SPI_Send(GR_SPI_Handler *handle, GR_SPI_Message *msg);

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

/**
 * @brief Returns the size of a message on the top of the rx circular buffer
 * @param handle
 * @return 0 if rx buffer is empty and nonzero for msg size
 */
uint32_t GR_SPI_Get_RxMsgSize(GR_SPI_Handler *handle);

// ============================= Helper Functions =============================

/**
 * @brief Returns the interrupt request number for a given SPIx peripheral
 * @param SPIx
 * @return GR_SPI_UNKNOWN_IRQN or a valid IRQ number for SPIx
 */
int GR_SPI_Get_IRQn(SPI_TypeDef *SPIx);

/**
 * @brief Returns the clock mask for a given GPIOx port
 * @param GPIOx
 * @return GR_SPI_UNKNOWN_CLOCK or a valid bit flag that corresponds to the clock for the GPIOx port
 */
uint32_t GR_SPI_Get_GPIO_Clock(GPIO_TypeDef *GPIOx);

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
