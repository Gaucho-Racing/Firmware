# Button mappings

> [!CAUTION]
> All buttons EXCEPT Button 4 (mapped to PC4) run through a GPIO interrupt handler. Button 4 runs by polling the pin input every tick, debounced by a state machine in software. This is due to a EXTI line conflict between PA4 and PC4, which both map to EXTI line 4, but only one pin can use the line at a time. Since PA4 is mapped to the RTD button, which is crucial to the car, we decided to "sacrifice" PC4 to the polling.


## butt1(TSActiveButton)

TS-Active

Connected to PA3

## butt2(RTDButton)

RTD (Ready to Drive)

Connected to PA4

## D1-D4(button1-button4)

All used for possible screen navigation

## D1(button1)

Eventual Screen Navigation

Connected to __

## D2(button2)

Eventual Screen Navigation

Connected to __

## D3(button3)

Eventual Screen Navigation

Connected to __

## D4(button4)

Eventual Screen Navigation

Connected to __
