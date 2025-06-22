// Testing LED lights on STM32

#include <stdint.h>

#include "stm32g4xx_hal.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void ledControl();

GPIO_InitTypeDef led;
GPIO_PinState ledState;
int ledArray[] = {15, 14, 13, 12, 11, 10, 9, 8};
uint32_t count;

int main()
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    led.Pin = GPIO_PIN_All;
    led.Mode = GPIO_MODE_OUTPUT_PP;
    led.Pull = GPIO_NOPULL;
    led.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &led);
    count = 1;
    while (1)
    {
        ledControl();
    }

    return 0;
}

void ledControl()
{
    // Should we toggle the LED here?
}
