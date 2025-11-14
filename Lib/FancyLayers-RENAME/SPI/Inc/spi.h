#include "circularBuffer.h"

#ifndef SPI_H
#define SPI_H

#define GR_SPI_UNKNOWN_IRQN -64

// Generic type
typedef struct {
    //LL structs
    SPI_TypeDef* SPIx; // Pointer to SPI register wrapper (e.g. SPI1, SPI2, SPI3 macro defines)
    //COPI, CIPO, SCLK, CS
    GPIO_TypeDef* GPIOx[4]; // Pointer to GPIO port register wrapper (e.g. GPIOA, GPIOB, GPIOC, etc. macro defines)
    //COPI, CIPO, SCLK, CS
    uint32_t pins[4]; // SPI pin numbers (e.g. LL_GPIO_PIN_0, LL_GPIO_PIN_1, LL_GPIO_PIN_2 macro defines)
    uint32_t num_pins = 4;
    //GR structs
    CircularBuffer* buffer;
    //GPIO pin data
    SPI_message cur_message;
    volatile uint16_t tx_index;
    volatile uint16_t rx_index;
    volatile uint8_t ongoing;
} GR_SPI_Handler;

// Generic type
typedef struct {
    uint8_t *tx_data;
    uint16_t size;
} SPI_Message;

// ============================= Handler Functions =============================

/**
 * @brief Initializes SPI with config values
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Initialize(GR_SPI_Handler* handle, LL_SPI_InitTypeDef* config, uint32_t alternate_function_num);

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
void GR_SPI_Interrupt_Handler(void);

//Map SPI1-3 IRQHandlers to custom interrupt handler
void SPI1_IRQHandler(void) { GR_SPI_Interrupt_Handler(); }
void SPI2_IRQHandler(void) { GR_SPI_Interrupt_Handler(); }
void SPI3_IRQHandler(void) { GR_SPI_Interrupt_Handler(); }

// ============================= Tx/Rx =============================

/**
 * @brief Send data through SPI
 * 
 * @param handler
 * @param data
 */
void GR_SPI_Send(GR_SPI_Handler* handler, SPI_Message data);

/**
 * @brief Read from the SPI buffer; primarily intended for polling. Rely on the passed in handler when setup() is called 
 * 
 * @param handler
 * @return SPI_Message 
 */

SPI_Message GR_SPI_Receive(GR_SPI_Handler* handler);

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
void GR_SPI_Configure_Pins(GR_SPI_Handler* handle, LL_GPIO_InitTypeDef* pin_config, uint32_t alternate_function_num);

#endif