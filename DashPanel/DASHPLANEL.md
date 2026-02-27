# DASH PLANel

- Update the README

## Notes:

- Torque, Current, Speed, or put battery before drive

- Need ecu state
- BMS, IMD, BSPD (off or red)
- TS Active, RTD buttons (light up according to state, multiple colors or flashing or whatever)

## CAN

- Test ping

- List of messages we need to listen to
  - ECU Status 1 (ECU Status)
  - Dash Config
  - Ping

- List of messages we need to send
  - Dash Status (if negative, then pressed)
  - Ping

## LED

1. Integrate NeoPixel
2. Set up lights

## Testing

1. Implement LP-UART for async & LOGOMATIC
