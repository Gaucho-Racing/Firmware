#ifndef ICANSTRUCT_H
#define ICANSTRUCT_H

#include <stdint.h>

// Include necessary STM32 definitions
#define FDCAN1 ((FDCAN_GlobalTypeDef *) FDCAN1_BASE)  // Enable FDCAN definitions
#include "../../Platform/STM32G474xE/Drivers/CMSIS/Device/ST/STM32G4xx/Include/stm32g4xx.h"
#include "../../Platform/STM32G474xE/Drivers/stm32-hal-driver/Inc/stm32g4xx_hal_def.h"
#include "../../Platform/STM32G474xE/Drivers/stm32-hal-driver/Inc/stm32g4xx_hal_fdcan.h"
//^^ these are so that syntax stops fucking complaining
typedef struct{
    /*
    this struct will have the general CAN features 
    and will leave the "rest" of the hardware specific implementation to the user.
    Idea is to have the CAN struct stored here, have enqueue and dequeue covered here.
    These two functions should call some function that will do the logics. 
    In essence all the user will need to do is add hal can transmit, and CAN struct. 
    Then call enqueu and dequeue when needed. 
    */
    /*
    Also thinking of making this a sort of "shim" struct
    that allows for debugging to be present every time something 
    in CAN occurs
    */

    //add cb here

    //functions that need to be platform specific
    //only need two functions since we only care about starting as well as sending a can message 
    //In this case, c_send() would get called in the TX_dequeue call where ever that is
    //this also means that this is done "asynch" as sending will be timer based as in ACU. 
    uint8_t (*c_init)(void);
    uint8_t (*c_send)(uint8_t* data, uint8_t size);
} CAN;


uint8_t* RX_dequeue(CAN *self);
uint8_t RX_enqueue(CAN *self, uint8_t* data, uint8_t size, uint32_t id);
uint8_t TX_dequeue(CAN *self);
uint8_t TX_enqueue(CAN *self, uint8_t* data, uint8_t size);

// Weak default implementations - user can override these
__weak uint8_t CAN_Init(void);
__weak uint8_t CAN_Send(uint8_t* data, uint8_t size);

#endif // ICANSTRUCT_H