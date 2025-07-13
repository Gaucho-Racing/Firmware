#include <stdbool.h>

#include "buttons.h"
#include "cmsis_os2.h"
#include "stm32u5xx.h"
#include "grIDs.h"
#include "fdcan.h"
#include "main.h"
#include "utils.h"
#include "dash.h"
#include "msgIDs.h"
#include "dash.h"
#include "spi.h"

NeoPixelData globalNeoPixelData = {0};

void colorPin(Color color, ButtonNames button)
{
    if ((button == TS_ACTIVE_BUTTON && color == globalNeoPixelData.TS_Active) || (button == RTD_BUTTON && color == globalNeoPixelData.RTD))
    {
        return;
    }

    switch(button)
    {
        case TS_ACTIVE_BUTTON:
            globalNeoPixelData.TS_Active = color;
            break;

        case RTD_BUTTON:
            globalNeoPixelData.RTD = color;
            break;
    }
}

void updateButtonColors(void* args)
{
    UNUSED(args);

    for (;;)
    {
        switch(globalStatus.ecuState)
        {
            case PRECHARGE_COMPLETE:
                globalNeoPixelData.RTD = (HAL_GetTick() * 71) & 0xFFFF00;    // Nice slow pulse
                break;

            case DRIVE_STANDBY:
            case DRIVE_ACTIVE_IDLE:
            case DRIVE_ACTIVE_POWER:
            case DRIVE_ACTIVE_REGEN:
                globalNeoPixelData.RTD = COLOR_RED;
                break;

            default:
                globalNeoPixelData.RTD = COLOR_BLUE;
                break;
        }

        switch(globalStatus.ecuState)
        {
            case TS_DISCHARGE_OFF:
            case ERRORSTATE:
                globalNeoPixelData.TS_Active = COLOR_BLUE;
                break;

            case GLV_ON:
                globalNeoPixelData.TS_Active = (HAL_GetTick() * 71) & 0xFFFF00;  // Nice slow pulse
                break;

            default:
                globalNeoPixelData.TS_Active = COLOR_RED;
        }

        // Dark tech right here sorry; essentially a waveform through an SPI message
        uint8_t neopixelTransmission[48];
        for (int i = 0; i < 2; i++)
        {
            for (int j = 23; j >= 0; j--)
            {
                neopixelTransmission[i * 24 + 23 - j] = 0x4 + ((globalNeoPixelData.rawData[i] >> j) & 0x1 << 1);    // 0x06 is high, 0x04 is low
            }
        }
        HAL_SPI_Transmit(&hspi1, neopixelTransmission, 48, 1000);   // Maybe change to DMA

        osDelay(UPDATE_BUTTON_COLORS_DELAY);

        // Update LED colors (no other good spot for em)
        HAL_GPIO_WritePin(LED_AMS_GPIO_Port, LED_AMS_Pin, getBit(globalStatus.dashStatusMsg.ledBits, 0));
        HAL_GPIO_WritePin(LED_IMD_GPIO_Port, LED_IMD_Pin, getBit(globalStatus.dashStatusMsg.ledBits, 1));
        HAL_GPIO_WritePin(LED_BSPD_GPIO_Port, LED_BSPD_Pin, getBit(globalStatus.dashStatusMsg.ledBits, 2));
    }
}

void pollButtonState(void* args)
{
    UNUSED(args);

    for (;;)
    {
        bool newValue = !HAL_GPIO_ReadPin(NO_RTD_GPIO_Port, NO_RTD_Pin);   // Active low

        if (globalStatus.dashStatusMsg.rtdButtonData != newValue)
        {
            globalStatus.dashStatusMsg.rtdButtonData = newValue;
            LOGOMATIC("RTD status changed, now reading %d\n", newValue);
            recievedNewInformationPleaseRefresh = true;
        }

        newValue = !HAL_GPIO_ReadPin(NO_TS_ACTIVE_GPIO_Port, NO_TS_ACTIVE_Pin);  // Active low

        if (globalStatus.dashStatusMsg.tsButtonData != newValue)
        {
            globalStatus.dashStatusMsg.tsButtonData = newValue;
            LOGOMATIC("TS Active status changed, now reading %d\n", newValue);
            recievedNewInformationPleaseRefresh = true;
        }

        osDelay(POLL_BUTTON_STATE_DELAY);
    }
}
