#include "mag.h"
#include "stdbool.h"
#include <stdio.h>

#include "stm32h5xx.h"
#include "stm32h5xx_hal_spi.h"

#define mag_write_address_msb 22
#define mag_write_address_lsb 23

#define mag_addr_mask 0x3F
#define mag_msb 0xFF00
#define mag_lsb 0x00FF

void mag_init(mag mag_dev, SPI_HandleTypeDefspi_port, GPIO_TypeDef port, uint16_t pin)
{
	mag_dev->spi_port = spi_port;
	mag_dev->port = port;
	mag_dev->pin = pin;

	uint16_t sta = mag_read(mag_dev, 0x22);
	if (!(sta & 0x0001)) {
		return HAL_ERROR;
	}

	mag_write(mag_dev, 0x1E, 0x01);

	return HAL_OK;
}

uint16_t mag_transmit(mag mag_dev, uint16_t data)
{
	uint8_t tx_word[2] = {data >> 8, data & 0xFF};
	uint8_t rx_word[2] = {0};

	//HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_RESET); //A1113 chip select active low
	HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_SET); //disable
	bool res = HAL_SPI_TransmitReceive(mag_dev->spi_port, tx_word, rx_word, 2, HAL_MAX_DELAY);
	//fix-me add error handling
	HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_SET);

	return ((uint16_t)rx_word[0] << 8) | rx_word[1];
}

/*
0
0 (W/R')
addr (6 bits)
data (8 bits)
crc (4 bits -- optional)

0
0
addr
0x00
crc
*/
uint16_t mag_read(mag mag_dev, uint8_t reg)
{
	uint16_t cmd = (uint16_t)((reg & mag_addr_mask) << 8);    // read: bit 14 = 0, address in bits 13:8
	mag_transmit(mag_dev, cmd);	      // frame 1: send command, discard response
	return mag_transmit(mag_dev, 0x0000); // frame 2: NOP, receive data
}

/*
write cycle:
0
1 (W/R')
addr (6 bits)
data (8 bits)
crc (4 bits -- optional)
*/

uint16_t mag_write(mag mag_dev, uint8_t reg, uint16_t data)
{
	uint16_t msb = (data & mag_msb) >> 8 | ((uint16_t(reg) & mag_addr_mask) << 8) | 0x4000;
	mag_transmit(mag_dev, mag_dev->spi_port, mag_dev->port, mag_dev->pin, msb) reg += 1; //increment from 0x22 to 0x23 for lsb
	uint16_t lsb = (data & mag_lsb) | ((uint16_t(reg) & mag_addr_mask) << 8)) | 0x4000;
	mag_transmit(mag_dev, mag_dev->spi_port, mag_dev->port, mag_dev->pin, lsb) return 0;
}

uint8_t mag_calib_abort(mag *mag_dev)
{
	mag_write(mag_dev, mag_CMD, mag_CMD_CALIB_ABORT);
	return 1;
}

// may ormay not work
uint16_t mag_read_encoder_angle(mag mag_dev)
{
	uint16_t read_angle = mag_transmit(mag_dev, 0x32); // 0x32 is angle register
	return ((uint16_t)(read_angle & 0x7FFF));	   // Mask to 15 bits (valid angle data)
}

// Address 0x22:0x23 (STA)—Device Status
// read bit 0 AOK [0]
// Address 0x28:0x29 (TSEN)—Temperature Sensor

/*
TODO:
Check the status of the acc, gyro and temp before returning the values
    if they are not ready return 0
    if they are ready return 1
    need to pass by reference the values to be returned

12 bit angle: 0 EF UV P data(12 bits) -- can use EF for error flag, uv for undervoltage

0x09 : extended write status
0x0D : extended read status
*/

bool check_status(mag mag_dev) {
	//Device Error Flags
	uint32_t error25 = mag_read(mag_dev, 0x25);
	bool voltage_err = ((error25 & 0x18) > 0);
	bool magnetic_err = ((error25 & 0x02) > 0);

	uint32_t error24 = mag_read(mag_dev, 0x24);
	bool angle_error = ((error24 & 0x02) > 0);

	//Device Warning Flags
	uint32_t warning27 = mag_read(mag_dev, 0x27);
	uint32_t warning26 = mag_read(mag_dev, 0x26);

	bool invalid_spi_len = ((warning26 & 0x80) > 0);
	bool temp_out_of_range = ((warning27 & 0x40) > 0);

	bool turn_counter_saturated = ((warning27 && 0x01) > 0);
	bool excessive_magnet_vel = ((warning27 && 0x08) > 0);

	//fix-me add LOGOMATIC

	return true;
}


uint16_t mag_read_acc_x(mag *mag_dev)
{
	return mag_read(mag_dev, mag_ACC_X);
}

uint16_t mag_read_acc_y(mag *mag_dev)
{
	return mag_read(mag_dev, mag_ACC_Y);
}

uint16_t mag_read_acc_z(mag *mag_dev)
{
	return mag_read(mag_dev, mag_ACC_Z);
}

uint16_t mag_read_gyr_x(mag *mag_dev)
{
	return mag_read(mag_dev, mag_GYR_X);
}

uint16_t mag_read_gyr_y(mag *mag_dev)
{
	return mag_read(mag_dev, mag_GYR_Y);
}

uint16_t mag_read_gyr_z(mag *mag_dev)
{
	return mag_read(mag_dev, mag_GYR_Z);
}

uint16_t mag_read_temp_data(mag *mag_dev)
{
	return mag_read(mag_dev, mag_TEMP_DATA);
}

uint16_t mag_read_status(mag *mag_dev)
{
	return mag_read(mag_dev, mag_STATUS);
}

uint16_t mag_read_err_reg(mag *mag_dev)
{
	return mag_read(mag_dev, mag_ERR_REG);
}

uint16_t mag_read_chip_id(mag *mag_dev)
{
	return mag_read(mag_dev, mag_CHIP_ID);
}

uint16_t mag_read_acc_conf(mag *mag_dev)
{
	return mag_read(mag_dev, mag_ACC_CONF);
}

uint16_t mag_read_gyr_conf(mag *mag_dev)
{
	return mag_read(mag_dev, mag_GYR_CONF);
}

uint8_t mag_enable_acc(mag *mag_dev, uint8_t acc_mode, uint8_t acc_avg_num, uint8_t acc_bw, uint8_t acc_range, uint8_t acc_odr)
{
	// uint16_t acc_conf = mag_read_acc_conf(mag_dev);
	uint16_t new_conf = 0;
	new_conf |= acc_mode << 12;
	new_conf |= acc_avg_num << 8;
	new_conf |= acc_bw << 7;
	new_conf |= acc_range << 4;
	new_conf |= acc_odr;
	mag_write(mag_dev, mag_ACC_CONF, new_conf);
	return 1;
}

uint8_t mag_enable_gyro(mag *mag_dev, uint8_t gyr_mode, uint8_t gyr_avg_num, uint8_t gyr_bw, uint8_t gyr_range, uint8_t gyr_odr)
{
	// uint16_t acc_conf = mag_read_acc_conf(mag_dev);
	uint16_t new_conf = 0;
	new_conf |= gyr_mode << 12;
	new_conf |= gyr_avg_num << 8;
	new_conf |= gyr_bw << 7;
	new_conf |= gyr_range << 4;
	new_conf |= gyr_odr;
	mag_write(mag_dev, mag_GYR_CONF, new_conf);
	return 1;
}
