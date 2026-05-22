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

---

## `FUNCTIONS DEFINITIONS`

### `TripSoftwareLatch(state_data)`

Sets the `SOFTWARE_OK_CONTROL` GPIO pin Low and sets `state_data->SOFTWARE_LATCH = false`, resulting in tripping the Emergency Shutdown Circuit.
Note: There is no software path to restore the latch to High

**Parameters:** `state_data` — `CCU_StateData*`

| Action | `SOFTWARE_LATCH` | Log |
|---|---|---|
| Sets pin Low | `false` | `"Software Latch: Low"` |

---

### `ACU_Warnings(state_data)`

Logs any active warning bits from `GRCAN_ACU_STATUS_2`. Returns `true` if any warnings are active. This function does not affect state or state transitions.

**Parameters:** `state_data` — `const CCU_StateData*`
**Returns:** boolean `true` if any warning is active

| Flag | Log output |
|---|---|
| `ACU_S2_UNDER20v_WARNING` | `"Under 20v Warning"` |
| `ACU_S2_UNDER12v_WARNING` | `"Under 12v Warning"` |
| `ACU_S2_UNDERVOLTSDC_WARNING` | `"Undervolt TSDC Warning"` |

---

### `CriticalError(state_data)`

Logs any active error bits from `GRCAN_ACU_STATUS_2`, and checks for an impossible IR latch state. Returns `true` if any error is active.

**Parameters:** `state_data` — `const CCU_StateData*`
**Returns:** boolean `true` if any error is active

| Condition | Log |
|---|---|
| `ACU_S2_OVERCURR_ERROR` | `"OVERCURR"` |
| `ACU_S2_OVERTEMP_ERROR` | `"OVERTEMP"` |
| `ACU_S2_OVERVOLT_ERROR` | `"OVERVOLT"` |
| `ACU_S2_UNDERCURR_ERROR` | `"UNDERCURR"` |
| `ACU_S2_UNDERVOLT_ERROR` | `"UNDERVOLT"` |
| `!IR_MINUS && IR_PLUS` | `"IMPOSSIBLE IR STATE"` |


### `SendPrechargeStatus(setPrecharge)`

Sends single byte CAN message ID `GRCAN_ACU_PRECHARGE`.
Note, in both states, `SendPrechargeStatus(true)` is sent only once per `recv_charge_cmd`, the flag is cleared immediately after. `STATE_IDLE` otherwise continuously sends `SendPrechargeStatus(false)` on the `PRECHARGE_SET_MSG_PERIOD_MILLIS` interval.



**Parameters:** `setPrecharge` — `bool`

| Condition | Log |
|---|---|
| `setPrecharge = True` | `Send TS Precharge status active` |
|`setPrecharge = False`| `Send TS Precharge status unactive`|


### `VCP_Oneliner(state_data)`

Transmits a compact status line over VCP and logs state bits to Logomatic. Uses a static 50-byte buffer to avoid repeated stack allocation.

**Parameters:** `state_data` — `const CCU_StateData*`

**VCP output format:**
`[<ms>] IR- <Open|Closed> | IR+ <Open|Closed> | <V>V | SOC <n>% | Max Cell <n>C | <IDLE|CHARGING>`

---

## Usage
Requires using the serial port and send:
- **`C`** — activate precharge (transitions IDLE → CHARGING, or try sending precharge again)
- **Any other key** — stop charging (transitions CHARGING → IDLE)
- **`?`** — print one-line status to VCP
