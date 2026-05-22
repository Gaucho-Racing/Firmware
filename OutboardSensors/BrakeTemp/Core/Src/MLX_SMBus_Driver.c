#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_smbus.h"

extern SMBUS_HandleTypeDef hsmbus2;

// Transaction synchronization flags
static volatile uint8_t smbus_busy = 0;
static volatile HAL_StatusTypeDef smbus_status = HAL_OK;

void MLX90614_SMBusFreqSet(int freq)
{
    return;
}

void MLX90614_SMBusInit(void)
{
    return;
}

/**
 * @brief Simple busy-wait for Interrupt completion.
 * In a production RTOS, you'd use a Semaphore or Event Group here.
 */
static int Wait_For_Transaction(uint32_t timeout_ms) {
    uint32_t start = HAL_GetTick();
    while (smbus_busy) {
        if ((HAL_GetTick() - start) > timeout_ms) return -1;
    }
    return (smbus_status == HAL_OK) ? 0 : -1;
}

/**
 * @brief Reads a 16-bit word from MLX90614 using SMBus Interrupts.
 * Protocol: [Addr+W][Command][Repeated Start][Addr+R][DataL][DataH][PEC]
 */
int MLX90614_SMBusRead(uint8_t slaveAddr, uint8_t readAddress, uint16_t *data)
{
    uint8_t buffer[3] = {0}; // LSB, MSB, PEC

    smbus_busy = 1;
    // MLX90614 expects a repeated start.
    // SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC handles the start, command, repeated start, read, and PEC check.
    if (HAL_SMBUS_Master_Receive_IT(&hsmbus2, (slaveAddr << 1), buffer, 3, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    if (Wait_For_Transaction(100) != 0) return -1;

    *data = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);
    return 0;
}

/**
 * @brief Writes a 16-bit word to MLX90614 using SMBus Interrupts.
 */
int MLX90614_SMBusWrite(uint8_t slaveAddr, uint8_t writeAddress, uint16_t data)
{
    uint8_t buffer[3]; // Command, LSB, MSB (PEC is appended by hardware)
    buffer[0] = writeAddress;
    buffer[1] = data & 0xFF;
    buffer[2] = (data >> 8) & 0xFF;

    smbus_busy = 1;
    // Hardware automatically calculates and appends PEC
    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), buffer, 3, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    if (Wait_For_Transaction(100) != 0) return -1;

    // MLX90614 requires ~10ms for EEPROM write cycles
    HAL_Delay(10);
    return 0;
}

/**
 * @brief Sends a lock/unlock byte command.
 */
int MLX90614_SendCommand(uint8_t slaveAddr, uint8_t command)
{
    if (command != 0x60 && command != 0x61) return -5;

    smbus_busy = 1;
    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), &command, 1, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    return Wait_For_Transaction(100);
}

/* --- REQUIRED HAL CALLBACKS --- */

void HAL_SMBUS_MasterTxCpltCallback(SMBUS_HandleTypeDef *hsmbus) {
    if (hsmbus->Instance == hsmbus2.Instance) {
        smbus_status = HAL_OK;
        smbus_busy = 0;
    }
}

void HAL_SMBUS_MasterRxCpltCallback(SMBUS_HandleTypeDef *hsmbus) {
    if (hsmbus->Instance == hsmbus2.Instance) {
        smbus_status = HAL_OK;
        smbus_busy = 0;
    }
}

void HAL_SMBUS_ErrorCallback(SMBUS_HandleTypeDef *hsmbus) {
    if (hsmbus->Instance == hsmbus2.Instance) {
        smbus_status = HAL_ERROR;
        smbus_busy = 0;
    }
}
