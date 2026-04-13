> [!WARNING]
> This library is not safe to use inside of ISRs (but it should not be used there anyway)!

# Neopixel Usage
The Neopixel library is used for setting up the LEDs on the WS2812 Neopixel LED strip using an SPI MOSI pin.

## Configuration

Write the maximum number of LEDs (in a single bus) you plan to use into your `main.h`
```c
#define GR_NEOPIXEL_MAX_LEDS (42)
```

Before initialization, a configuration struct of type NeopixelConfig needs to be created like so:

```c
NeopixelConfig neopixelConfig = {.SPI_Instance = SPI1,
				  .Neopixel_Count = 3,
				  .MOSI_Pin = LL_GPIO_PIN_5,
				  .GPIO_AlternateFunction = Neopixel_AF5,
				  .GPIO_Port = Neopixel_GPIOB,
				  .SPI_BaudRatePrescaler = Neopixel_SPI_PS64};
```

### Configuration Parameters
- `SPI_Instance`: The instance of SPI being used (`SPI1`, `SPI2`, `SPI3`)
- `Neopixel_Count`: Number of LEDs, must be less than `GR_NEOPIXEL_MAX_LEDS`
- `MOSI_Pin`: Pin used for GPIO and SPI (can specify pin or use bitmask)
- `GPIO_AlternateFunction`: For a GPIO pin to be directly connected to a peripheral.
- `GPIO_Port`: Port that the MOSI pin belongs to. If using bitmask, all pins should belong to the same port.
- `SPI_BaudRatePrescaler`: Sets SPI clock communication speed. The prescaler determines how much to divide an SCK tick by.

## Example
Zeroth, define in your header the `GR_NEOPIXEL_MAX_LEDS` value for your board in your `main.h`:
```c
#define GR_NEOPIXEL_MAX_LEDS (42)
```

First, initialize the configured peripherals:
```c
NeopixelContext neopixel_context = {0};
Neopixel_Setup(&neopixelConfig, &neopixel_context);
```

To be able to write colors, you must have an array of colors of type Neopixel_Color and length equivalent to the number of LEDs. Each Neopixel_Color is 24 bits (8 for red, green, blue each). In this example, we call this array ```neopixelColors```. Then, you can use:

```c
Neopixel_WriteAll(neopixel_context, neopixelColors, sizeof(neopixelColors));
```
