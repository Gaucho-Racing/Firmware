- USAGE:

CANHandle* can_init(CANConfig *config); //user must supply an rx callback function

int can_set_clksource(uint32_t LL_RCC_FDCAN_CLKSOURCE); //ex LL_RCC_FDCAN_CLKSOURCE_PCLK1 for STM32G474RE

int can_start(CANHandle*handle);
int can_stop(CANHandle*handle);
int can_send(CANHandle*handle, FDCANMessage* buffer, size_t send);  
int can_release(CANHandle* handle); //deinit circular buffer and turn off can peripheral and gpios
int can_add_filter(CANHandle* handle, HAL_FDCAN_FilterTypeDef * filter);
int can_add_global_filter(CANHandle* handle, HAL_FDCAN_FilterTypeDef* filter); 

//alternatively instead use the HAL filter functions
//HAL_FDCAN_ConfigGlobalFilter(canHandle->hal_fdcanP, filterTypeDef) 
//HAL_FDCAN_ConfigFilter(ca)

If no filters are set, the default behaviour is to accept all standard and extended frames into the RXFIFO0





PROBLEMS: 
Verify ISR safety, no race conditions, atomic read/writes
   - Interrupts keep firing while trying to can_release()
    - Could try to set the NVIC register to selectively disable interrupts (preferably using a bitmask)
- Need to discuss expected behaviour of API
    - particularly can_start, can_stop
    - can_release
- Freeing within ISRs whenever popping from CircularBuffer (yes its faster, than stack copies, but heap is getting fragmented)
- ISRS might take too long to resolve because popping and freeing circular buffer. 

- HARDCODE Platform Usage Flag for compiler definitions
- CAN.H expects #STM32G4 to be defined, 


- RX Callback must perform deep copy of data supplied to it - could also malloc, but not safe to do inside ISRs
-  


-Shouldn't disable GPIOs in the MSP layers when releasing, might affect other peripherals


IDEAS for other features:
- abstract to different STM families besides STM32G4
- Rx Buffering
- TX Buffering policy, do we spread them out over multiple TX buffers
- DMA support for copying from circular buffer, circular buffer could then be stack allocated 
- Smaller can headers for tx and rx (right now its just use the TXHeaderTypeDef)
- TX FIFO vs Queue policy (only allow FIFOS)
- Add support for RXFifo1


TESTING- ----------------------------------------------
USE LOGOMATIC for return status - 
either returns through semihosting or debug cores
LOGOMATIC is defined platform by platform

Testing framework
- Can operate on API states and behaviours, but API should work across platforms
- All API tests are defined in can_test.c
- All tests are run from the top level function in can_test.c

- can_test.c should initialize everything properly.
- May have to create platform specific asserts when testing state
- use LOGOMATIC to return errors or throw asserts

- Platform testing, such as in G4PERTESTING just needs include "can_test.h" and call top level function
in main. 

Two approaches:
Platform centric
- In G4PERTesting, include "can_tests.h" and call the top level function in can_test.c
- This approach is better because we can abstract the logging and debug method

Library Centric Testing:
- Test the implementation in each library. 

HAL_Rewrite:
- Alternatively, rewrite without using HAL, just use CMSIS definitions. 
- PROS: Would look good on your Github. 
- CONS: takes too long