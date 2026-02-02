# STM32G474 G4SPI

Nucleo-G474RE-C04

## 0. Prerequisites

- 2 Nucleo-G474RE Discovery Boards
- 2 Micro-USB connectors to access the onboard STLink probe and provide power.
- 4 female-to-female jumper cables.
- A little faith.

## 1. Wiring

Wire the boards together by referring to the [MCU pin functions](https://wiki.gauchoracing.com/books/onboarding-4Mh/page/w3-peripherals-workshop-images) and [Nucleo Board Pinout](https://wiki.gauchoracing.com/books/onboarding-4Mh/page/w3-peripherals-workshop-images). Either board can act as transmitter or receiver with this wiring setup.

- You only need to wire the pins that are labeled SPI.
- You should only need to use the female jumper cables to connect male headers together.
- The pin labeled PA5 controls the [User LED](https://wiki.gauchoracing.com/books/onboarding-4Mh/page/w3-peripherals-workshop-images) on each board.

## 2. Compilation

Give the code for both boards a quick look and try to guess what their behaviour will look like. You only need to look at the main() functions in [W3_G4SPI_Receive](W3_G4SPI_Receive/Core/Src/main.c) and [W3_G4SPI_Transmit](W3_G4SPI_Transmit/Core/Src/main.c).

To verify your predictions, you will need to flash the boards with G4SPI_Transmit.elf and G4SPI_Receive.elf. From the root directory of Firmware, run the cmake commands:

- `cmake --preset Debug`
- `cmake --build build/Debug`

## 3. Flashing

Wait for the files to compile. Once you have the .elf files, select the Run and Debug tab on the left-hand side of VSCode. In the window that pops up, click on the dropdown next to the green play button. and select either "SPI Receive W3 WS" or "SPI Transmit W3 WS", depending on the board you would like to flash. You may have to disconnect one board while you program the other one. If multiple boards are connected at a time, openocd may just flash the first board it detects.

## 4. Verification

Once both boards are flashed plug them both back to the USB connectors so that they both have power.

- If nothing happened right after flashing, try pressing the black reset buttons on both boards.
- It may also help to check that your wiring is correct.
- Occasionally, there are glitches in the receiver board. These should also get resolved by hitting the reset button.

If you think that the boards are working correctly (the User LEDs on both boards should both be blinking), see if the LED pattern matches your expectations from the code. Here are some other questions you could ask yourself:

- SPI can be used in either full-duplex or half-duplex mode. What do these terms mean, and which mode is it being used in right now?
- Out of the 4 wires used to connect the boards together, are there any that are non-critical? Remove it and see if the board still functions as expected.
- What happens when you remove a line that is critical? Again, see if this matches your expectation from the code.

## 5. Beyond HAL

You will probably need to dive deeper than the HAL API throughout the rest of this season, and interact with registers directly.

- Take a look at MX_SPI2_Init() inside either transmitter or receiver [main.c](W3_G4SPI_Receive/Core/Src/main.c) file. The code in this function should correspond roughly to SPI parameters in CubeMX.
- Right-click on HAL_SPI_Init() at the end of MX_SPI2_Init(). Scroll past the assert statements until you get to the section that modifies the CR1 and CR2 registers.
- These correspond directly to registers described in the datasheet for the STM32NucleoG474RE. Hopefully this removes the mystery behind the CubeMX magic.
