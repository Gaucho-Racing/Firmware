// Wonderful SPI Abstraction Layer courtesy of Bailey
#include "spi.h"

void GR_SPI_Initialize(SPI_Handler* handle, LL_SPI_InitTypeDef* config, GR_SPI_Pins* pins) {
    //Enable GPIO and SPI clocks
    GR_SPI_Enable_Clocks(pins);
    //Configure GPIOs for alternate function and pin mode, pull-up/pull-down, speed, and output type
    LL_GPIO_InitTypeDef pin_config;
    LL_GPIO_StructInit(&pin_config);
    pin_config.Alternate = pins->alternate_function_number;
    pin_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    pin_config.Mode = LL_GPIO_MODE_ALTERNATE;
    for(int i = 0; i < pins->num_pins; i++) {
        pin_config.Pin = pins->pin_nums[i];
        LL_GPIO_Init(handle->pins[i], &pin_config);
    }

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