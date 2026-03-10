#ifndef CAN_TESTS_H
#define CAN_TESTS_H

#include "Logomatic.h"
#include "can.h"
#include "can_platform_deps.h"

// Tested STM32 Families
#define USECAN1
#define TX_BUFFER_2_SIZE 10
#define USECAN2
#define TX_BUFFER_3_SIZE 10

// abstract families
extern int defaultSTM32G4_CANCfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t Mode);
extern int get_cfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t Mode);

// testing functions
extern int can_external_test(void);
extern int can_internal_test(void);
extern int can_stress_test(void);
extern int can_release_test(void);

#endif
