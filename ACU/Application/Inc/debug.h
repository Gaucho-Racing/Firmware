#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"
#include "bcc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "stm32g474xx.h"
#include <stdio.h>
#include <math.h>

#ifndef __UINT32_MAX__
    #include <inttypes.h>
#else
    typedef unsigned long uint32_t;
    typedef unsigned long long uint64_t; // maybe not needed?
#endif

#define DEBUG_MODE 0

#define DUMP_TARGETS 0
#define DUMP_ADC_DATA 1

#define DUMP_TEMPS 1
#define DUMP_VOLTS 1

#define CHARG_CTL 0
#define DUMP_ERR_WARN 0

#define DUMP_IMD_DATA 0
#define DUMP_CHARGER_DATA 0

#define DUMP_ENERGY_MEASURE_DATA 0

void print_bcc_status(bcc_status_t stat);
void debug();
#endif