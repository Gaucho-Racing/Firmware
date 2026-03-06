#include "dashutils.h"
#include "gr_neopixel.h"
#include "main.h"
#include "Logomatic.h"

NeopixelContext *NeoPixel_LED_Context;
NeopixelContext *NeoPixel_Button_Context;

static uint32_t i = 0;

void NeoPixel_Init(){
	// LED NeoPixel Config
	NeopixelConfig NeoPixel_LED_Config = {.SPI_Instance = SPI1,
				  .NumberOfNeopixels = NUM_LEDS, // 3 LEDs
				  .gpio_port = Neopixel_GPIOA,
				  .neopixelAF = Neopixel_GPIO_AF_5,
				  .mosi_gpio_pin = LL_GPIO_PIN_7,
				  .neopixel_baudRatePrescaler = Neopixel_SPI_BaudRatePrescaler_Div64};

	// LED NeoPixel Config
	NeopixelConfig NeoPixel_Button_Config = {.SPI_Instance = SPI3,
				  .NumberOfNeopixels = NUM_BUTTONS,
				  .gpio_port = Neopixel_GPIOC,
				  .neopixelAF = Neopixel_GPIO_AF_6,
				  .mosi_gpio_pin = LL_GPIO_PIN_12,
				  .neopixel_baudRatePrescaler = Neopixel_SPI_BaudRatePrescaler_Div64};

	NeoPixel_LED_Context = Neopixel_Setup(&NeoPixel_LED_Config);
	NeoPixel_Button_Context = Neopixel_Setup(&NeoPixel_Button_Config);
}

void Neopixel_LEDWrite() {

	// 1: BMS, 2: IMD, 3: BSPD
	LED_colors[0] = (dashStatus.led_bits & 0x01) ? COLOR_RED : COLOR_OFF;
	LED_colors[1] = (dashStatus.led_bits & 0x02) ? COLOR_RED : COLOR_OFF;
	LED_colors[2] = (dashStatus.led_bits & 0x03) ? COLOR_RED : COLOR_OFF;

	Neopixel_WriteAll(NeoPixel_LED_Context, LED_colors, sizeof(LED_colors));
	LOGOMATIC("LED Flashing\n");
	return;
}

void Neopixel_ButtonWrite() {

	// 1: TS Active, 2: RTD
	// If the button doesn't do anything, it's off
	// If the button will turn the car back a state, it's red
	// If the button is ready to be pressed, it's a magical color
	// Milliseconds since boot is a seed, multiply by 71, take last 24 bits, then that's the magical color

	button_colors[0] = COLOR_OFF;
	button_colors[1] = COLOR_OFF;

	uint32_t COLOR_MAGICAL = (i++ * 27644437) & 0x00FFFFFF;

	// Not dealing with this enum include nonsense
	switch (dashStatus.ECUState) {
		case 1: // GR_GLV_ON
			button_colors[0] = COLOR_MAGICAL;
			button_colors[1] = COLOR_WHITE;
			break;
		case 2: // GR_PRECHARGE_ENGAGED
			button_colors[0] = COLOR_RED;
			break;
		case 3: // GR_PRECHARGE_COMPLETE
			button_colors[0] = COLOR_RED;
			button_colors[1] = COLOR_MAGICAL;
			break;
		case 4: // GR_DRIVE_ACTIVE
			button_colors[0] = COLOR_RED;
			button_colors[1] = COLOR_RED;
			break;
	}

	Neopixel_WriteAll(NeoPixel_Button_Context, button_colors, sizeof(button_colors));
	LOGOMATIC("Button Flashing\nColor: %x\n", COLOR_MAGICAL);
	return;
}
