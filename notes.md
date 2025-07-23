BCC - Battery Cell Controller
MC33771 vs MC33772 - 771 is 14 cell; 772 is 7 cell

- ACU
    - 
- ECU
    - 
- CCU
    - 
- Lib
    - Peripheral Lib (Register Domain)
        - CAN
        - GPIO
        - ADC
        - DAC
        - SPI
        - I2C
        - Serial
        - Printf debug
    - Fancy Layers (TODO: Rename)
        - NeoPixel
        - GRCAN
        - SAE J1939 (for IMD & Charger)
        - TPL (Fancy SPI BCC/ISOSPI)
    - Utils
        - queue system
        - analyzer for da buffers
    - Sensor Libs
        - BCC
- Autogen stuff (potentially ambitious)
    - CAN spreadsheet
        
ECU & CCU shares precharge sequence, ccu extends for charge comms?
