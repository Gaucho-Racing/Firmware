#ifndef MCU_H
#define MCU_H

#include "bcc.h"
#include "gpio.h"
#include "spi.h"
#include "stm32g474xx.h"
#include "stm32g4xx_ll_gpio.h"
#include "system_stm32g4xx.h"
#include "tim.h"
#include "usart.h"

#include <debug.h>
#include <stdio.h>
#include <string.h>

#ifndef __UINT32_MAX__
#include <inttypes.h>
#else
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t; // maybe not needed?
#endif

extern void BCC_MCU_WaitUs(uint32_t delay);
extern void BCC_MCU_WaitMs(uint16_t delay);
extern bcc_status_t BCC_MCU_StartTimeout(uint32_t timeoutUs);
extern bool BCC_MCU_TimeoutExpired(void);
extern void BCC_MCU_Assert(const bool x);
extern bcc_status_t BCC_MCU_TransferSpi(const uint8_t drvInstance,
					uint8_t txBuf[], uint8_t rxBuf[]);
extern bcc_status_t BCC_MCU_TransferTpl(const uint8_t drvInstance,
					uint8_t txBuf[], uint8_t rxBuf[],
					const uint16_t recvTrCnt);
extern void BCC_MCU_WriteCsbPin(const uint8_t drvInstance, const uint8_t value);
extern void BCC_MCU_WriteRstPin(const uint8_t drvInstance, const uint8_t value);
extern void BCC_MCU_WriteEnPin(const uint8_t drvInstance, const uint8_t value);
extern uint32_t BCC_MCU_ReadIntbPin(const uint8_t drvInstance);

extern void BCC_MCU_WaitUs(uint32_t delay);
extern void BCC_MCU_WaitMs(uint16_t delay);
extern bcc_status_t BCC_MCU_StartTimeout(uint32_t timeoutUs);
extern bool BCC_MCU_TimeoutExpired(void);
extern void BCC_MCU_Assert(const bool x);
extern bcc_status_t BCC_MCU_TransferSpi(const uint8_t drvInstance,
					uint8_t txBuf[], uint8_t rxBuf[]);
extern bcc_status_t BCC_MCU_TransferTpl(const uint8_t drvInstance,
					uint8_t txBuf[], uint8_t rxBuf[],
					const uint16_t recvTrCnt);
extern void BCC_MCU_WriteCsbPin(const uint8_t drvInstance, const uint8_t value);
extern void BCC_MCU_WriteRstPin(const uint8_t drvInstance, const uint8_t value);
extern void BCC_MCU_WriteEnPin(const uint8_t drvInstance, const uint8_t value);
extern uint32_t BCC_MCU_ReadIntbPin(const uint8_t drvInstance);

#endif