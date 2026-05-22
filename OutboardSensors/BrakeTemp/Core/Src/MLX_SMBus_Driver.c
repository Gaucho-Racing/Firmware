#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_smbus.h"

extern SMBUS_HandleTypeDef hsmbus2;

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
int MLX90614_SendCommand(uint8_t slaveAddr, uint8_t command)
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


/*

I2C i2c(p9, p10);

uint8_t Calculate_PEC(uint8_t, uint8_t);
void WaitEE(uint16_t ms);

void MLX90614_SMBusInit()
{
	i2c.stop();
}

int MLX90614_SMBusRead(uint8_t slaveAddr, uint8_t readAddress, uint16_t *data)
{
	uint8_t sa;
	int ack = 0;
	uint8_t pec;
	char cmd = 0;
	char smbData[3] = {0, 0, 0};
	uint16_t *p;

	p = data;
	sa = (slaveAddr << 1);
	pec = sa;
	cmd = readAddress;

	i2c.stop();
	wait_us(5);
	ack = i2c.write(sa, &cmd, 1, 1);

	if (ack != 0x00) {
		return -1;
	}

	sa = sa | 0x01;
	ack = i2c.read(sa, smbData, 3, 0);

	if (ack != 0x00) {
		return -1;
	}
	i2c.stop();

	pec = Calculate_PEC(0, pec);
	pec = Calculate_PEC(pec, cmd);
	pec = Calculate_PEC(pec, sa);
	pec = Calculate_PEC(pec, smbData[0]);
	pec = Calculate_PEC(pec, smbData[1]);

	if (pec != smbData[2]) {
		return -2;
	}

	*p = (uint16_t)smbData[1] * 256 + (uint16_t)smbData[0];

	return 0;
}

void MLX90614_SMBusFreqSet(int freq)
{
	i2c.frequency(1000 * freq);
}

int MLX90614_SMBusWrite(uint8_t slaveAddr, uint8_t writeAddress, uint16_t data)
{
	uint8_t sa;
	int ack = 0;
	char cmd[4] = {0, 0, 0, 0};
	static uint16_t dataCheck;
	uint8_t pec;

	sa = (slaveAddr << 1);
	cmd[0] = writeAddress;
	cmd[1] = data & 0x00FF;
	cmd[2] = data >> 8;

	pec = Calculate_PEC(0, sa);
	pec = Calculate_PEC(pec, cmd[0]);
	pec = Calculate_PEC(pec, cmd[1]);
	pec = Calculate_PEC(pec, cmd[2]);

	cmd[3] = pec;

	i2c.stop();
	wait_us(5);
	ack = i2c.write(sa, cmd, 4, 0);

	if (ack != 0x00) {
		return -1;
	}
	i2c.stop();

	WaitEE(10);

	MLX90614_SMBusRead(slaveAddr, writeAddress, &dataCheck);

	if (dataCheck != data) {
		return -3;
	}

	return 0;
}

int MLX90614_SendCommand(uint8_t slaveAddr, uint8_t command)
{
	uint8_t sa;
	int ack = 0;
	char cmd[2] = {0, 0};
	uint8_t pec;

	if (command != 0x60 && command != 0x61) {
		return -5;
	}

	sa = (slaveAddr << 1);
	cmd[0] = command;

	pec = Calculate_PEC(0, sa);
	pec = Calculate_PEC(pec, cmd[0]);

	cmd[1] = pec;

	i2c.stop();
	wait_us(5);
	ack = i2c.write(sa, cmd, 2, 0);

	if (ack != 0x00) {
		return -1;
	}
	i2c.stop();

	return 0;
}

uint8_t Calculate_PEC(uint8_t initPEC, uint8_t newData)
{
	uint8_t data;
	uint8_t bitCheck;

	data = initPEC ^ newData;

	for (int i = 0; i < 8; i++) {
		bitCheck = data & 0x80;
		data = data << 1;

		if (bitCheck != 0) {
			data = data ^ 0x07;
		}
	}
	return data;
}
    */
