#include "gr_neopixel.h"
#include "main.h"
#include "spi.h"
#include <stdbool.h>
#include <stdint.h>

void Neopixel_update() {

    NeoPixelData globalNeoPixelData = {0};

    // temp color set
    globalNeoPixelData.RTD = COLOR_BLUE;
    globalNeoPixelData.TS_Active = COLOR_BLUE;

    uint8_t neopixelTransmission[48];
    for (int i = 0; i < 2; i++) {
        for (int j = 23; j >= 0; j--) {
            neopixelTransmission[i * 24 + 23 - j] = 0x4 + ((globalNeoPixelData.rawData[i] >> j) & 0x1 << 1); // 0x06 is high, 0x04 is low
        }
    }

    //might need to iterate through array and send each bit individually
    LL_SPI_TransmitData8(SPI1, neopixelTransmission);
}
