#include "extra.h"
#include "main.h"
#include "i2c.h"
#include <stdint.h>


void MLX90640_I2CInit(void)
{
    MX_I2C2_Init();
    return;
}

int MLX90640_I2CGeneralReset(void)
{
    uint8_t data = 0x06;
    return HAL_I2C_Master_Transmit(&hi2c2, 0x00, &data , 1, 1000);
}


int MLX90640_I2CRead(uint8_t slaveAddr,uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    //HAL_I2C_Master_Receive(&hi2c2, slaveAddr, data, 2*nMemAddressRead, 1000);
    //HAL_I2C_Master_Receive_DMA(&hi2c2, slaveAddr, data, nMemAddressRead);
    /*
    HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
    */
    //return HAL_I2C_Mem_Read_DMA(&hi2c2, slaveAddr, startAddress, I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, nMemAddressRead);
    return HAL_I2C_Mem_Read(&hi2c2, slaveAddr, startAddress, I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, 2*nMemAddressRead, 10000);
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
    // union
    // {
    //     uint8_t bytes[2];
    //     uint16_t data;
    // }extra  = {.data = data};
    return HAL_I2C_Mem_Write(&hi2c2, slaveAddr, writeAddress, I2C_MEMADD_SIZE_16BIT, (uint8_t *)&data, 2, 10000);
    //return HAL_I2C_Master_Transmit_DMA(&hi2c2, writeAddress, extra.bytes, 2);
}

void MLX90640_I2CFreqSet(int freq)
{
   return;
}

uint8_t I2CRead(uint8_t slaveAddr,uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    return HAL_I2C_Mem_Read(&hi2c2, slaveAddr, startAddress, I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, 2*nMemAddressRead, 500);
}

uint8_t I2CWrite(uint8_t slaveAddr,uint16_t writeAddress, uint16_t data)
{

    return HAL_I2C_Mem_Write(&hi2c2, slaveAddr, writeAddress, I2C_MEMADD_SIZE_16BIT, (uint8_t *) &data, 2, 500);
}

