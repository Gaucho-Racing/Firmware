#include "StateData.h"
#include <string.h>

//FIXME: This is all gonna go


//Set ccu state
static void ccu_set_state(ccu_t *ccu, ccu_state_t s){

    ccu->state = s;
    ccu->ms_in_state = 0;
}

//Change ccu state if fault detected
static void ccu_fault_detected(ccu_t *ccu, ccu_fault_t f){\

    ccu->fault = f;
    ccu_set_state(ccu, CCU_STATE_FAULT);
}