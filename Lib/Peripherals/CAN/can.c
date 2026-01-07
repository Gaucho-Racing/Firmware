#include "can.h"
#include "Logomatic.h"
#include <stdint.h>

//#ifdef STM32G474xx

//HAL handles
FDCAN_HandleTypeDef hal_fdcan1 = {.Instance = FDCAN1}; 
FDCAN_HandleTypeDef hal_fdcan2 = {.Instance = FDCAN2}; 
FDCAN_HandleTypeDef hal_fdcan3 = {.Instance = FDCAN3}; 

//Handle for circular buffer + HAL fdcan handle
CANHandle CAN1 = {.hal_fdcanP = &hal_fdcan1};
CANHandle CAN2 = {.hal_fdcanP = &hal_fdcan2};
CANHandle CAN3 = {.hal_fdcanP = &hal_fdcan3}; 

#define _GPIOx_CLK_ENABLE(GPIOX)                                        \
        do {                                                            \
        switch ((uintptr_t)(GPIOX)) {                                   \
            case (uintptr_t)GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break; \
            case (uintptr_t)GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break; \
            case (uintptr_t)GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break; \
            case (uintptr_t)GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break; \
            default: LOGOMATIC("CAN: Invalid GPIO Pins"); break; /* unknown port, do nothing */              \
        }  \
        } while(0)
//#endif

#define NVIC_ACTIVATE(FDCAN1)
        #
        HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

CANHandle* can_init(const CANConfig *config) { 
    //config validation?
    //assert(config != 0)

    //#ifdef STM32G474xx
    CANHandle * canHandle = 0;
    if (config->fdcan_instance == FDCAN1 && ) {
        if (CAN1->tx_buffer) {LOGOMATIC("CAN: FDCAN1 is already initialized"); return 0;}
        else canHandle = &CAN1; 
    } else if (config->fdcan_instance == FDCAN2 && CAN2->tx_buffer) {
        if (CAN2->tx_buffer) {LOGOMATIC("CAN: FDCAN2 is already initialized"); return 0;}
        else canHandle = &CAN2; 
    } else if (config->fdcan_instance == FDCAN3 && CAN3->tx_buffer) {
        if (CAN3->tx_buffer) {LOGOMATIC("CAN: FDCAN3 is already initialized"); return 0;}
        else canHandle = &CAN3; 
    }
    //#elif defined(STM32L476xx)
    //#else 
    //#error "Unsupported STM32 family"
    //#endif
    else {
        LOGOMATIC("CAN: Unrecognized FDCAN instance");
        return 0; 
    }

    (canHandle->hal_fdcanP).Init = config->hal_fdcan_init; //copy FDCAN parameters from user
    canHandle->rx_gpio = config->rx_gpio; 
    canHandle->tx_gpio = config->tx_gpio;   
    canHandle->rx_callback = config->rx_callback; 
    canHandle->tx_buffer_length = config->tx_buffer_length; 

    //FDCAN Clock Init
    __HAL_RCC_FDCAN_CLK_ENABLE();
    //Clock speed for FDCAN determined by APB1 clock speed and FDCAN prescaler

    //GPIOs init
    HAL_GPIO_Init(config->rx_gpio, &(config->init_rx_gpio) );
    HAL_GPIO_Init(config->tx_gpio, &(config->init_tx_gpio) );

    if (HAL_FDCAN_Init( config->hal_fdcanP ) != HAL_OK)
    {
        LOGOMATIC("CAN: HAL Could not initialize FDCAN peripheral");
        can_release(canHandle); 
        return 0; 
        //Error_Handler();
    }

    _GPIOx_CLK_ENABLE(config->rx_gpio); 
    _GPIOx_CLK_ENABLE(config->tx_gpio); 

    //FDCAN peripheral init
    if (HAL_FDCAN_Init( canHandle->hal_fdcanP ) != HAL_OK)
    {
        LOGOMATIC("CAN: HAL Could not initialize FDCAN peripheral");
        can_release(canHandle); 
        return 0; 
        //Error_Handler();
    }

    //Configure interrupts
    //uint32_t rxevents = FDCAN_IT_RX_FIFO0_NEW_MESSAGE;
    uint32 status = 0; 
    status |= HAL_FDCAN_ActivateNotification( canHandle->hal_fdcanP, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    status |= HAL_FDCAN_ConfigInterruptLines( canHandle->hal_fdcanP, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_INTERRUPT_0);

    //uint32_t txevents = FDCAN_IT_TX_COMPLETE;
    uint32_t destinations = FDCAN_TX_BUFFER0; 
  
    //not sure if need to be on separate interrupt lines or if HAL takes care of it
    status |= HAL_FDCAN_ActivateNotification( canHandle->hal_fdcanP, FDCAN_IT_TX_COMPLETE, destinations);
    status |= HAL_FDCAN_ConfigInterruptLines( canHandle->hal_fdcanP, FDCAN_IT_TX_COMPLETE, FDCAN_INTERRUPT_1);
    //Callbacks redefined later
    //HAL_FDCAN_TxBufferCompleteCallback
    //HAL_FDCAN_RxFifo0Callback

    //Enable Interrupts 
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

    if (status & HAL_ERROR) {
        LOGOMATIC("CAN: Could not activate rx and tx interrupts"); 
        can_release(canHandle); 
        return 0; 
    }

    //Circular Buffer
    canHandle->tx_buffer = GR_CircularBuffer_Create(config->tx_buffer_length); 
    if (!canHandle->tx_buffer ) {
        LOGOMATIC("CAN: Could not allocate circular buffer"); 
        return 0; 
    }

    return canHandle; 
}

HAL_FDCAN_TxBufferCompleteCallback( canHandle->hal_fdcanP, ) {

}
HAL_FDCAN_RxFifo0Callback() {
    
}


CANHandle can_add_filter() {
    //HAL_FDCAN_ConfigFilter()
}

int can_start(CANHandle * canHandle) {
    //Start the HAL function
    //HAL_FDCAN_Start
}

int can_stop(CANHandle * canHandle) {
    //Add stopped state?

    //HAL_FDCAN_Stop
}

int can_release(CANHandle * canHandle) {
    if (canHandle == 0) {
        LOGOMATIC("CAN: Tried to release a null handle"); 
        return -1; 

    //free circular buffer contents
    //turn off gpio clocks
    
    //clear filters
    //HAL_FDCAN_DeInit(); 

    //turn off fdcan clock if no other instances are remaining
    //__HAL_RCC_FDCAN_CLK_ENABLE();

    memset(canHandle, 0, sizeof(canHandle));

    return 0; 
}

//tx callback should free memory of sending buffer
//block until enqueued on circular buffer
void can_send() {
    //
}


//not needed, provide an rx callback instead
//void can_receive(); 

//HAL_FDCAN_Start
//HAL_FDCAN_Stop
//HAL_AddMessageToTXFifoQ

//HAL_FDCAN_GetProtocolStatus
//HAL_FDCAN_GetErrorCounters

//HAL_FDCAN_IsTXBufferMessagePending
//HAL_FDCAN_AbortTXRequest
//HAL_FDCAN_GetRXMessage
//HAL_FDCAN_GetTxEvent

//HAL_FDCAN_GetRxFifoFillLevel
//HAL_FDCAN_GetTxFifoFreeLevel

//HAL_FDCAN_ConfigFilter

//HAL_FDCAN_ConfigRXFifoOverwrite


//HAL_FDCAN_Start
//HAL_FDCAN_Stop

//HAL_FDCAN_TxEventFifoCallback

//HAL_FDCAN_GetState

