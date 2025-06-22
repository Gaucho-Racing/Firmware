#include <stdint.h>
#include <stdbool.h>

// 0 indexed LSB first
bool getBit(uint8_t number, uint8_t index)
{
    return getBits(number, index, 1);   // Equivalent to `(number >> index) & 0x1
}

// 0 indexed LSB first
uint8_t getBits(uint8_t number, uint8_t index, uint8_t length)
{
    return (number >> index) & ((1 << length) - 1);
}
