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

/**
 * @brief Reads a 16-bit word from MLX90614 RAM using 2-phase SMBus Interrupts.
 * Flow: [Start][Addr+W] -> [RAM Cmd] -> [Repeated Start][Addr+R] -> [DataL][DataH][PEC][Stop]
 * @return 0 on success, -1 on NACK/Timeout, -2 on PEC error.
 */
int MLX90614_SMBusRead(uint8_t slaveAddr, uint8_t readAddress, uint16_t *data)
{
    uint8_t buffer[3] = {0};

    // Enforce 5-bit isolation and apply RAM prefix (0b000x_xxxx)
    uint8_t cmd = 0x00 | (readAddress & 0x1F);

    /* =======================================================================
     * PHASE 1: Send RAM Command (Leaves bus open without a STOP bit)
     * ======================================================================= */
    smbus_busy = 1;
    smbus_status = HAL_OK;

    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), &cmd, 1, SMBUS_FIRST_FRAME) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    if (Wait_For_Transaction(50) != 0) return -1;

    /* =======================================================================
     * PHASE 2: Issue Repeated Start and Read Data + Hardware PEC
     * ======================================================================= */
    smbus_busy = 1;
    smbus_status = HAL_OK;

    if (HAL_SMBUS_Master_Receive_IT(&hsmbus2, (slaveAddr << 1), buffer, 3, SMBUS_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    if (Wait_For_Transaction(50) != 0) {
        if (HAL_SMBUS_GetError(&hsmbus2) == HAL_SMBUS_ERROR_PECERR) {
            return -2; // PEC discrepancy
        }
        return -1; // General NACK or timeout
    }

    // Process data (Little-Endian)
    *data = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);
    return 0;
}

/**
 * @brief Erases, writes, and verifies a 16-bit value in the MLX90614 EEPROM.
 * @return 0 on success, -1 on NACK/Timeout, -2 on Read PEC error, -3 on data mismatch.
 */
int MLX90614_SMBusWrite(uint8_t slaveAddr, uint8_t writeAddress, uint16_t data)
{
    uint8_t buffer[3];
    uint8_t readBuffer[3] = {0};
    uint16_t readBackData = 0;

    // Enforce 5-bit isolation and apply EEPROM prefix (0b001x_xxxx)
    uint8_t cmd = 0x20 | (writeAddress & 0x1F);

    /* =======================================================================
     * PHASE 1: Erase the EEPROM address (Write 0x0000)
     * ======================================================================= */
    buffer[0] = cmd;
    buffer[1] = 0x00; // Low Data
    buffer[2] = 0x00; // High Data

    smbus_busy = 1;
    smbus_status = HAL_OK;

    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), buffer, 3, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    if (Wait_For_Transaction(50) != 0) return -1;
    HAL_Delay(5); // 5ms erase cycle delay

    /* =======================================================================
     * PHASE 2: Write Target Data to EEPROM address
     * ======================================================================= */
    buffer[0] = cmd;
    buffer[1] = data & 0xFF;        // Low Data Byte
    buffer[2] = (data >> 8) & 0xFF; // High Data Byte

    smbus_busy = 1;
    smbus_status = HAL_OK;

    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), buffer, 3, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    if (Wait_For_Transaction(50) != 0) return -1;
    HAL_Delay(5); // 5ms write cycle delay

    /* =======================================================================
     * PHASE 3: Read Back From EEPROM for Verification
     * ======================================================================= */
    // Send EEPROM Command
    smbus_busy = 1;
    smbus_status = HAL_OK;
    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), &cmd, 1, SMBUS_FIRST_FRAME) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }
    if (Wait_For_Transaction(50) != 0) return -1;

    // Receive data bytes + PEC check
    smbus_busy = 1;
    smbus_status = HAL_OK;
    if (HAL_SMBUS_Master_Receive_IT(&hsmbus2, (slaveAddr << 1), readBuffer, 3, SMBUS_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1;
    }

    if (Wait_For_Transaction(50) != 0) {
        if (HAL_SMBUS_GetError(&hsmbus2) == HAL_SMBUS_ERROR_PECERR) {
            return -2; // Read phase PEC error
        }
        return -1;
    }

    // Process and check data mismatch
    readBackData = (uint16_t)readBuffer[0] | ((uint16_t)readBuffer[1] << 8);
    if (readBackData != data) {
        return -3; // Stored data memory mismatch
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
    // Validate command input parameters first
    if (command != 0x60 && command != 0x61) {
        return -5; // Invalid command parameter
    }

    smbus_busy = 1;
    smbus_status = HAL_OK;

    // Execute transmission. Hardware automatically appends computed PEC.
    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus2, (slaveAddr << 1), &command, 1, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK) {
        smbus_busy = 0;
        return -1; // Local peripheral configuration error
    }

    // Wait for the transaction to clear over the bus hardware
    if (Wait_For_Transaction(100) != 0) {
        return -1; // Bus timed out or a NACK condition occurred
    }

    return 0; // Communication successful
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
