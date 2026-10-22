#ifndef CAN_CFG_H
#define CAN_CFG_H

// #define USEDMA //unsafe atm

#define USECAN1
#define TX_BUFFER_1_SIZE 30

#define CAN_TIMER_SEND_PERIOD_US 625

// not using
// #define CAN1_RX_GPIO_PORT
// #define CAN1_RX_GPIO_PIN
// #define CAN1_TX_GPIO_PORT
// #define CAN1_TX_GPIO_PIN

// Janky but the STM32G431 does not support TIM5, and we do not want to touch code that touches the ECU at this stage
#ifndef TIM5
#define SKETCHY_OVERRIDE
#endif
// FIXME After this development cycle, replace this janky workaround with a proper manner to configure the timer instance

#endif
