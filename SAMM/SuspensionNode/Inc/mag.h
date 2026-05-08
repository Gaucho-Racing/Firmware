#ifndef __MAG_H__
#define __MAG_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "stm32h5xx.h"

typedef struct {
	SPI_HandleTypeDef *spi_port;
	uint16_t chip_id;
	GPIO_TypeDef *port;
	uint16_t pin;
	/* data */
} mag;

HAL_StatusTypeDef mag_init(mag *mag_dev, SPI_HandleTypeDef *spi_port, GPIO_TypeDef *port, uint16_t pin);

uint16_t mag_transmit(mag *mag_dev, uint16_t data);

uint16_t mag_read(mag *mag_dev, uint8_t reg);

void mag_write(mag *mag_dev, uint8_t reg, uint16_t data);

uint16_t mag_read_encoder_angle(mag *mag_dev);

bool mag_read_device_status(mag *mag_dev);

uint8_t mag_read_temp(mag *mag_dev);

bool check_status(mag *mag_dev);

int16_t mag_read_turns(mag *mag_dev);

float mag_read_HANG(mag *mag_dev);

void mag_write_error(mag *mag_dev);

#endif
