#include "circularBuffer.h"

#ifndef SPI_H
#define SPI_H

typedef struct {
    uint32_t CIPO_Pin, COPI_Pin, CS_Pin, SCLK_Pin;
    uint32_t CIPO_Port, COPI_Port, CS_Port, SCLK_Port;
    uint32_t Alternate_Function;
    uint32_t SPIx_Number;
} GR_SPI_Pins;

// Generic type
typedef struct {
    //LL structs
    SPI_TypeDef* spi = nullptr;
    uint32_t SPIx_Number;
    //GR structs
    CircularBuffer* buffer;
    //GPIO pin data
    
} GR_SPI_Handler;

// Generic type
typedef struct {

} SPI_Message;

// ============================= Handler Functions =============================

/**
 * @brief Initializes SPI with config values
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Initialize(SPI_Handler* handle, LL_SPI_InitTypeDef* config, GR_SPI_Pins *pins);

/**
 * @brief Tear down the SPI handler
 * 
 * @param handler
 */
void GR_SPI_Close(SPI_Handler* handler);

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
void GR_SPI_Send(SPI_Handler* handler, SPI_Message data);

/**
 * @brief Read from the SPI buffer; primarily intended for polling. Rely on the passed in handler when setup() is called 
 * 
 * @param handler
 * @return SPI_Message 
 */
SPI_Message GR_SPI_Receive(SPI_Handler* handler);

// ============================= Helper Functions =============================

/**
 * @brief Returns the interrupt request number for a given SPIx peripheral
 * 
 * @param handle
 */
uint32_t GR_SPI_Get_IRQn(uint32_t SPIx_number);

/**
 * @brief Enables the GPIO port clocks and SPI clock needed for the given pins
 * 
 * @param pins
 */
void GR_SPI_Enable_Clocks(GR_SPI_Pins* pins);

#endif