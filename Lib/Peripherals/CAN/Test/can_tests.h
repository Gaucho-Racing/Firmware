#ifndef CAN_TESTS_H
#define CAN_TESTS_H

#include "Logomatic.h"
#include "can.h"
#include "can_platform_deps.h"

// Tested STM32 Families
#ifdef STM32G4
#define USECAN1
#define TX_BUFFER_2_SIZE 10
#define USECAN2
#define TX_BUFFER_3_SIZE 10

// #elif defined(STM32L4)
// #elif defined(STM32U5)
#else
#error "Untested STM32 Family"
#endif

// Assume the LOGOMATIC is setup correctly
int can_test(void); // top-level function, just call this and check if the return status is correct

#endif
