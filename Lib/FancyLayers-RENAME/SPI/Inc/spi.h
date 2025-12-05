#include "circularBuffer.h"

#ifndef SPI_H
#define SPI_H

#define GR_SPI_UNKNOWN_IRQN -64
#define GR_SPI_BUFFER_MESSAGE_CAPACITY 16

static GR_SPI_Handler* GR_SPI_HANDLER_LUT[3]; //Stores pointer to the handler structs for SPI1 (0), SPI2 (1), & SPI3 (2)

typedef struct {
    SPI_TypeDef* SPIx; // Pointer to SPI register wrapper (e.g. SPI1, SPI2, SPI3 macro defines)
    //COPI, CIPO, SCLK, CS
    GPIO_TypeDef* GPIOx[4]; // Pointer to GPIO port register wrapper (e.g. GPIOA, GPIOB, GPIOC, etc. macro defines)
    //COPI, CIPO, SCLK, CS
    uint32_t pin_nums[4]; // SPI pin numbers (e.g. LL_GPIO_PIN_0, LL_GPIO_PIN_1, LL_GPIO_PIN_2 macro defines)
    uint32_t num_pins = 4;
    uint32_t alternate_function_number;
} GR_SPI_Pins;

// Generic type
typedef struct {
    // Contains all configuration information
    LL_SPI_InitTypeDef* spi_config;
    GR_SPI_Pins* pins;
    //GR structs
    CircularBuffer* rx_buffer;
    CircularBuffer* tx_buffer;
    //Tx-Rx parameters
    uint8_t transfer_size;
    //Tx-Rx current messages
    SPI_Message* current_msg;
    volatile uint16_t current_tx_msg_index, current_rx_msg_index;
    volatile uint8_t msg_status;
} GR_SPI_Handler;

// Generic type
typedef struct {
    uint8_t* data; //byte array
    uint16_t size;
} GR_SPI_Message;

// ============================= Handler Functions =============================

/**
 * @brief Initializes SPI with config values and alternate function number. Creates circular buffer structs.
 * 
 * @param handle
 * @param config
 * @param alternate_function_num
 * @return 
 */
void GR_SPI_Initialize(GR_SPI_Handler* handle, LL_SPI_InitTypeDef* config, GR_SPI_Pins* pin_config);

/**
 * @brief Tear down the SPI handler
 * 
 * @param handler
 */
void GR_SPI_Close(GR_SPI_Handler* handler);

/**
 * @brief Handles SPI interrupts
 * 
 * @param 
 * @return 
 */
void GR_SPI_Interrupt_Handler(GR_SPI_Handler* handle);

//Map SPI1-3 IRQHandlers to custom interrupt handler
void SPI1_IRQHandler(void) { GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[0]); }
void SPI2_IRQHandler(void) { GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[1]); }
void SPI3_IRQHandler(void) { GR_SPI_Interrupt_Handler(GR_SPI_HANDLER_LUT[2]); }

// ============================= Tx/Rx =============================

/**
 * @brief Send data through SPI
 * 
 * @param handle
 * @param data
 */
void GR_SPI_Send(GR_SPI_Handler* handle, GR_SPI_Message* data);

/**
 * @brief Read from the SPI buffer; primarily intended for polling. Rely on the passed in handler when setup() is called 
 * 
 * @param handle
 * @return SPI_Message 
 */

GR_SPI_Message* GR_SPI_Receive(GR_SPI_Handler* handle);

/**
 * @brief Checks if the Rx circular buffer is empty
 * 
 * @param handle
 */
bool GR_SPI_Is_RXE(GR_SPI_Handler* handle) { return GR_CircularBuffer_IsEmpty(handle->rx_buffer); }

// ============================= Helper Functions =============================

/**
 * @brief Returns the interrupt request number for a given SPIx peripheral
 * 
 * @param handle
 */
uint32_t GR_SPI_Get_IRQn(SPI_TypeDef* SPIx);

/**
 * @brief Enables the GPIO port clocks and SPI clock needed for the given pins
 * 
 * @param pins
 */
void GR_SPI_Enable_Clocks(GR_SPI_Handler* handle);

/**
 * @brief Configures GPIO pins for SPI
 * 
 * @param config
 * @param pins
 */
void GR_SPI_Configure_Pins(GR_SPI_Handler* handle, LL_GPIO_InitTypeDef* pin_config);

/**
 * @brief Continues sending the next byte(s) within an SPI message
 * 
 * @param config
 * @param pins
 */
void GR_SPI_Transfer_Tx_Bytes(GR_SPI_Handler* handle);

#endif