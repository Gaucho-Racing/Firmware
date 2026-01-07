# Tests
1. Initialize FDCAN1, then try to initialize the same instance (should error)
2. Initialize, then release immediately. Check state of GPIOs and FDCAN. Then initialize the same peripheral again
3. Initialize, add some filters until you add too many (should do something interesting)
        //


after releasing:
- State of FDCANx peripheral registers are back to reset. 
- State of GPIOx ports registers are back to reset.
- turn off RCC clock if there are no other FDCAN peripherals. 

