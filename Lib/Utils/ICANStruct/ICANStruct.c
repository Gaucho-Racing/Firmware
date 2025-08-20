#include "ICANStruct.h"

/*
ALL THESE ARE PSUEDOCODE IMPLS.
each time cb is accessed a "message struct" is pushed or popped
*/

// __weak uint8_t CAN_Send(void) {
//     return 0;
// }

// __weak uint8_t CAN_Init(void) {
//     return 0;
// }
//^^these need to be in platform specific. 

uint8_t* RX_dequeue(CAN *self) {
    return self.RX_cb->pop();
}

uint8_t RX_enqueue(CAN *self, uint8_t* data, uint8_t size, uint32_t id) {
    //make a temp strcut
    return self.RX_cb->push(/*temp struct*/);
}

uint8_t TX_dequeue(CAN *self) {
    data = self.TX_cb->pop();
    c_send(data, sizeof(data));
    return 0;
}

uint8_t TX_enqueue(CAN *self, uint8_t* data, uint8_t size) {
    self.TX_cb->push(data);
}
