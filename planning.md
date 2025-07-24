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
- FUSE
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
            - Linkerscript
            - Toolchain file
            - startup_stm<chip>
        - Drivers
            - CMSIS
            - HAL/LL Driver
    - Fancy Layers (TODO: Rename)
        - NeoPixel
        - GRCAN
        - SAE J1939 (for IMD & Charger)
        - TPL (Fancy SPI BCC/ISOSPI)
    - Utils
        - Queue system
        - Analyzer for da buffers
        - Bit mapping utils
        - Circular Buffer
    - Sensor Libs
        - BCC
- Autogen stuff (potentially ambitious)
    - CAN spreadsheet
   
BCC - Battery Cell Controller
MC33771 vs MC33772 - 771 is 14 cell; 772 is 7 cell
ECU & CCU shares precharge sequence, ccu extends for charge comms?
