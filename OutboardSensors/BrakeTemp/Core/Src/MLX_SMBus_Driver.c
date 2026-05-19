#include "MLX90614_SMBus_Driver.h"
#include "stm32g4xx_hal.h"

extern SMBUS_HandleTypeDef hsmbus;

/**
 * @brief Frequency stub. Configuration is handled in the SMBus Peripheral Init.
 */
void MLX90614_SMBusFreqSet(int freq)
{
    return;
}

/**
 * @brief Initialization stub.
 */
void MLX90614_SMBusInit(void)
{
    return;
}

/**
 * @brief Reads a 16-bit word from the MLX90614.
 * Uses SMBus Read Word: [Addr+W][Command][Addr+R][DataLSB][DataMSB][PEC]
 */
int MLX90614_SMBusRead(uint8_t slaveAddr, uint8_t readAddress, uint16_t *data)
{
    uint8_t buffer[3];

    // HAL_SMBUS_Mem_Read handles the repeated start and 8-bit command protocol
    if (HAL_SMBUS_Mem_Read(&hsmbus, (slaveAddr << 1), readAddress, SMBUS_MEMADD_SIZE_8BIT, buffer, 3, HAL_MAX_DELAY) != HAL_OK) {
        return -1;
    }

    *data = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);
    return 0;
}

/**
 * @brief Writes a 16-bit word to the MLX90614.
 * Uses SMBus Write Word: [Addr+W][Command][DataLSB][DataMSB][PEC]
 */
int MLX90614_SMBusWrite(uint8_t slaveAddr, uint8_t writeAddress, uint16_t data)
{
    uint8_t buffer[2];
    buffer[0] = data & 0x00FF;
    buffer[1] = (data >> 8) & 0x00FF;

    if (HAL_SMBUS_Mem_Write(&hsmbus, (slaveAddr << 1), writeAddress, SMBUS_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY) != HAL_OK) {
        return -1;
    }

    return 0;
}

/**
 * @brief Sends a lock (0x61) or unlock (0x60) command to EEPROM address 0x0F.
 *
 * @param slaveAddr 7-bit device address
 * @param command 0x60 (Unlock) or 0x61 (Lock)
 * @return 0 on success, -1 on NACK/Bus error, -5 for invalid command
 */
int MLX90614_SMBusSendCommand(uint8_t slaveAddr, uint8_t command)
{
    // Validate the command per requirements
    if (command != 0x60 && command != 0x61) {
        return -5;
    }

    /*
     * Uses SMBus 'Send Byte' protocol.
     * The MLX90614 interprets 0x60/0x61 as specific operations
     * for the EEPROM address 0x0F.
     */
    if (HAL_SMBUS_Master_Transmit(&hsmbus, (slaveAddr << 1), &command, 1, HAL_MAX_DELAY) != HAL_OK) {
        return -1; // Communication failure or NACK
    }

    return 0;
}
