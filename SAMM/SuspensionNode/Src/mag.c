#include "mag.h"

#include <stdio.h>

#include "main.h"
#include "stm32h5xx.h"

#define mag_write_address_msb 22
#define mag_write_address_lsb 23

#define mag_addr_mask 0x3F
#define mag_msb 0xFF00
#define mag_lsb 0x00FF

HAL_StatusTypeDef mag_init(mag *mag_dev, SPI_HandleTypeDef *spi_port, GPIO_TypeDef *port, uint16_t pin)
{
	mag_dev->spi_port = spi_port;
	mag_dev->port = port;
	mag_dev->pin = pin;

	LL_mDelay(20);

	uint16_t null = mag_read(mag_dev, 0x00);
	uint16_t error_reg = mag_read(mag_dev, 0x24);
	uint16_t status = mag_read(mag_dev, 0x22);
	UNUSED(null);	  // Used for debugging
	UNUSED(error_reg); // Used for debugging

	// Check AOK=1 (bit 0) and BIP=0 (bit 1)
	// If either not true, return error
	/*
	if (!((status & 0x0001) || ((status | ~(0x0002)) != 0xFFFF))) {
		return HAL_ERROR;
	}
	*/

	if (!(status & 0x0003)) {
		return HAL_ERROR;
	}

	mag_write(mag_dev, 0x1E, 0x01);

	return HAL_OK;
}

uint16_t mag_transmit(mag *mag_dev, uint16_t data)
{
	/*
	uint8_t tx_bytes[4] = {(uint8_t)(data >> 8), (uint8_t)(data & 0xFF), 0x00, 0x00};
	uint8_t rx_bytes[4] = {0};

	HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_RESET);
	LL_mDelay(5);
	HAL_StatusTypeDef res = HAL_SPI_TransmitReceive(mag_dev->spi_port, tx_bytes, rx_bytes, 4, HAL_MAX_DELAY);
	LL_mDelay(5);
	HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_SET);

	if (res != HAL_OK) {
		return 0xFFFF;
	}

	return ((uint16_t)rx_bytes[2] << 8) | rx_bytes[3];
	*/

	uint8_t tx_bytes[2] = {(uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};
	uint8_t tx_dummy[2] = {0};

	uint8_t rx_dummy[2] = {0};
	uint8_t rx_bytes[2] = {0};

	HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef send_res = HAL_SPI_TransmitReceive(mag_dev->spi_port, tx_bytes, rx_dummy, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_SET);

	if (send_res != HAL_OK) {
		return 0xFFFF;
	}

	LL_mDelay(1);

	HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef receive_res = HAL_SPI_TransmitReceive(mag_dev->spi_port, tx_dummy, rx_bytes, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(mag_dev->port, mag_dev->pin, GPIO_PIN_SET);

	if (receive_res != HAL_OK) {
		return 0xFFFF;
	}

	uint16_t new_data = ((uint16_t)rx_bytes[0] << 8) | rx_bytes[1];

	return new_data;
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

uint16_t mag_read(mag *mag_dev, uint8_t reg)
{
	uint16_t cmd = (uint16_t)((reg & mag_addr_mask) << 8); // read: bit 14 = 0, address in bits 13:8
	return mag_transmit(mag_dev, cmd);		       // frame 1: send command, discard response
}

/*
write cycle:
0
1 (W/R')
addr (6 bits)
data (8 bits)
crc (4 bits -- optional)
*/

void mag_write(mag *mag_dev, uint8_t reg, uint16_t data)
{
	uint16_t msb = ((data & mag_msb) >> 8) | (((uint16_t)reg & mag_addr_mask) << 8) | 0x4000;

	mag_transmit(mag_dev, msb);

	reg += 1; // increment from 0x22 to 0x23 for lsb

	uint16_t lsb = (data & mag_lsb) | (((uint16_t)reg & mag_addr_mask) << 8) | 0x4000;

	mag_transmit(mag_dev, lsb);
}

// Address 0x22:0x23 (STA)—Device Status
// read bit 0 AOK [0]
bool mag_read_device_status(mag *mag_dev)
{
	uint16_t read = mag_read(mag_dev, 0x22); // 0x22 is device status
	return (read & 0x0001);			 // read aok
}

// Address 0x28:0x29 (TSEN) — 12 bits
// Difference between junction (internal) temperature and room temperature
uint16_t mag_read_temp(mag *mag_dev)
{
	uint16_t read_temp = mag_read(mag_dev, 0x28);			       // 0x28 is temp register
	uint16_t masked_temp = read_temp & 0x0FFF;
	float_t calc_temp = (masked_temp / 8.0f) + 25.0f; // Mask to 12 bits (valid temp data), range is -60 to 180 C
	UNUSED(calc_temp); // for debugging

	return ((uint16_t)masked_temp);
}

// Address 0x30:0x31 (HANG) — 12 bits
// Hysteresis Angle Value (original range is 0 to 360 degrees)
uint16_t mag_read_HANG(mag *mag_dev)
{
	uint16_t read_HANG = mag_read(mag_dev, 0x30); // 0x30 is Hysteresis Angle Value
	return ((uint16_t)(read_HANG & 0x0FFF));      // Mask to 12 bits
}

// Address 0x32:0x33 (ANG15) — 15 bits
// Current Angle Reading (original range is 0 to 360 degrees)
uint16_t mag_read_encoder_angle(mag *mag_dev)
{
	uint16_t read_angle = mag_read(mag_dev, 0x32); // 0x32 is angle register
	uint16_t masked_angle = read_angle & 0x7FFF;
	float_t calc_angle = masked_angle * (360.0f / 32768.0f);
	UNUSED(calc_angle); // for debugging

	return ((uint16_t)(masked_angle));      // Mask to 15 bits (valid angle data) before conversion
}

// Address 0x2C:02D (TURNS) – 15 bits
// Total number of turns relative to angle observed on power-up
int16_t mag_read_turns(mag *mag_dev)
{
	uint16_t read_turns = mag_read(mag_dev, 0x2C); // 0x2C is turn counter
	int parity_bit = read_turns && (0x1000);
	UNUSED(parity_bit); // for debugging

	int16_t masked_turns = (int16_t)((read_turns & 0x0FFF) << 4) >> 4; // Mask to 12 bits (valid angle data)
	return masked_turns;
}

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

uint8_t check_status(mag *mag_dev)
{
	uint8_t errors = 0;
	// Device Error Flags
	uint16_t error25 = mag_read(mag_dev, 0x24);
	int voltage_err = ((error25 & 0x0C) > 0);
	errors += voltage_err * 64;
	int magnetic_err = ((error25 & 0x02) > 0);
	errors += magnetic_err * 32;
	uint16_t error24 = mag_read(mag_dev, 0x24);
	int angle_error = ((error24 & 0x02) > 0);
	errors += angle_error * 16;
	// Device Warning Flags
	uint32_t warning27 = mag_read(mag_dev, 0x27);
	uint32_t warning26 = mag_read(mag_dev, 0x26);

	int invalid_spi_len = ((warning26 & 0x80) > 0);
	errors += invalid_spi_len * 8;
	int temp_out_of_range = ((warning27 & 0x40) > 0);
	errors += temp_out_of_range * 4;

	int turn_counter_saturated = ((warning27 & 0x01) > 0);
	errors += turn_counter_saturated * 2;
	int excessive_magnet_vel = ((warning27 & 0x08) > 0);
	errors += excessive_magnet_vel;

	// fix-me add LOGOMATIC
	if (errors != 0) {
		LOGOMATIC("Something is cooked");
	}
	return errors;
}

// Address 0x24:0x25 (ERR)—Device Error Flags
//  FIXME: add error handling

// Address 0x1E:0x1F (CTRL)—Device Control
void mag_write_error(mag *mag_dev)
{
	mag_write(mag_dev, 0x1E, 0x0300);
	return;
}
//  FIXME: add error flag handling
