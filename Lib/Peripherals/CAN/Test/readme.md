# Tests
1. Initialize FDCAN1, then try to initialize the same instance (should error)
2. Initialize, then release immediately. Check state of GPIOs and FDCAN. Then initialize the same peripheral again
3. Initialize, add some filters until you add too many (should do something interesting)
4. Send some data, but try to release buffer halfway through and see if it gets logged by the TX complete callback


after releasing:
- State of FDCANx peripheral registers are back to reset. 
- State of GPIOx ports registers are back to reset.
- turn off RCC clock if there are no other FDCAN peripherals.

- can_send_helper, free a circular buffer

- discuss freeing within ISR, how to avoid. 

- Critical Section when releasing:
- 


- Add MACRO support to detect if FDCAN is a supported peripheral on the defined STM32 family
