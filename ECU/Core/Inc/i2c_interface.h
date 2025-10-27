#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H
// header file for i2c interface
#include "stm32g4xx_hal_i2c.h"
#include <stdint.h>

extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef I2C_Write(uint16_t DevAddress, uint8_t *pData, uint16_t Size,
			    uint32_t Timeout);
HAL_StatusTypeDef I2C_Read(uint16_t DevAddress, uint8_t *pData, uint16_t Size,
			   uint32_t Timeout);

#endif // I2C_INTERFACE_H