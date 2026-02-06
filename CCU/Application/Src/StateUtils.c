#include "StateUtils.h"
#include "CCUStateData.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"
#include "Logomatic.h"


void setSoftwareLatch(bool close){


    if (close && !(HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))){
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
        LOGOMATIC("Software Latch: High");
    } else if (!(close) && HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port,SOFTWARE_OK_CONTROL_Pin)){
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
        LOGOMATIC("Software Latch: Low");
    }

}
