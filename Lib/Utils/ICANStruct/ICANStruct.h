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

    // meant for the hw side of the can, needs to be inited when making CAN struct. 
    FDCAN_HandleTypeDef hfdcan;

    //functions that need to be platform specific
    uint8_t (*c_mx_init)(void);
    uint8_t (*c_start)(CAN *self);
    uint8_t (*c_activate_notification)(CAN *self);
    uint8_t (*init)(CAN *self);
    uint8_t (*send)(CAN *self);
    uint8_t (*recieve)(CAN *self);
} CAN;


uint8_t RX_dequeue(CAN *self, FDCAN_RxHeaderTypeDef *header, uint8_t* data);
uint8_t RX_enqueue(CAN *self, FDCAN_RxHeaderTypeDef *header, uint8_t* data);
uint8_t TX_dequeue(CAN *self, FDCAN_TxHeaderTypeDef *header, uint8_t* data);
uint8_t TX_enqueue(CAN *self, FDCAN_TxHeaderTypeDef *header, uint8_t* data);

// Weak default implementations - user can override these
__weak uint8_t CAN_MX_Init(void);
__weak uint8_t CAN_Start(void);
__weak uint8_t CAN_Activate_Notification(void);
__weak uint8_t CAN_Init(void);
__weak uint8_t CAN_Send(void);
__weak uint8_t CAN_Receive(void);

// Helper function to initialize CAN struct with default weak functions
void CAN_InitStruct(CAN *can_instance);

#endif // ICANSTRUCT_H