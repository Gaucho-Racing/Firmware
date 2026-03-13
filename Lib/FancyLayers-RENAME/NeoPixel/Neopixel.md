# Neopixel Usage
The Neopixel library is used for setting up the LEDs on the WS2812 Neopixel LED strip using an SPI MOSI pin.

# Configuration
Before initialization, a configuration struct of type NeopixelConfig needs to be created like so:

```
NeopixelConfig neopixelConfig1 = {.SPI_Instance = SPI1,
				  .Neopixel_Count = NEOPIXEL_LED_COUNT,
				  .MOSI_Pin = LL_GPIO_PIN_5,
				  .GPIO_AlternateFunction = Neopixel_AF5,
				  .GPIO_Port = Neopixel_GPIOB,
				  .SPI_BaudRatePrescaler = Neopixel_SPI_PS64};
```

## Configuration Parameters
- SPI_Instance: The instance of SPI being used (SPI1, SPI2, SPI3)
- Neopixel_Count: Number of LEDs
- MOSI_Pin: Pin used for GPIO and SPI (can specify pin or use bitmask)
- GPIO_AlternateFunction: For a GPIO pin to be directly connected to a peripheral. Use Neopixel_AF5 for SPI1 and SPI2, Neopixel_AF6 for SPI3.
- GPIO_Port: Port that the MOSI pin belongs to. If using bitmask, all pins should belong to the same port.
- SPI_BaudRatePrescaler: Sets SPI clock communication speed. The prescaler determines how much to divide an SCK tick by.

# Example initialization
First, initialize the configured peripherals:
```
NeopixelContext *neopixel_context_1 = Neopixel_Setup(&neopixelConfig1);
```

To be able to write colors, you must have an array of colors of type Neopixel_Color and length equivalent to the number of LEDs. Each Neopixel_Color is 24 bits (8 for red, green, blue each). In this example, we call this array ```neopixelColors1```. Then, you can use:

```
Neopixel_WriteAll(neopixel_context_1, neopixelColors1, sizeof(neopixelColors1));
```
