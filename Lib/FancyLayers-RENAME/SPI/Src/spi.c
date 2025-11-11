// Wonderful SPI Abstraction Layer courtesy of Bailey
#include "spi.h"

void GR_SPI_Initialize(SPI_Handler* handle, LL_SPI_InitTypeDef* config, GR_SPI_Pins* pins) {
    //Enable GPIO and SPI clocks
    GR_SPI_Enable_Clocks(pins);
    //Configure GPIOs for alternate function and pin mode, pull-up/pull-down, speed, and output type
    LL_GPIO_SetPinMode(pins.CIPO_Port, pins.CIPO_Pin, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(pins.COPI_Port, pins.COPI_Pin, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(pins.SCLK_Port, pins.SCLK_Pin, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(pins.CS_Port, pins.CS_Pin, LL_GPIO_MODE_ALTERNATE);
    
    //Configure SPI protocol with config values
    LL_SPI_Init(handle->spi, config);
    //Enable SPI peripheral
    LL_SPI_Enable(handle->spi);
    //Enable interrupts in NVIC
    uint32_t SPI_IRQn = GR_SPI_Get_IRQn(pins->SPIx_Number);
    NVIC_SetPriority(SPI_IRQn, 1);
    NVIC_EnableIRQ(SPI_IRQn);
    //Enable interrupts at peripheral level
    LL_SPI_EnableIT_ERR(handle->spi); // Error interrupt
    LL_SPI_EnableIT_RXNE(handle->spi); // Not empty Rx buffer
    // Consider enabling the empty Tx buffer interrupt (TXE)
}

void GR_SPI_Interrupt_Handler(void) {
    //STUB
}

uint32_t GR_SPI_Get_IRQn(uint32_t SPIx_number) {
    return 0; //STUB
}

void GR_SPI_Enable_Clocks(GR_SPI_Pins* pins) {
    //STUB
}