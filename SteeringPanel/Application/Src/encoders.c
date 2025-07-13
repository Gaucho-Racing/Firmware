#include <stdint.h>

#include "encoders.h"
#include "steering.h"
#include "main.h"
#include "stm32u5xx_hal.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "grIDs.h"

volatile EncoderPercentages globalEncoderPercentages = {0};

void pollEncoderStatus(Encoder encoder)
{
    uint8_t value = 0;

    EncoderPercentages oldEncoderPercentages = globalEncoderPercentages;

    switch(encoder)
    {
        case ENC_CURRENT:
            value = 0 |
                (HAL_GPIO_ReadPin(CME_P1_GPIO_Port, CME_P1_Pin) << 3) |
                (HAL_GPIO_ReadPin(CME_P2_GPIO_Port, CME_P2_Pin) << 2) |
                (HAL_GPIO_ReadPin(CME_P3_GPIO_Port, CME_P3_Pin) << 1) |
                HAL_GPIO_ReadPin(CME_P4_GPIO_Port, CME_P4_Pin);

            break;

        case ENC_REGEN:
            value = 0 |
                (HAL_GPIO_ReadPin(RME_P1_GPIO_Port, RME_P1_Pin) << 3) |
                (HAL_GPIO_ReadPin(RME_P2_GPIO_Port, RME_P2_Pin) << 2) |
                (HAL_GPIO_ReadPin(RME_P3_GPIO_Port, RME_P3_Pin) << 1) |
                HAL_GPIO_ReadPin(RME_P4_GPIO_Port, RME_P4_Pin);

            break;

        case ENC_TORQUE:
            value = 0 |
                (HAL_GPIO_ReadPin(TME_P1_GPIO_Port, TME_P1_Pin) << 3) |
                (HAL_GPIO_ReadPin(TME_P2_GPIO_Port, TME_P2_Pin) << 2) |
                (HAL_GPIO_ReadPin(TME_P3_GPIO_Port, TME_P3_Pin) << 1) |
                HAL_GPIO_ReadPin(TME_P4_GPIO_Port, TME_P4_Pin);
    }

    switch (value)
    {
        case 0x0:        // 0 deg
            value = 0x0; // 00.0%
            break;

        case 0x4:        // 45 deg
            value = 0x2; // 13.3%
            break;

        case 0xC:        // 90 deg
            value = 0x4; // 26.6%
            break;

        case 0xE:        // 135 deg
            value = 0x6; // 40.0%
            break;

        case 0x6:        // 180 deg
            value = 0x8; // 53.3%
            break;

        case 0x7:        // 225 deg
            value = 0xB; // 73.3%
            break;

        case 0x3:        // 270 deg
            value = 0xD; // 86.6%
            break;

        case 0x2:        // 315 deg
            value = 0xF; // 100%
    }

    switch(encoder)
    {
        case ENC_CURRENT:
            globalEncoderPercentages.current = value;
            break;

        case ENC_REGEN:
            globalEncoderPercentages.current = value;
            break;

        case ENC_TORQUE:
            globalEncoderPercentages.torque = value;
            break;
    }

    if (oldEncoderPercentages.current != globalEncoderPercentages.current || oldEncoderPercentages.regen != globalEncoderPercentages.regen || oldEncoderPercentages.torque != globalEncoderPercentages.torque)
    {
        updatedDataRecieved();
    }
}

void pollButtonStatus(void)
{
    uint8_t old = outgoingData.steeringStatusMsg.RMEandButtonMap;

    outgoingData.steeringStatusMsg.RMEandButtonMap =
        ((outgoingData.steeringStatusMsg.RMEandButtonMap >> 4) << 4) |
        (HAL_GPIO_ReadPin(BUTTON_1_GPIO_Port, BUTTON_1_Pin) << 3) |
        (HAL_GPIO_ReadPin(BUTTON_2_GPIO_Port, BUTTON_2_Pin) << 2) |
        (HAL_GPIO_ReadPin(BUTTON_3_GPIO_Port, BUTTON_3_Pin) << 1) | 
        HAL_GPIO_ReadPin(BUTTON_4_GPIO_Port, BUTTON_4_Pin);
    
    if (old != outgoingData.steeringStatusMsg.RMEandButtonMap)
    {
        updatedDataRecieved();
    }
}
