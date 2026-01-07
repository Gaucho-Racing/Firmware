#include "can_platform_deps.h"
#include "circularBuffer.h"

typedef void (*CAN_RXCallback) (void*, uint32_t size);
typedef struct {
    //can baud rate is set by fdcan prescaler and RCC clock configurations

    FDCAN_GlobalTypeDef *fdcan_instance; //Base address of FDCAN peripheral in memory (FDCAN1, FDCAN2, FDCAN3 macros)

    FDCAN_InitTypeDef hal_fdcan_init; 
    CAN_RXCallback rx_callback; 
    uint32_t rx_interrupt_priority;

    //Circular Buffer
    uint32_t tx_buffer_length; 

    GPIO_TypeDef *rx_gpio;  //Instance name, like GPIOA, GPIOB, etc.
    GPIO_InitTypeDef init_rx_gpio; //GPIO Parameters - set correct Alternate Function, no pullup/pulldown, high frequency
    GPIO_TypeDef *tx_gpio; 
    GPIO_InitTypeDef init_tx_gpio;

    //additional parameters????

} CANConfig;



//FDCAN peripheral for STM32G4
typedef struct {
    FDCAN_HandleTypeDef *hal_fdcanP; 
    CircularBuffer* tx_buffer; 
    uint32_t tx_buffer_length; 

    CAN_RXCallback rx_callback;
    
    //for release
    GPIO_TypeDef * rx_gpio; 
    GPIO_TypeDef * tx_gpio; 

    //Stopped state, will HAL throw an error if trying to user tries to add to queue when FDCAN is stopped?

    //NVIC exceptions
} CANHandle

/*
FDCAN_FilterTypeDef
FDCAN_FilterTypeDef is defined in the stm32g4xx_hal_fdcan.h
Data Fields
• uint32_t IdType
• uint32_t FilterIndex
• uint32_t FilterType
• uint32_t FilterConfig
• uint32_t FilterID1
• uint32_t FilterID2
*/

CANHandle* can_init(CANConfig *config); //user must supply an rx callback function

//tx callback should free memory of sending buffer
//block until enqueued on circular buffer
//void can_();  //enqueue onto circular 
void can_enqueue(CANHandle* canHandle, char * message,);     //engueue onto circular buffer
//remember to free all data in the circular buffer
void can_release(CANHandle* canHandle); //deinit circular buffer and turn off can peripheral and gpios

void can_add_filter(HAL_FDCAN_FilterTypeDef * filter);


