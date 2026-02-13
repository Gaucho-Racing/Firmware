#include "gr_neopixel.h"

#include <stdbool.h>
#include <stdint.h>

void Neopixel_update()
{

	NeoPixelData globalNeoPixelData = {0};

	// temp color set
	globalNeoPixelData.RTD = 0x0000FF;
	globalNeoPixelData.TS_Active = 0x0000FF;

	uint8_t neopixelTransmission[48];
	for (int i = 0; i < 2; i++) {
		for (int j = 23; j >= 0; j--) {
			neopixelTransmission[i * 24 + 23 - j] = 0x4 + ((globalNeoPixelData.rawData[i] >> j) & 0x1 << 1); // 0x06 is high, 0x04 is low
		}
	}

	// HAL_SPI_Transmit(SPI1, neopixelTransmission, 48, 1000);

	for (int i = 0; i < sizeof(neopixelTransmission); i++) {
		LL_SPI_TransmitData8(SPI1, neopixelTransmission[i]);
	}
}
