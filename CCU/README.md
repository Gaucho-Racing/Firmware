# STM32G474 CCU

Charging Control Unit

Compile and flash CCU.elf

GR26 CCU is the GR25 ECU (aka GR25 Big Bird)

## State Transitions

Two States: CCU_STATE_IDLE and CCU_STATE_CHARGING

`STATE_IDLE()`:

- Calls `ACU_Warnings()`
- Checks state_data for errors by calling `CriticalErrors()`
- If there are error(s), set `SoftwareLatch` to low (False) and `ACU_PRECHARGE_SET_TS_ACTIVE` to False
  - Sends CAN message `GR_CAN_PRECHARGE_MSG`
- If no errors and `recv_charge_cmd` set to True
  - switches state to `CCU_STATE_CHARGING` and set `ACU_PRECHARGE_SET_TS_ACTIVE` to True
  - Sends CAN message `GR_CAN_PRECHARGE_MSG`

`STATE_CHARGING()`:

- Calls `ACU_Warnings()`
- Checks state_data for errors by calling `CriticalErrors()`
- If there are error(s), set `SoftwareLatch` to low (False) and `ACU_PRECHARGE_SET_TS_ACTIVE` to False
  - Sets state to `CCU_STATE_IDLE`
  - Sends CAN message `GR_CAN_PRECHARGE_MSG`
- If `recv_charge_cmd` set to False
  - set state to `CCU_STATE_IDLE` and `ACU_PRECHARGE_SET_TS_ACTIVE` to False
  - Sends CAN message `GR_CAN_PRECHARGE_MSG`

## Initializations and Implementations

State_Data: Stores data given from 'GR_CAN_ACU_STATUS_2'


CCU State: Initialized to `CCU_STATE_IDLE`
Software Latch: Initialized to high (True)

In main infinite while loop:

- `CCU_State_Tick()`:
  - Checks for state transition for every "tick"
- `CheckDebuggerPrint()`:
  - Dumps a full log of the current CCU state, including errors and warnings, when a print request flag is set to True.

## State Utils and State Data

`SetSoftwareLatch()`

- Purpose: Controls hardware GPIO Pins that act as a software-controlled latch, while keeping the 'state_data' in sync. If any critical errors occur, 'SetSoftwareLatch()' is tripped and set to low, and Emergency Shutdown Circuit is also tripped.

- Parameters:
  - state: boolean value, desired latch state
    - True: drive pin High
    - False: drive pin Low
  - 'state_data': const ptr of CCU_StateData
- Behaviour:

| `state` | Current Pin | Action | `ACU_S2_SOFTWARE_LATCH` | Log Output |
| --- | --- | --- | --- | --- |
| `true` | Low | Pin driven HIGH | `true` | `"Software Latch: High"` |
| `false` | High | Pin driven LOW | `false` | `"Software Latch: Low"` |

`ACU_Warnings()`

- Purpose: logs if any `GR_CAN_ACU_STATUS_2` warning bits are true
- Parameters:
  - state_data: const pointer to `state_data`
- Behavior: does not affect state data or state transitions

`CriticalError()`

- Purpose: logs if any `GR_CAN_ACU_STATUS_2` error bits are true
- Parameters:
  - state_data: const pointer to `state_data`
- Behavior: function returns boolean, does not affect state_data

`CheckDebuggerPrint()`

- Purpose: logs all information in `state_data` if extern boolean `request_print_statedata` is True

- Parameters:
  - state_data: const pointer to `state_data`,
- Behavior:

| `request_print_statedata` | Action |
| --- | --- |
| `true` | Logs All 'state_data', then 'request_print_statedata' set to false|
| `false` | Does nothing |


### VCP

- Receives/Transmits data with USART (receives user input)
- If the data received is `C` and `recv_charge_cmd` is False:
  - Change `recv_charge_cmd` to True (used in State Transitions)
  - Transmit `C`
- If data received is `?`:
  - Change `request_print_statedata` to True (used in `CheckDebuggerPrint()`)
  - Transmit `?`
- Else:
  - Change `recv_charge_cmd` to False (used in State Transitions)
  - Tranmist `X`

## Usage

Requires using the serial port to send `C` to toggle charging and any other input to disable it. Send '?' for state data
