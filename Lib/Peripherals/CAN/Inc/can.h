#include "can_platform_deps.h"
#include "circularBuffer.h"


//Must perform a deep copy of the data
typedef void (*CAN_RXCallback) (void* data, uint32_t size);

typedef struct {
    //can baud rate is set by fdcan prescaler and RCC clock configurations

    FDCAN_GlobalTypeDef *fdcan_instance; //Base address of FDCAN peripheral in memory (FDCAN1, FDCAN2, FDCAN3 macros)

    FDCAN_InitTypeDef hal_fdcan_init; 
    CAN_RXCallback rx_callback; 
    uint32_t rx_interrupt_priority;
    uint32_t tx_interrupt_priority; 

    //Circular Buffer
    uint32_t tx_buffer_length; 

    GPIO_TypeDef *rx_gpio;  //Instance name, like GPIOA, GPIOB, etc.
    GPIO_InitTypeDef init_rx_gpio; //GPIO Parameters - set correct Alternate Function, no pullup/pulldown, high frequency
    GPIO_TypeDef *tx_gpio; 
    GPIO_InitTypeDef init_tx_gpio;

    //additional parameters
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
    bool init; 
    bool started; 

    //NVIC exceptions
} CANHandle;

#define FDCAN_MAX_DATA_BYTES 64
typedef struct {
    FDCAN_TxHeaderTypeDef tx_header;
    uint8_t               data[FDCAN_MAX_DATA_BYTES]; 
} FDCANMessage; 


CANHandle* can_init(CANConfig *config); //user must supply an rx callback function

int can_start(CANHandle*handle);
int can_stop(CANandle*handle);
int can_send(char* buffer, size_t send);  
int can_release(CANHandle* canHandle); //deinit circular buffer and turn off can peripheral and gpios
int can_add_filter(CANHandle* canHandle, HAL_FDCAN_FilterTypeDef * filter);
//alternatively use 
//HAL_FDCAN_ConfigGlobalFilter()
//HAL_FDCAN_ConfigFilter()



//tx callback should free memory of sending buffer
//block until enqueued on circular buffer
//void can_();  //enqueue onto circular 
//remember to free all data in the circular buffer



