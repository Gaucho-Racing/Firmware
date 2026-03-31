#ifndef __EXTRA_H__
#define __EXTRA_H__
#include <stdint.h>
/*
This is for some xtra fucntions that would be needed later on


*/

void MLX90640_I2CInit(void);
int MLX90640_I2CGeneralReset(void);
int MLX90640_I2CRead(uint8_t slaveAddr,uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);
int MLX90640_I2CWrite(uint8_t slaveAddr,uint16_t writeAddress, uint16_t data);
void MLX90640_I2CFreqSet(int freq);


uint8_t I2CRead(uint8_t slaveAddr,uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);
uint8_t I2CWrite(uint8_t slaveAddr,uint16_t writeAddress, uint16_t data);

#endif /* __EXTRA_H__ */