# STM32G474 G4SPI
Nucleo-G474RE-C04

# 0. Prerequisites:
- 2 Nucleo-G474RE Discovery Boards
- 2 Micro-USB connectors to access the onboard STLink probe and provide power.  
- 4 female-to-female jumper cables.
- A little faith. 

# 1. Wiring
Wire the boards together by referring to the [MCU pin functions](PinFunctions.png) and [Nucleo Board Pinout](Pinout.png). Either board can act as transmitter or receiver with this wiring setup. 
- You only need to wire the pins that are labeled SPI. 
- You should only need to use the female jumper cables to connect male headers together. 

- The pin labeled PA5 controls the [User LED](led_location.png) on each board. 

# 2. Compilation
Give the code for both boards a quick look and try to guess what their behaviour will look like. You only need to look at the main() functions in [G4SPI_Receive/Core/Src/main.c](G4SPI_Receive/Core/Src/main.c) and [G4SPI_Transmit/Core/Src/main.c](G4SPI_Transmit/Core/Src/main.c).

To verify your predictions, you will need to flash the boards with G4SPI_Transmit.elf and G4SPI_Receive.elf. From the root directory of Firmware, run the cmake commands:
  - `cmake --preset Debug` 
  - `cmake --build build/Debug`

# 3. Flashing
Wait for the files to compile. Once you have the .elf files, you can either run openocd commands to flash your boards or use GUI tools in VSCode or from elsewhere. 

  - `openocd -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program build/Debug/G4SPI_Transmit.elf reset verify exit"`
  - `openocd -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program build/Debug/G4SPI_Receive.elf reset verify exit"`

If these commands don't work, double-check that the path to your elf files is correct.

You might have trouble if you have two boards connected at once, as openocd will usually just select the first board that is detected. A simple fix is to program one while the other is disconnected from your computer. 

If this seems a little janky to you, there is another way. You can command openocd to flash the STLink only if it has the correct serial number. To list this information, you can install the open source ![stlink tool](https://github.com/stlink-org/stlink/releases/tag/v1.7.0), then run `st-info --probe` in your command prompt to list. Once the serial numbers are obtained, run this command. Make sure to alternate the serial number and elf files (either G4SPI_Transmit.elf or G4SPI_Receive.elf).   
    - `openocd -c "adapter serial {correct serial number}" -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program build/Debug/{name}.elf reset verify exit"`

You can also use other options like the STM32CubeProgrammer tool, which includes a GUI that lets you select an STLink if there are multiple connected. 

# 4. Verification
Once both boards are flashed (using whichever method you like), plug them back into power. 
- If nothing happened right after flashing, try pressing the black reset buttons on both boards.  
- It may also help to check that your wiring is correct.
- Occasionally, there are glitches in the receiver board. These should also get resolved by hitting the reset button. 

If you think that the boards are working correctly (the User LEDs on both boards should both be blinking), see if the LED pattern matches your expectations from the code. Here are some other questions you could ask yourself:
- SPI can be used in either full-duplex or half-duplex mode. Which mode is it being used in right now?

- Out of the 4 lines, are there any that are non-critical? Remove it and see if the board still functions as expected.

- What happens when you remove a line that is critical? Again, see if this matches your expectation from the code. 

# 5. Beyond HAL
You will probably need to dive deeper than the HAL API throughout the rest of this season, and interact with registers directly.  
- Take a look at MX_SPI2_Init() inside either transmitter or receiver [main.c](G4SPI_Receive/Core/Src/main.c) file. The code in this function should correspond roughly to SPI parameters in CubeMX.

- Right-click on HAL_SPI_Init() at the end of MX_SPI2_Init(). Scroll past the assert statements until you get to the section that modifies the CR1 and CR2 registers. 
- These correspond directly to registers described in the datasheet for the STM32NucleoG474RE. Hopefully this removes the mystery behind the CubeMX magic.