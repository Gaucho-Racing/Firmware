#ifndef CUBE_CAN_CONFIG_H
#define CUBE_CAN_CONFIG_H

// TODO Add a template to the library

#include "main.h"

#define USECAN1
#define USECAN2

#define CUBEMX_CAN_TIMER_INSTANCE TIM5
#define CUBEMX_CAN_TX_QUEUE_SIZE 32U
#define CAN_TIMER_SEND_PERIOD_US 500U

#endif
