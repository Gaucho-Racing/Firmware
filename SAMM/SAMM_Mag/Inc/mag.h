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

#endif
