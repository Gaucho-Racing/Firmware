// Wonderful SPI Abstraction Layer courtesy of Bailey
#include "spi.h"
#define spi_status 0
#define spi_ongoing -1

void GR_SPI_Initialize(GR_SPI_Handler* handle, LL_SPI_InitTypeDef* config, GR_SPI_Pins* pin_config, uint32_t* rx_NE_flag) {    
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

    //Deep copy of pins struct
    handle->pins = (GR_SPI_Pins *)malloc(sizeof(GR_SPI_Pins)); //Make memory for GR_SPI_Pins struct
    handle->pins->pin_nums = (uint32_t *)malloc(pin_config->num_pins * sizeof(uint32_t)); // Make memory for pin_nums[num_pins]
    handle->pins->GPIOx = (GPIO_TypeDef *)malloc(pin_config->num_pins * sizeof(GPIO_TypeDef)); //Make memory for GPIOx[num_pins]
    for (int i = 0; i < pin_config->num_pins; i++) {
        handle->pins->pin_nums[i] = pin_config->pin_nums[i];
        handle->pins->GPIOx[i] = pin_config->GPIOx[i];
    }
    handle->pins->SPIx = pin_config->SPIx;
    handle->pins->num_pins = pin_config->num_pins;
    handle->pins->alternate_function_number = pin_config->alternate_function_number;
    
    //Copy rx_NE_flag
    handle->rx_NE_flag = rx_NE_flag;

    // Store handler in lookup table for interrupts
    switch(SPIx) {
        case SPI1: {
            GR_SPI_HANDLER_LUT[0] = handler;
            break;
        }
        case SPI1: {
            GR_SPI_HANDLER_LUT[1] = handler;
            break;
        }
        case SPI1: {
            GR_SPI_HANDLER_LUT[2] = handler;
            break;
        }
    }
    
    //Enable GPIO and SPI clocks
    GR_SPI_Enable_Clocks(pins);

    //Configure GPIOs
    LL_GPIO_InitTypeDef pin_config;
    GR_SPI_Configure_Pins(handle, &pin_config, pins);

    //Configure SPI protocol with config values
    LL_SPI_Init(handle->spi, config);
    //Transaction size is 8-bits
    if(config->DataWidth <= 8) handle->tx_rx_size = 0;
    //Transaction size is 16-bits
    else tx_rx_size = 1;

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

void GR_SPI_Interrupt_Handler(GR_SPI_Handler* handle) {
    //Check if valid handle
    if(handle == NULL) {
        //Throw an error
        return;
    }

    
    //Check if called by error interrupt
    //Frame format error
    if(LL_SPI_IsActiveFlag_FRE(handle->pins->SPIx)) {
        //Log an error
        return;
    }
    //Overrun error
    else if(LL_SPI_IsActiveFlag_OVR(handle->pins->SPIx)) {
        //Log an error
        return;
    }
    //Fault mode error
    else if(LL_SPI_IsActiveFlag_MODF(handle->pins->SPIx)) {
        //Log an error
        return;
    }
    //CRC error
    else if(LL_SPI_IsActiveFlag_CRCERR(handle->pins->SPIx)) {
        //Log an error
        return;
    }
    
    // No errors detected...

    //Check if Rx not empty
    if(LL_SPI_IsActiveFlag_RXNE(handle->pins->SPIx)) {
        GR_CircularBuffer_Push(handle->rx_buffer, LL_SPI_ReceiveData8());
    }
    //Check if Tx is empty
    if(LL_SPI_IsActiveFlag_TXE(handle->pins->SPIx)) {
        //Grab a message off of the send buffer if there is one
        if(GR_CircularBuffer_Peek(handle->tx_buffer) != NULL) {
            if(tx_rx_size == 0) {
                uint8_t* data = GR_CircularBuffer_Pop(handle->tx_buffer);
                LL_SPI_TransmitData8(handle->pins->SPIx, data);
            } else {
                uint16_t* data = GR_CircularBuffer_Pop(handle->tx_buffer);
                LL_SPI_TransmitData16(handle->pins->SPIx, data);
            }
        }
    }
}

//SPIx_IRQn is defined in stm32 libraries
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

void GR_SPI_Send(GR_SPI_Handler* handle, SPI_Message data) {
    if(handler->ongoing){
        return spi_ongoing; 
    }
    handle->ongoing = 1;
    handle->tx_index = 0;
    handle->rx_index = 0;
    handle->cur_msg = data;

    if (handle->CS_Port != NULL) {
        LL_GPIO_ResetOutputPin(handle->GPI0x[3], handle->pins[3]);
    }

    while (LL_SPI_IsActiveFlag_RXNE(handle->SPIx)) {
        if (LL_SPI_GetDataWidth(handle->SPIx) == LL_SPI_DATAWIDTH_16BIT) {
            (void)LL_SPI_ReceiveData16(handle->SPIx);
        } else {
            (void)LL_SPI_ReceiveData8(handle->SPIx);
        }
    }

    LL_SPI_EnableIT_TXE(handle->SPIx);
    LL_SPI_EnableIT_RXNE(handle->SPIx);
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