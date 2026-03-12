# Gaucho Racing CAN API Reference

This document provides an overview of the Gaucho Racing CAN API, a simplified wrapper for STM32 FDCAN peripherals (specifically STM32G4). It handles hardware initialization, interrupt-driven message transmission with software buffering, and simplified receiver callbacks.

---

# Build Usage

Somewhere in your application, define:

```c
#define USECAN1
#define TX_BUFFER_1_SIZE <size>
```

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
