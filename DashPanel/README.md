# Functionality

This project contains the firmware for the Dash Panel, a series of 4 buttons, 2 buttons equipped with Neopixel LEDs, and 3 separate Neopixel LEDs that the driver will have access to (right in front of them). The 4 buttons (Buttons 1-4) correspond to control over the dash screen (dash screen not included), the 2 Neopixel buttons (TS Active and RTD Buttons) correspond to enabling TS Active and RTD modes, and the 3 Neopixel LEDs correspond to IMS (Isolation Monitoring Device), BMS (Battery Management System), BSPD (Brake System Plausibility Device).

## CAN

The Dash Panel regularly receives CAN information from the ECU (DashConfig, ECUStatus1) to update its information. In particular, Dash Panel uses these messages to keep track of the current ECU state and the statuses of IMS, BMS, and BSPD. On the other hand, the Dash Panel sends CAN messages either every 100 ms or whenever one of the buttons have been pressed (meaning new information is ready to be sent). Dash Panel only (ignoring Debug and Ping) sends the CAN message DashStatus to the ECU, which contains information on which of the 3 Neopixel LEDs are lit up and if any of 6 buttons have been pressed.

## Lights

For the 3 Neopixel LEDs, when each system is active, the corresponding LED should light up red. Otherwise, the LED should not be lit up. For the 2 LEDs on the TS Active and RTD buttons, the buttons should be a gradient color (that is, it should be changing) only if pressing that button will advance the car's state towards driving. The color will be red if pressing that button will move the car's state further from driving. The only exception is in `GLV_ON`, where the RTD button will be white (just to verify that it is on). By default, the buttons will be off.

In particular:
- In `GLV_ON`, TS Active will be gradient, and RTD will be white.
- In `PRECHARGE_ENGAGED`, TS Active will be red, and RTD will be off.
- In `PRECHARGE_COMPLETE`, TS Active will be red, and RTD will be gradient.
- In `DRIVE_ACTIVE`, TS Active will be red, and RTD will be red.

## Buttons

Pressing TS Active/RTD will either advance or further the state to/from driving depending the current state of the car.

In particular:
- In `GLV_ON`, TS Active will advance the state to `PRECHARGE_ENGAGED`, and RTD will do nothing.
- In `PRECHARGE_ENGAGED`, TS Active will move the state to `TRACTIVE_SYSTEM_DISCHARGE`, and RTD will do nothing.
- In `PRECHARGE_COMPLETE`, TS Active will move the state to `TRACTIVE_SYSTEM_DISCHARGE`, and RTD will advance the state to `DRIVE_ACTIVE`.
- In `DRIVE_ACTIVE`, TS Active will move the state to `TRACTIVE_SYSTEM_DISCHARGE`, and RTD will move the state to `TRACTIVE_SYSTEM_DISCHARGE`.

Buttons 1-4 will be used to interact with the Dash Screen. As of now, their specific usage is still undecided.

# Button mappings

> [!CAUTION]
> All buttons EXCEPT Button 4 (mapped to PC4) run through a GPIO interrupt handler. Button 4 runs by polling the pin input every tick, debounced by a state machine in software. This is due to a EXTI line conflict between PA4 and PC4, which both map to EXTI line 4, but only one pin can use the line at a time. Since PA4 is mapped to the RTD button, which is crucial to the car, we decided to "sacrifice" PC4 to the polling.
> I, Bailey Say, take full responsibility if the polling messes things up.

- TS-Active: Connected to PA3
- RTD (Ready to Drive): Connected to PA4
- Button 1: Connected to PA5
- Button 2: Connected to PA6
- Button 3: Connected to PA7
- Button 4: Connected to PC4
