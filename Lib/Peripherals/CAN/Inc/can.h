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
    GPIO_InitTypeDef init_rx_gpio; //GPIO Parameters - set correct Alternate Function, no pullup/pulldown, high/very_high frequency
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
    uint32_t Clock_Source;

    //state
    bool init; 
    bool started; 


    //error states
} CANHandle;

#define FDCAN_MAX_DATA_BYTES 64
typedef struct {
    FDCAN_TxHeaderTypeDef tx_header;
    uint8_t               data[FDCAN_MAX_DATA_BYTES]; 
} FDCANTxMessage; 
typedef struct {
    FDCAN_RxHeaderTypeDef rx_header;
    uint8_t               data[FDCAN_MAX_DATA_BYTES]; 
} FDCANRxMessage; 


CANHandle* can_init(const CANConfig *config); //user must supply an rx callback function
int can_start(CANHandle*handle);
int can_stop(CANHandle*handle);
int can_send(CANHandle*handle, FDCANTxMessage* buffer);  
int can_release(CANHandle* handle); //deinit circular buffer and turn off can peripheral and gpios
int can_add_filter(CANHandle* handle, FDCAN_FilterTypeDef * filter);
//alternatively use 
//HAL_FDCAN_ConfigGlobalFilter()
//HAL_FDCAN_ConfigFilter()

//doesn't need a handle, independent of any CAN instance
int can_set_clksource(uint32_t clksource); //LL_RCC_FDCAN_CLKSOURCE_PCLK1 for STM32G474RE




//tx callback should free memory of sending buffer
//block until enqueued on circular buffer
//void can_();  //enqueue onto circular 
//remember to free all data in the circular buffer



