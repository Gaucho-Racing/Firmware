The most critical section of the car


# Some information

- Added PWM generation with TIM and DMA. This has been the most reliable way of addressing so far, but it's worth giving SPI a shot as well. 
- SPI encoding and transmission exists in fragments and needs to be updated

- APB1 and APB2 clocks are set to 40 MHz
- APB1 and APB2 Timer clocks are set to 80 MHz

- Remember that the Nucleo only outputs 3.3 V logic level, while the WS2812 requires 5V logic. A level shifter is probably needed. For smaller strips, this might not be necessary, but due to the length of cable we are using, this is likely an issue. 
- This website provides a nice [guide](https://electricfiredesign.com/2021/03/12/logic-level-shifters-for-driving-led-strips/) 

# TIM
- Every clock cycle, the timer increases its counter. When the counter reaches ARR, it resets to 0. 

- Timer clock: 80 MHz
- ARR = 100 clock cycles -> timer period = 1.25 us
- For WS2812 Bit 1, set TIM2->CCR1 = 66. 
- For Bit 0, set TIM2->CCR1 = 33. 

- uint16_t for each bit -> 1 pixel = 48 bytes per pixel 
- it may be possible to reduce this to uint8_t, but DMA might act strangely. 

# SPI
- Can adjust prescaler to achieve different SPI baud rates:
- 5 MHz -> 0.2 us per SPI bit -> 6 bits per WS2812 bit
    - 9 half-words (16 bits) is 24 WS2812 or 1 pixel 

- 2.5 MHz -> 0.4 us per SPI bit -> 3 bits per WS2812 bit
    - 9 bytes is 24 WS2812 bits or 1 pixel

- If it seems like SPI could be more reliable than TIM, it would be nice to rework the SPI encoding function to return bytes rather than half-words, if it seems like memory constraints are an issue. 

# THINGS TO EXPERIMENT WITH/TODO
- Adding a 30-220 ohm resistor in series with the level shifter to stabilize the data line
- \# of Reset Pixels
- Using higher clock frequencies might lead to a better waveform -> more stable led strip. 
- Specialized driving circuits rather than hobby logic level shifters
- Try out SPI vs TIM
- Try to reduce memory space of lighting pattern
- Clean up the code
- Characterize the timing requirements of the LED strip (is it actually 0.4/0.8 us and 1.2 us period, or is it something else!??) 