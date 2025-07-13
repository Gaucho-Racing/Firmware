#include <stdint.h>
#include "stm32u5xx_hal.h"

#include "utils.h"

uint32_t millis(void) {
    return HAL_GetTick() * (1000 / TICK_FREQ);
}

uint8_t getBit(uint8_t number, uint8_t indexFromLeft)
{
    return (number >> (7 - indexFromLeft)) & 0x01;
}

uint8_t getBits(uint8_t number, uint8_t indexFromLeft, uint8_t length)
{
    return (number >> (8 - indexFromLeft - length)) & ((1 << length) - 1);
}