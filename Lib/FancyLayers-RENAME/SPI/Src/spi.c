// Wonderful SPI Abstraction Layer courtesy of Bailey
#include "spi.h"
#define spi_status 0
#define spi_ongoing -1

void GR_SPI_Initialize(GR_SPI_Handler* handle, LL_SPI_InitTypeDef* config, uint32_t alternate_function_num) {    
    //Create Circular Buffers
    CircularBuffer* circular_buffer_ptr;
    circular_buffer_ptr = GR_CircularBuffer_Create(GR_SPI_BUFFER_MESSAGE_CAPACITY);
    if(circular_buffer_ptr == NULL) {
        //Attempt to Create Rx Buffer Error
    } else {
        handle->rx_buffer = circular_buffer_ptr;
    }
    circular_buffer_ptr = GR_CircularBuffer_Create(GR_SPI_BUFFER_MESSAGE_CAPACITY);
    if(circular_buffer_ptr == NULL) {
        //Attempt to Create Tx Buffer Error
    } else {
        handle->tx_buffer = circular_buffer_ptr;
    }
    
    //Enable GPIO and SPI clocks
    GR_SPI_Enable_Clocks(pins);

    //Configure GPIOs
    LL_GPIO_InitTypeDef pin_config;
    GR_SPI_Configure_Pins(handle, &pin_config, pins);

    //Configure SPI protocol with config values
    LL_SPI_Init(handle->spi, config);

    //Enable SPI peripheral
    LL_SPI_Enable(handle->spi);

    //Enable interrupts in NVIC
    uint32_t SPI_IRQn = GR_SPI_Get_IRQn(pins->SPIx);
    if(SPI_IRQn != GR_SPI_UNKNOWN_IRQN) {
        NVIC_SetPriority(SPI_IRQn, 1);
        NVIC_EnableIRQ(SPI_IRQn);
    } else {
        //Throw an error
    }
    
    //Enable interrupts at peripheral level
    LL_SPI_EnableIT_ERR(handle->spi); // Error interrupt
    LL_SPI_EnableIT_RXNE(handle->spi); // Not empty Rx buffer
    LL_SPI_EnableIT_TXE(handle->spi); // Empty Tx buffer
}

void GR_SPI_Interrupt_Handler(void) {
    //STUB
}

uint32_t GR_SPI_Get_IRQn(SPI_TypeDef* SPIx) {
    switch(SPIx) {
        case SPI1: return SPI1_IRQn; //35
        case SPI2: return SPI2_IRQn; //36
        case SPI3: return SPI3_IRQn; //51
        default: return GR_SPI_UNKNOWN_IRQN;
    }
}

void GR_SPI_Enable_Clocks(GR_SPI_Handler* handle) {
    //Enable GPIO clock
    uint32_t GPIOx_Port
    for(int i = 0; i < handle->num_pins; i++) {
        switch(handle->GPIOx[i]) {
            case GPIOA:
                GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOA;
                break;
            case GPIOB:
                GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOB;
                break;
            case GPIOC:
                GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOC;
                break;
            case GPIOD:
                GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOD;
                break;
            case GPIOE:
                GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOE;
                break;
            case GPIOF:
                GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOF;
                break;
            case GPIOG:
                GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOG;
                break;
            case GPIOH:
                GPIOx_Port = LL_AHB2_GRP1_PERIPH_GPIOH;
                break;
            default: //Do nothing (unknown GPIOx)
        }
        LL_AHB2_GRP1_EnableClock(GPIOx_Port);
    }
    
    //Enable SPI clock
    switch(handle->SPIx) {
        case SPI1:
            LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
            break;
        case SPI2:
            LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
            break;
        case SPI3:
            LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
            break;
        default: //Do nothing (unknown SPIx)
    }
}

void GR_SPI_Send(GR_SPI_Handler* handler, SPI_Message data) {
    if(handler->ongoing){
        return spi_ongoing; 
    }
    handler->ongoing = 1;
    handler->tx_index = 0;
    handler->rx_index = 0;
    handler->cur_msg = *msg;

    if (handler->CS_Port != NULL) {
        LL_GPIO_ResetOutputPin(handler->GPI0x[3], handler->pins[3]);
    }

    while (LL_SPI_IsActiveFlag_RXNE(handler->SPI1)) {
        if (LL_SPI_GetDataWidth(handler->SPI1) == LL_SPI_DATAWIDTH_16BIT) {
            (void)LL_SPI_ReceiveData16(handler->SPI1);
        } else {
            (void)LL_SPI_ReceiveData8(handler->SPI1);
        }
    }

    LL_SPI_EnableIT_TXE(handler->SPI1);
    LL_SPI_EnableIT_RXNE(handler->SPI1);
    return spi_status;
}

void GR_SPI_Configure_Pins(GR_SPI_Handler* handle, LL_GPIO_InitTypeDef* pin_config, uint32_t alternate_function_num) {
    LL_GPIO_StructInit(pin_config); // Default config values
    pin_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH; // Very high output speed
    pin_config.Pull = LL_GPIO_PULL_NO; //No pull-up or pull-down resistance
    pin_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL; // Push-pull output (not open-drain)
    pin_config.Mode = LL_GPIO_MODE_ALTERNATE; // Alternate pin function mode
    pin_config.Alternate = alternate_function_num; // Alternate function number
    for(int i = 0; i < handle->num_pins; i++) {
        pin_config.Pin = handle->pins[i];
        LL_GPIO_Init(handle->pins[i], &pin_config);
    }
}

SPI_Message GR_SPI_Receive(GR_SPI_Handler* handler){
    
}