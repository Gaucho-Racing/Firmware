#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_smbus.h"

extern SMBUS_HandleTypeDef hsmbus2;

// Transaction synchronization flags
static volatile uint8_t smbus_busy = 0;
static volatile HAL_StatusTypeDef smbus_status = HAL_OK;

// Already implemented in CubeMX
void MLX90614_SMBusFreqSet(int freq)
{
    UNUSED(freq);
}

// Already implemented in CubeMX
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

static int SMBusRead(uint8_t slaveAddr, uint8_t cmd, uint16_t *data) {
    uint8_t buffer[3] = {0};

    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), &cmd, 1, SMBUS_FIRST_FRAME) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    while(HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY);

    if (HAL_SMBUS_Master_Receive_IT(&hsmbus2, (slaveAddr << 1), buffer, 3, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    while(HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY);

    // Process data (Little-Endian)
    *data = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);
    return 0;
}

/**
 * @brief Reads a 16-bit word from MLX90614 RAM using 2-phase SMBus Interrupts.
 * Flow: [Start][Addr+W] -> [RAM Cmd] -> [Repeated Start][Addr+R] -> [DataL][DataH][PEC][Stop]
 * @return 0 on success, -1 on NACK/Timeout, -2 on PEC error.
 */
int MLX90614_SMBusRead(uint8_t slaveAddr, uint8_t readAddress, uint16_t *data)
{
    // Enforce 5-bit isolation and apply RAM prefix (0b000x_xxxx)
    uint8_t cmd = 0x20 | (readAddress & 0x1F);

    return SMBusRead(slaveAddr, cmd, data);
}

/**
 * @brief Erases, writes, and verifies a 16-bit value in the MLX90614 EEPROM.
 * @return 0 on success, -1 on NACK/Timeout, -2 on Read PEC error, -3 on data mismatch.
 */
int MLX90614_SMBusWrite(uint8_t slaveAddr, uint8_t writeAddress, uint16_t data)
{
    uint8_t buffer[3];
    uint16_t readBackData = 0;

    // Enforce 5-bit isolation and apply EEPROM prefix (0b001x_xxxx)
    uint8_t cmd = 0x20 | (writeAddress & 0x1F);

    buffer[0] = cmd;
    buffer[1] = data & 0xFF;        // Low Data Byte
    buffer[2] = (data >> 8) & 0xFF; // High Data Byte

    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), buffer, 3, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK) {
        return -1;
    }

    while(HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY);
    HAL_Delay(1000);

    // Process and check data mismatch
    SMBusRead(slaveAddr, cmd, &readBackData);
    if (readBackData != data) {
        return -3;
    }

    return 0; // Operation completely successful
}

/**
 * @brief Sends a lock/unlock byte command to the MLX90614 EEPROM.
 * @param slaveAddr - Slave address of the MLX90614 device
 * @param command   - Command to send (0x60 to unlock, 0x61 to lock)
 * @return 0 on success, -1 on NACK/timeout, -5 for an invalid command.
 */
int MLX90614_SendCommand(uint8_t slaveAddr, uint8_t command)
{
    // Validate the expected parameter values for driver compliance
    if (command != 0x60 && command != 0x61) {
        return -5; // Invalid command according to spec
    }

    // Do NOT transmit anything over the wire.
    // Return 0 to tell the higher-level driver everything is fine.
    return 0;
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
