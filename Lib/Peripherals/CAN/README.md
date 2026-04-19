# Gaucho Racing CAN API Reference

This document provides an overview of the Gaucho Racing CAN API, a simplified wrapper for STM32 FDCAN peripherals (specifically STM32G4). It handles hardware initialization, bus-off recovery, interrupt-driven message transmission with software buffering, and simplified receiver callbacks.

---

# Build Usage

In your application*, add a file called `can_cfg.h`, containing at least:

```c
#ifndef CAN_CFG_H
#define CAN_CFG_H

//#define USEDMA //unsafe atm

#define USECAN1
#define TX_BUFFER_1_SIZE 10

//not using
//#define CAN1_RX_GPIO_PORT
//#define CAN1_RX_GPIO_PIN
//#define CAN1_TX_GPIO_PORT
//#define CAN1_TX_GPIO_PIN

//#define CAN

#define USECAN2
#define TX_BUFFER_2_SIZE 10

// ... potentially other USECANM + TX_BUFFER_M_SIZE defines...

#endif


```

for each CAN peripheral you want to use.

\* Make sure `can_cfg.h` is in a folder included by `target_include_directories` in the project's `CMakeLists.txt`!


---

# 1. Quick Start Workflow

1. **Set Clock Source**
   Call `can_set_clksource` before any other operation.

2. **Define Callback**
   Create a function to handle incoming messages.

3. **Configure**
   Use `get_cfg` helper or fill a `CANConfig` struct.

4. **Initialize**
   Call `can_init`.

5. **Start**
   Call `can_start` to enable the peripheral and interrupts.

6. **Communicate**
   Use `can_send` to transmit data.

---

# 2. Global Configuration

## `can_set_clksource`

Sets the FDCAN kernel clock, which must be common across all CAN instances.

**Parameter**

```c
uint32_t clksource
```

Example:

```c
LL_RCC_FDCAN_CLKSOURCE_PCLK1
```

**Usage**

Must be called once before `can_init`.

---

# 3. Initialization and Setup

## `CANConfig` Structure

This structure contains all parameters required to bridge the Gaucho API with the underlying STM32 HAL.

| Field | Description |
|------|-------------|
| `fdcan_instance` | Base address (e.g., `FDCAN1`, `FDCAN2`). |
| `hal_fdcan_init` | Standard HAL `FDCAN_InitTypeDef` struct (baud rate, etc). |
| `rx_callback` | Your custom handler for received messages. |
| `rx_interrupt_priority` | Priority for RX interrupts (0–15). |
| `tx_interrupt_priority` | Priority for TX interrupts (0–15). |
| `rx_gpio` / `tx_gpio` | GPIO Port and Init structures for CAN pins. |

---

## Configuration Helper

Instead of filling the `CANConfig` manually, use the helper function in `can_cfg_helpers.c`:

```c
int get_cfg(
    FDCAN_GlobalTypeDef *instance,
    CAN_RXCallback callback,
    CANConfig *out_cfg,
    uint32_t Mode
);
```

---

# 4. Example Usage

## Define RX Callback

```c
void on_receive(uint32_t ID, void *data, uint32_t size)
{
    uint8_t my_data[64];

    memcpy(my_data, data, size);

    // Process my_data...
}
```

---

## Initialize CAN

```c
can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

CANConfig my_cfg;

get_cfg(FDCAN1, on_receive, &my_cfg, FDCAN_MODE_NORMAL);

CANHandle *h1 = can_init(&my_cfg);
```

---

## Start and Send

```c
can_start(h1);

FDCANTxMessage msg = { ... }; // Fill header and data

can_send(h1, &msg);
```

---

# 5. Callback Definition

```c
void MyCAN_RxCallback(uint32_t ID, void *data, uint32_t size);
```

---

# 6. Communication

## `can_send`

Sends a message using a `FDCANTxMessage` struct.

**Behavior**

If the hardware FIFO is full, the message is automatically queued in a software buffer.

**Returns**

```
CAN_SUCCESS
CAN_ERROR
```

---

## `can_add_filter`

Adds a hardware-level filter to the instance.

**Constraint**

Must be called while the peripheral is initialized but **not yet started**.


---

# 7. Recovery Behavior

The CAN peripheral includes automatic TX-path recovery attempts when the controller enters restricted operation mode (such as after temporary bus faults/disconnect events).

## Recovery Triggers

Recovery is attempted from:

- `can_send` (before direct Tx enqueue)
- `HAL_FDCAN_ErrorStatusCallback` (on error-status interrupt events)
- `HAL_FDCAN_RxFifo0Callback` (opportunistic recovery on Rx activity)

## Recovery Strategy

- If controller is **not** in restricted mode, normal TX flow continues.
- If in restricted mode, protocol status is checked.
- If **Bus-Off** is active, recovery is deferred (controller is not forced out).
- If Bus-Off is clear, `HAL_FDCAN_ExitRestrictedOperationMode` is attempted.
- On successful exit, queued software TX messages are dequeued back to HW TX FIFO.

## Optional Periodic Recovery Timer

An optional compile-time timer path can periodically trigger recovery attempts, even when no RX/TX/error callback activity is occurring.

Define in `can_cfg.h`:

```c
#define CAN_ENABLE_RECOVERY_TIMER
#define CAN_RECOVERY_TIMER_USE_TIM7
#define CAN_RECOVERY_TIMER_PERIOD_MS 1000U
```

Constraints:

- `CAN_RECOVERY_TIMER_PERIOD_MS` must be at least `100` ms.
- TIM7 ownership should be considered reserved when this feature is enabled.

Boot/runtime safety notes:

- TIM7 counter starts from `can_start` (not global boot init).
- Recovery tick logic only acts on CAN handles that are both initialized and started.

# Implementation Notes and Constraints

- `RX Callback` must perform a **deep copy** of the data supplied to it.
- Allocating memory using `malloc` inside ISRs is **not safe**.
- `can.h` expects `STM32G4` to be defined as a compiler definition.
- Platform usage flags may be **hardcoded through compiler definitions**.
- GPIOs should **not be disabled in MSP layers during release**, as they may affect other peripherals.

---

# Known Issues / Current Problems

- ISRs may take **too long to resolve** due to popping and freeing the circular buffer.

---

# Ideas and Future Improvements

- DMA support for copying **64 bytes from circular buffer**
- Abstract support for **different STM32 families** besides STM32G4
- RX buffering support
- TX buffering policy improvements
  - Possibly distribute messages across multiple TX buffers
- DMA support for copying from circular buffer
  - Circular buffer could then be **stack allocated**
- Smaller CAN headers for TX/RX
  - Currently uses `TXHeaderTypeDef`
- TX FIFO vs Queue policy
  - Possibly only allow **FIFO**
- Add support for **RXFifo1**

---
