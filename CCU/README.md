# STM32G474 CCU — Charging Control Unit

Compile and flash `CCU.elf`.

GR26 CCU is the GR25 ECU (aka GR25 Big Bird).



## Overview

The CCU manages two states — `CCU_STATE_IDLE` and `CCU_STATE_CHARGING` — and controls the TS precharge over CAN via `SendPrechargeStatus()`.

**Initial conditions:**
- CCU state: `CCU_STATE_IDLE`
- Software latch: High (True)



## State Machine

### `CCU_STATE_IDLE`

Each tick:
1. If `PRECHARGE_SET_MSG_PERIOD_MILLIS` = 20ms has elapsed since last send, calls `SendPrechargeStatus(false)`
2. Calls `ACU_Warnings()`
3. Calls `CriticalError()`, checking `state_data`

| Condition | Action |
|---|---|
| Error detected | Calls `TripSoftwareLatch()` |
| No errors and `recv_charge_cmd == True` | Calls `SendPrechargeStatus(true)`, clears `recv_charge_cmd`, transitions to `CCU_STATE_CHARGING` |

### `CCU_STATE_CHARGING`

Each tick:
1. Calls `ACU_Warnings()`
2. Checks `recv_stop_cmd`, `CriticalError()`, IR latch state, and `recv_charge_cmd` in order

| Condition | Action |
|---|---|
| `recv_stop_cmd == True` | Clears `recv_stop_cmd`, transitions to `CCU_STATE_IDLE` |
| Error detected | Calls `TripSoftwareLatch()`, transitions to `CCU_STATE_IDLE` |
| `IR_MINUS && IR_PLUS` (charging complete) | Transitions to `CCU_STATE_IDLE` |
| `recv_charge_cmd == True` | Calls `SendPrechargeStatus(true)`, clears `recv_charge_cmd`, stays in `CCU_STATE_CHARGING`|
