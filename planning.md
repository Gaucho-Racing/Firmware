Contains the planning for the structure of the monorepo

- ACU
    - Application
    - Core (Configures Peripherals & instances)
- ECU
    - Application
    - Core (Configures Peripherals & instances)
- CCU
    - Application
    - Core (Configures Peripherals & instances)
- SAM
    - per sensor board
        - Application
        - Core (Configures Peripherals & instances)
- Fuse Box (E-Fuses)
    - Application
    - Core (Configures Peripherals & instances)
- Lib
    - per Chip
        - Peripheral Lib (Register Domain, per chip)
            - CAN
            - GPIO
            - ADC
            - DAC
            - SPI
            - I2C
            - Serial/Printf debug
        - Compile Resources
            - gcc-arm-none-eabi.cmake Toolchain file
            - startup_stm<chip>.s (startup assembly)
            - STM<chip>_FLASH.ld
            - <chip>.svd
        - Drivers
            - CMSIS
            - HAL/LL Driver
    - Fancy Layers (TODO: Rename)
        - NeoPixel *TBD*
        - GRCAN *HOOTL Tests*
        - SAE J1939 (for IMD & Charger) *HOOTL Tests*
        - TPL (Fancy SPI BCC/ISOSPI)
        - 
    - Utils *HOOTL Tests*
        - Queue system
        - Analyzer for da buffers
        - Bit mapping utils
        - Circular Buffer
    - Sensor Libs (Vendor provided)
        - BCC
- Autogen stuff (potentially ambitious)
    - CAN spreadsheet
   
BCC - Battery Cell Controller
MC33771 vs MC33772 - 771 is 14 cell; 772 is 7 cell
ECU & CCU shares precharge sequence, ccu extends for charge comms?
