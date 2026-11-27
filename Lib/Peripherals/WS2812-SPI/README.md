# WS2812 (NeoPixel) Library

> [!NOTE]
> Currently only supports 3 SPI bits per NeoPixel bit.
>
> If support is desired for another please reach out to the Firmware manager.

TODO

## Quickstart

### CubeMX

Set SPI mode to transmit-only master, 8-bit data size, MSB first

Baud rate should be prescaled to match the encoding, ~2.4-3 MHz for 3 SPI bits per NoePixel bit

If using DMA, add a `SPIx_TX` DMA request in normal mode, not circular, memory to peripheral, byte data width; double check the DMA and SPI global interrupts get enabled

TODO

### Usage

TODO

## Internals

TODO

## WS2812 (NeoPixel) SPI

TODO

### Timing Diagram

TODO

## Advanced

### Assertions

TODO

### Interrupt Safety

TODO

### Globals

TODO

## Troubleshooting

TODO

As issues are found we will extend this section. For now, please contact the Firmware Manager if you are unable to get this working.
