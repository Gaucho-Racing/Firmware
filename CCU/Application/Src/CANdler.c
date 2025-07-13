#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "msgIDs.h"
#include "grIDs.h"
#include "CANdler.h"

volatile uint8_t numberOfBadMessages = 0;

void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length)
{
    if (length > 8) {
        numberOfBadMessages++;
        return;
    } else {
        numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
    }
}
