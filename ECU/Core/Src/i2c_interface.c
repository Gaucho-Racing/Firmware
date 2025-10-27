#include "i2c_interface.h"
#include "i2c.h"

#define I2C_TIMEOUT 100 // Timeout duration

HAL_StatusTypeDef I2C_Write(uint16_t DevAddress, uint8_t *pData, uint16_t Size,
			    uint32_t Timeout)
{
	return HAL_I2C_Master_Transmit(&hi2c1, DevAddress, pData, Size,
				       Timeout);
}
HAL_StatusTypeDef I2C_Read(uint16_t DevAddress, uint8_t *pData, uint16_t Size,
			   uint32_t Timeout)
{
	return HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, Size, Timeout);
}