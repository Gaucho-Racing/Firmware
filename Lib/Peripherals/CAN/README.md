- USAGE:

CANHandle* can_init(CANConfig *config); //user must supply an rx callback function

int can_set_clksource(uint32_t LL_RCC_FDCAN_CLKSOURCE); //ex LL_RCC_FDCAN_CLKSOURCE_PCLK1 for STM32G474RE

int can_start(CANHandle*handle);
int can_stop(CANHandle*handle);
int can_send(CANHandle*handle, FDCANMessage* buffer, size_t send);  
int can_release(CANHandle* handle); //deinit circular buffer and turn off can peripheral and gpios
int can_add_filter(CANHandle* handle, HAL_FDCAN_FilterTypeDef * filter);
int can_add_global_filter(CANHandle* handle, HAL_FDCAN_FilterTypeDef* filter); 

//alternatively instead use the HAL libraries
//HAL_FDCAN_ConfigGlobalFilter(canHandle->hal_fdcanP, filterTypeDef)
//HAL_FDCAN_ConfigFilter(ca)




PROBLEMS: 
Verify ISR safety, no race conditions, atomic read/writes
   - Interrupts keep firing while trying to can_release()
    - Could try to set the NVIC register to selectively disable interrupts (preferably using a bitmask)
- Need to discuss expected behaviour of API
    - particularly can_start, can_stop
    - can_release
- Freeing within ISRs whenever popping from CircularBuffer (yes its faster, than stack copies, but heap is getting fragmented)
- ISRS might take too long to resolve because popping and freeing circular buffer. 
:

IDEAS for other features:
- abstract to different STM families besides STM32G4
- Rx Buffering
- TX Buffering policy, do we spread them out over multiple TX buffers
- DMA support for copying from circular buffer, circular buffer could then be stack allocated 
- Smaller can headers for tx and rx (right now its just use the TXHeaderTypeDef)
- TX FIFO vs Queue policy (only allow FIFOS)
- Add support for RXFifo1

lot of work:
- Alternatively, rewrite without using HAL, just use CMSIS definitions. 
- PROS: Would look good on your Github. 
- CONS: Wouldn't help 