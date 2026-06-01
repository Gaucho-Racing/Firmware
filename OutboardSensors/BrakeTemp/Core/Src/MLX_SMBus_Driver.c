#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_smbus.h"

#define GR_MLX_TIMEOUT_MS 1000
#define GR_MLX_SMBUS_BUSY 1
#define GR_MLX_SMBUS_IDLE 0

extern SMBUS_HandleTypeDef hsmbus2;

// Transaction synchronization flags
static volatile uint8_t smbus_status = GR_MLX_SMBUS_IDLE;
static uint32_t timeout_start = 0;

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

static uint8_t Calculate_CRC8(uint8_t *data, uint8_t len) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief Simple busy-wait for Interrupt completion.
 * In a production RTOS, you'd use a Semaphore or Event Group here.
 */
static int Wait_For_Transaction(uint32_t timeout_ms) {
    timeout_start = HAL_GetTick();
    while (smbus_status == GR_MLX_SMBUS_BUSY) {
        if ((HAL_GetTick() - timeout_start) > timeout_ms) return -1;
    }
    return HAL_OK;
}

// Reading does not require a PEC byte to work!
// Format of cmd (1 byte):
//    RAM:    b'000x_xxxx, where x_xxxx is a 5-bit address of MLX90614 RAM register
//    EEPROM: b'001x_xxxx, where x_xxxx is a 5-bit address of MLX90614 EEPROM register
int MLX90614_SMBusRead(uint8_t slaveAddr, uint8_t cmd, uint16_t *data) {
    uint8_t buffer[3] = {0};

    smbus_status = GR_MLX_SMBUS_BUSY;
    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), &cmd, 1, SMBUS_FIRST_FRAME) != HAL_OK) {
        smbus_status = GR_MLX_SMBUS_IDLE;
        return -1;
    }

    if(Wait_For_Transaction(GR_MLX_TIMEOUT_MS) != HAL_OK) return -1;
    while(HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY);

    smbus_status = GR_MLX_SMBUS_BUSY;
    if (HAL_SMBUS_Master_Receive_IT(&hsmbus2, (slaveAddr << 1), buffer, 3, SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK) {
        smbus_status = GR_MLX_SMBUS_IDLE;
        return -1;
    }

    if(Wait_For_Transaction(GR_MLX_TIMEOUT_MS) != HAL_OK) return -1;
    while(HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY);

    // Process data (Little-Endian)
    *data = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);
    return 0;
}

/**
 * @brief Erases, writes, and verifies a 16-bit value in the MLX90614 EEPROM.
 * @return 0 on success, -1 on NACK/Timeout, -2 on Read PEC error, -3 on data mismatch.
 */
// Format of cmd (1 byte):
//    RAM:    b'000x_xxxx, where x_xxxx is a 5-bit address of MLX90614 RAM register
//    EEPROM: b'001x_xxxx, where x_xxxx is a 5-bit address of MLX90614 EEPROM register
int MLX90614_SMBusWrite(uint8_t slaveAddr, uint8_t cmd, uint16_t data)
{
    uint8_t buffer[5];
    uint16_t readBackData = 0;

    buffer[0] = slaveAddr << 1;
    buffer[1] = cmd;
    buffer[2] = data & 0xFF;        // Low Data Byte
    buffer[3] = (data >> 8) & 0xFF; // High Data Byte
    buffer[4] = Calculate_CRC8(buffer, 4);

    smbus_status = GR_MLX_SMBUS_BUSY;
    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, buffer[0], &(buffer[1]), 4, SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK) {
        smbus_status = GR_MLX_SMBUS_IDLE;
        return -1;
    }

    if(Wait_For_Transaction(GR_MLX_TIMEOUT_MS) != HAL_OK) return -1;
    while(HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY);
    HAL_Delay(10); // 5ms delay minimum after write, 10ms is safe

    // Process and check data mismatch
    MLX90614_SMBusRead(slaveAddr, cmd, &readBackData);
    if (readBackData != data) {
        return -3;
    }

    return 0; // Operation completely successful
}

// Note that this command is for MLX90614xxC EEPROM lock mechanism.
// We are using MLX90614xxA so this function is a STUB and meant to make the MLX90614_API.c library happy.
int MLX90614_SendCommand(uint8_t slaveAddr, uint8_t cmd)
{
    UNUSED(slaveAddr);

    // Validate the expected parameter values for driver compliance
    if (cmd != 0x60 && cmd != 0x61) {
        return -5; // Invalid command according to spec
    }

    // Do NOT transmit anything over the wire.
    // Return 0 to tell the higher-level driver everything is fine.
    return 0;
}

/* --- REQUIRED HAL CALLBACKS --- */

void HAL_SMBUS_MasterTxCpltCallback(SMBUS_HandleTypeDef *hsmbus) {
    if (hsmbus->Instance == hsmbus2.Instance) {
        smbus_status = GR_MLX_SMBUS_IDLE;
    }
}

void HAL_SMBUS_MasterRxCpltCallback(SMBUS_HandleTypeDef *hsmbus) {
    if (hsmbus->Instance == hsmbus2.Instance) {
        smbus_status = GR_MLX_SMBUS_IDLE;
    }
}

void HAL_SMBUS_ErrorCallback(SMBUS_HandleTypeDef *hsmbus) {
    if (hsmbus->Instance == hsmbus2.Instance) {
        smbus_status = GR_MLX_SMBUS_IDLE;
    }
}
