Contains the planning for the structure of the monorepo
Ad- BLINKY
    - Application
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
    - cmake
        - gcc-arm-none-eabi.cmake
    - Platform
        - CMakeLists.txt
        - [chip]
            - Peripheral Lib (Register Domain, per chip)
                - CAN
                - GPIO
                - ADC
                - DAC
                - SPI
                - I2C
                - Serial/Printf debug
            - CompileDependencies
                - startup_stm[chip].s
                    <!-- Startup assembly -->
                - STM[chip]_FLASH.ld
                - [chip].svd
                - [chip].cmake
                    <!--
                    creates a function for setting the specific executable target to be compiled for said chip
                    said function should act like add_executable but have the Driver library automatically linked
                    -->
            - Drivers
                - CMSIS
                - HAL/LL Driver (TODO: currently named stm32-hal-driver as that's what stm calls it, need to rename to make it fit better)
            - CMakeLists.txt
                <!--
                should compile the Drivers into an interface lib 
                -->
                
    - Fancy Layers (TODO: Rename)
        - NeoPixel *TBD*
        - GRCAN *HOOTL Tests*
        - SAE J1939 (for IMD & Charger) *HOOTL Tests*
        - TPL (Fancy SPI BCC/ISOSPI)
    - Utils *HOOTL Tests*
        - Queue system
        - Analyzer for da buffers
        - Bit mapping utils
        - Circular Buffer
    - Vendor Libs (Sensor libs)
        - BCC
- Autogen stuff (potentially ambitious)
    - CAN spreadsheet
   
BCC - Battery Cell Controller
MC33771 vs MC33772 - 771 is 14 cell; 772 is 7 cell
ECU & CCU shares precharge sequence, ccu extends for charge comms?

Presets:
Configure will control which architecture (x86 or whatever STM chip) to use, build presets for the type of release (Debug, minSize, ...)

TODO:
- Move componets from ACU into new infastructure
- Add required cmake infra for building ACU
- Analyze ACU code and scavenge for stuff to put in Lib
- Add in ECU to test that we can reuse the Drivers/CompileDependencies
- Basically whatever to allow Andrey to start on SAM dev
- Setup basic HOOTL testing infra inside of each level of Lib
- Setup GitHub actions for CTests
- Setup VS Code intellisense and syntax highlighting
- Setup VS Code for flashing specific boards (cortex-debug | st-utils | st-link)
- Look into J-Links in addition to ST-Links
- Docker container for onboarding and github actions
- Figure out how to track versions in HAL driver

- ...
- Profit
