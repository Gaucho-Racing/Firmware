#ifndef CAN_TESTS_H
#define CAN_TESTS_H

#include "Logomatic.h"
#include "can.h"
#include "can_platform_deps.h"
#include "profile.h"

// abstract families
// extern int get_cfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t Mode);

// testing functions
extern int can_external_two_node(void);
extern int can_external_test(void);
extern int can_internal_test(void);
extern int can_stress_test(void);
extern int can_release_test(void);
extern int can_filter_test(void);
extern int can_lost_rx_test(void);


#endif
