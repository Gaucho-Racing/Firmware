# STM32G474 CCU

Charging Control Unit

Compile and flash CCU.elf

FIXME For testing on a Nucleo, use HSI; otherwise use HSE whenever possible

GR26 CCU is the GR25 ECU (aka GR25 Big Bird)

## State Transitions

Two States: CCU_STATE_IDLE and CCU_STATE_CHARGING

`STATE_IDLE()`:

- Calls `BCU_Warnings()`
- Checks state_data for errors by calling `CriticalErrors()`
- If there are error(s), set `SoftwareLatch` to low (False) and `BCU_PRECHARGE_SET_TS_ACTIVE` to False
  - Sends CAN message `GR_CAN_PRECHARGE_MSG`
- If no errors and `recv_charge_cmd` set to True
  - switches state to `CCU_STATE_CHARGING` and set `BCU_PRECHARGE_SET_TS_ACTIVE` to True
  - Sends CAN message `GR_CAN_PRECHARGE_MSG`

`STATE_CHARGING()`:

- Calls `BCU_Warnings()`
- Checks state_data for errors by calling `CriticalErrors()`
- If there are error(s), set `SoftwareLatch` to lower (False) and `BCU_PRECHARGE_SET_TS_ACTIVE` to False
  - Sets state to `CCU_STATE_IDLE`
  - Sends CAN message `GR_CAN_PRECHARGE_MSG`
- If `recv_charge_cmd` set to False
  - set state to `CCU_STATE_IDLE` and `BCU_PRECHARGE_SET_TS_ACTIVE` to False
  - Sends CAN message `GR_CAN_PRECHARGE_MSG`

## Initializations and Implementations

State: Initialized to `CCU_STATE_IDLE`
Software Latch: Initialized to high (True)

In infinite while loop in main:

- `CCU_State_Tick()`:
  - Checks for state transition for every "tick"
- `CheckDebuggerPrint()`:
  - Checks boolean for VCP state data dump \\

## State Utils and State Data

`SetSoftwareLatch()`

`BCU_Warnings()`

`CriticalError()`

`CheckDebuggerPrint()`

## Usage

Requires using the serial port to send `C` to toggle charging and any other input to disable it. Send '?' for state data
