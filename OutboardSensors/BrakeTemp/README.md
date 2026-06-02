# Brake Temp Sensor

Copied from [Sensor-Nodes](https://github.com/Gaucho-Racing/Sensor-Nodes/tree/main/Software/STM32G4/G431-MLX90640) from GR25

Driver* library for MLX90614 that we are using/modified:
https://github.com/melexis/mlx90614-library

*NOTE that the above library is for MLX90614xxC while we are using MLX90614xxA. The main difference here is that the EEPROM (eraseable and writable ROM) is locked in the xxC variant and NOT for the xxA. We had to adjust the driver to not send the unlock/lock commands.

-The MLX90614xxA sensor reads a single temperature value continuously upon startup. It stores the object and ambient temperatures in its RAM (which is readily readable using the driver). The sensor uses two digital filters for noise reduction (FIR and IIR), where FIR is a simple running average of N samples. We disabled IIR and used FIR=128, which has a minimum delay of 36ms (close to 30 Hz).
    -This sensor uses SMBus for error checking (PEC protocol) any data we attempt to write to it's EEPROM. To write to the EEPROM you generally have to write 0x0000 to the 0x001x_xxxx register on the device. The 0x001 part stands for EEPROM while the x_xxxx is the address of the EEPROM. After writing 0x0000, wait 10ms and then write your intended value. Wait another 10ms and powercycle. EEPROM has limited lifetime writing cycles so avoid having this in any sort of loop or consistent startup.
-Using the driver to get the object and ambient temperature is super easy since there is only one function
-This build also contains capability to measure wheel speed (rpm) by detecting rising edges coming from a Hall Effect sensor. The Hall Effect sensor detects changes in a magnetic field from a rotating gear on the wheel axel.
    -RPM is calculated by using a circular buffer to capture a history of N <# edges, time delta> pairs. A standard GPIO rising edge interrupt works just fine and produces 0.14% error at 2800 rpm. If your input was somehow MHz scale, then you must switch to an input capture timer.
    -There is also a debounce timer (which has a tunable duration) to prevent noise occuring shortly after a rising edge from incurring additional edge detection.
    -Note that there is an atomic section in the main while(1) loop that prevents edge detection from occuring while the MCU is communicating with SMBus or CAN. You can actually remove this and it will still work, but it only adds an additional 2% error at 2800 rpm (we never expect to reach beyond 1800 in standard operations, i.e. the car wheel is touching the ground).
    -Wheel speed history is reset on a timeout after not detecting any rising edges for a while.

Important changes to the MLX90614 driver to accomodate STM32 SMBus:

-The MLX90614 requires a PEC byte to accept any incoming EEPROM write data. This byte must be calculated in software because from my knowledge the STM32 SMBus automatic PEC hardware only interprets incoming PEC bytes and does not append one on outgoing messages.
    -Feel free to verify and disprove my assumption of the STM32 hardware
-Only MLX90614_SMBusRead and MLX90614_SMBusWrite are implemented by Gaucho Racing.
    -SMBusRead first transmits a read command (to either RAM or EEPROM) before quickly following up with HAL_SMBUS_Master_Receive_IT. If there is any significant delay between these operations, the temperature sensor will NACK.
        -Note that the HAL SMBus driver only uses an interrupt-based approach and does not support polling
    -SMBusWrite first transmits a write command with its data-to-be-written and a PEC byte. Then it waits 10ms before reading back from the same address to verify that the change went into effect.
