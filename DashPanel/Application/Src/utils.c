#include "utils.h"

uint8_t getBit(uint8_t number, uint8_t index)
{
    return getBits(number, index, 1);
}

uint8_t getBits(uint8_t number, uint8_t index, uint8_t length)
{
    return (number >> (index)) & ((1 << length) - 1);
}

uint8_t findTernaryMax(const uint8_t a, const uint8_t b, const uint8_t c)
{
    if (a > b && a > c)
    {
        return a;
    }
    else if (b > a && b > c)
    {
        return b;
    }
    else
    {
        return c;
    }
}
