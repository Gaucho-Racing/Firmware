#ifndef ICANSTRUCT_H
#define ICANSTRUCT_H

//need to figure out how to have case dependent includes here, maybe have main.h define platform or smthn. 
#include <stm32g4xx_hal_fdcan.h>

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


    // data would need to be a 64 byte array to allow for "all" data to be stored.
    

    //functions that need to be platform specific
    uint8_t (*init)();
    uint8_t (*send)();
    uint8_t (*recieve)();

}CAN;


uint8_t RX_dequeue(CAN *self, FDCAN_RxHeaderTypeDef &header, uint8_t* data);
uint8_t RX_enqueue(CAN *self, FDCAN_RxHeaderTypeDef &header, uint8_t* data);
uint8_t TX_dequeue(CAN *self, FDCAN_TxHeaderTypeDef &header, uint8_t* data);
uint8_t TX_enqueue(CAN *self, FDCAN_TxHeaderTypeDef &header, uint8_t* data);
#endif // ICANSTRUCT_H