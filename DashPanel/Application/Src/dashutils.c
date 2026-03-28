#include "dashutils.h"

#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "gr_neopixel.h"
#include "StateMachine.h"
#include "main.h"
#include "bitManipulations.h"

NeopixelContext *NeoPixel_LED_Context;
NeopixelContext *NeoPixel_Button_Context;

static Neopixel_Color LED_colors[NUM_LEDS];
static Neopixel_Color button_colors[NUM_BUTTONS];

void NeoPixel_Init()
{
	NeopixelConfig NeoPixel_LED_Config = {.SPI_Instance = SPI2,
					      .Neopixel_Count = NUM_LEDS,
					      .GPIO_Port = Neopixel_GPIOB,
					      .GPIO_AlternateFunction = Neopixel_AF5,
					      .MOSI_Pin = LL_GPIO_PIN_15,
					      .SPI_BaudRatePrescaler = Neopixel_SPI_PS64};
	NeoPixel_LED_Context = Neopixel_Setup(&NeoPixel_LED_Config);

	NeopixelConfig NeoPixel_Button_Config = {.SPI_Instance = SPI3,
						 .Neopixel_Count = NUM_BUTTONS,
						 .GPIO_Port = Neopixel_GPIOC,
						 .GPIO_AlternateFunction = Neopixel_AF6,
						 .MOSI_Pin = LL_GPIO_PIN_12,
						 .SPI_BaudRatePrescaler = Neopixel_SPI_PS64};
	NeoPixel_Button_Context = Neopixel_Setup(&NeoPixel_Button_Config);
}

void Neopixel_LEDWrite()
{
	LED_colors[0] = GETBIT(dashStatus.led_bits, 0) ? COLOR_RED : COLOR_OFF;
	LED_colors[1] = GETBIT(dashStatus.led_bits, 1) ? COLOR_RED : COLOR_OFF;
	LED_colors[2] = GETBIT(dashStatus.led_bits, 2) ? COLOR_RED : COLOR_OFF;

	Neopixel_WriteAll(NeoPixel_LED_Context, LED_colors, sizeof(LED_colors));

	return;
}

void Neopixel_ButtonWrite()
{
	static uint32_t magic_color_counter = 0;

	// 1: TS Active, 2: RTD
	// If the button doesn't do anything, it's off
	// If the button will turn the car back a state, it's red
	// If the button is ready to be pressed, it's a magical color

	button_colors[0] = COLOR_OFF;
	button_colors[1] = COLOR_OFF;

	uint32_t COLOR_MAGICAL = (magic_color_counter++ * 27644437) & 0x00FFFFFF;

	switch (dashStatus.ECUState) {
		case GR_GLV_ON:
			button_colors[0] = COLOR_MAGICAL;
			button_colors[1] = COLOR_WHITE;
			break;
		case GR_PRECHARGE_ENGAGED:
			button_colors[0] = COLOR_RED;
			break;
		case GR_PRECHARGE_COMPLETE:
			button_colors[0] = COLOR_RED;
			button_colors[1] = COLOR_MAGICAL;
			break;
		case GR_DRIVE_ACTIVE:
			button_colors[0] = COLOR_RED;
			button_colors[1] = COLOR_RED;
			break;
	}

	Neopixel_WriteAll(NeoPixel_Button_Context, button_colors, sizeof(button_colors));
	return;
}
