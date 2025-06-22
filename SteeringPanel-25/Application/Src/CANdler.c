#include <stdint.h>
#include <string.h>

#include "CANdler.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "steering.h"

volatile int numberOfBadMessages = 0;

void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length)
{
    // FIXME: Remove once testing works alright
    incomingData.cellData[0].cellVoltage = 100;
    // Remove once testing works alright

    switch(msgID) {
        case MSG_DEBUG_FD:
            if (length > 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            strncpy((char*)incomingData.debugMessage, (char*)data, length);

            break;

        case MSG_PING:
            if (length != 4) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }
            
            writeMessage(MSG_PING, srcID, data, length);    // Right back at you buckaroo

            break;

        case MSG_STEERING_CONFIG:
            // Reserved
            break;

        case MSG_ACU_CELL_DATA_1:
            if (length != 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            memcpy((char*)incomingData.cellData, data, length);

            break;

        case MSG_ACU_CELL_DATA_2:
            if (length != 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            memcpy((char*)incomingData.cellData + length, data, length);

            break;

        case MSG_ACU_CELL_DATA_3:
            if (length != 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            memcpy((char*)incomingData.cellData + 2 * length, data, length);

            break;

        case MSG_ACU_CELL_DATA_4:
            if (length != 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            memcpy((char*)incomingData.cellData + 3 * length, data, length);

            break;
            
        case MSG_ACU_CELL_DATA_5:
            if (length != 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            memcpy((char*)incomingData.cellData + 4 * length, data, length);

            break;

        case MSG_ECU_PING_INFORMATION:
            if (length != 3) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            memcpy((uint8_t*)incomingData.ecuPingMap, data, length);

            break;

        default:
            return;
    }

    updatedDataRecieved();
}
