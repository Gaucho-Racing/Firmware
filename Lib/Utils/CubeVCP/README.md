# Cube Virtual COM Port

Support for STM32CubeMX and VCP. See [`CUBEMX.md`](/CUBEMX.md) for background.

## Summary

CubeVCP provides interrupt-driven UART transmission and line-oriented reception.

> [!IMPORTANT]
> You must keep your Rx callback performant, it is a blocking operation within an ISR
>
> It is recommended that you keep all Tx/Rx messages short and abbreviated!

## Setup

### 0. CubeMX

Open `LPUART1` and enable the peripheral and configure the following options

| Label | Option |
| --- | --- |
| Mode | Asynchronous |
| Hardware Flow Control (RS485) | Disabled |
| Hardware Flow Control (RS485) | FALSE / unchecked |
| Baud Rate | (As desired) |
| Word Length | 8 bits (including Parity) |
| Parity | None |
| Stop Bits | 1 |
| Data Direction | Receive and Transmit |
| Single Sample | Disable |
| FIFO Mode | FIFO mode disable |

Make sure that the `LPUART1` Tx and Rx pins are correctly wired into the STLink, not all boards have this physically wired

### 1. CMake

Add `CUBE_VCP_LIB` as an interface target link library within your project.

### 2. Configuration

Add a file named `CubeVCP_Config.h` to your project, generally `Application/Inc/CubeVCP_Config.h`, that looks something like:

```c
#ifndef CUBEVCP_TEST_CONFIG_H
#define CUBEVCP_TEST_CONFIG_H

#define CUBE_VCP_TX_BUFFER_SIZE 16
#define CUBE_VCP_RX_BUFFER_SIZE 64

#endif
```

#### Configuration Requirements

- `CUBE_VCP_TX_BUFFER_SIZE` must be a power of two, acts as a circular buffer on the queue to transmit
- `CUBE_VCP_RX_BUFFER_SIZE` should be sufficiently large to handle receiving longer messages without breaking into different callbacks

### 3. Entrance

Make sure to `#include "CubeVCP.h"` where needed. Do not include `PrivateInc/internal.h` outside of the peripheral source files.

Within `main` after `MX_LPUART1_Init()` and Logomatic are setup (see [Logomatic](/Lib/Utils/Logomatic/README.md)), call `CubeVCP_Entrance()` and check the return value for any errors.

If you wish to configure receiving messages then call `CubeVCP_EnableRx()` afterwards and check the return value

### 4. Operation

#### Transmission

Call `CubeVCP_SendString()` to queue a string for sending over the configured serial port.

Messages are queued in a lock-free ring buffer and processed in FIFO order.

#### Reception

Your provided callback `CubeVCP_Rx_Callback()` is called for received strings ending in a newline or for messages that fill the Rx queue.

The peripheral attempts to flip between two queues to prevent losing data, but this is not foolproof and you should

- Configure your Rx queue to be long enough to not cut apart different commands
- Keep all Rx messages short
- Keep your callback function quick

#### Architecture

TODO MERMAID DIAGRAM

## Advanced

### Interrupt Safety

CubeVCP is designed to handle concurrent access from multiple execution contexts (main loop and ISR).

#### Overview

- `vcp_ring_head` and `vcp_ring_tail` are atomic `uint16_t` values used by the lock-free Tx ring buffer
- `CubeVCP_SendString()` writes bytes into the Tx queue and starts a UART transfer only when the Tx state is idle
- `HAL_UART_TxCpltCallback()` advances the tail and re-arms transmission from ISR context
- `HAL_UART_RxCpltCallback()` copies incoming bytes into the Rx buffer, flushes on newline or buffer full, and calls the user callback once per complete message

#### Critical Sections

Interrupt protection is used only in the receive path while the ISR copies data into the staging buffer before invoking the user callback.

- `HAL_UART_RxCpltCallback()` wraps the staging-copy and index reset in `CRITICAL_SECTION`
- The Tx path relies on atomic head/tail updates instead of blocking interrupts for normal queue operations

#### ISR Interactions

- Safe to call `CubeVCP_SendString()` from `main()` or any ISR
- Safe to call `CubeVCP_EnableRx()` from the main thread before the UART is actively running
- Do not call any CubeVCP function from inside `CubeVCP_Rx_Callback()`, keep the callback short and non-blocking
- The Rx callback runs in interrupt context, so do not perform long blocking operations or time-consuming processing there

### Assertions

We currently provide compile time static assertions to validate:

- `ATOMIC_BOOL_LOCK_FREE` and `ATOMIC_SHORT_LOCK_FREE` are available for lock-free operations
- `CUBE_VCP_TX_BUFFER_SIZE` is non-zero and a power of two
- `CUBE_VCP_RX_BUFFER_SIZE` is greater than 8 bytes so line-oriented input remains useful

### Globals

CubeVCP maintains a small private global state for the UART handle, callback, and Tx/Rx queues.

- `vcp_uart_handle` and `vcp_rx_callback` hold the active UART and user RX callback
- `vcp_tx_buffer`, `vcp_ring_head`, `vcp_ring_tail`, and `vcp_tx_active` implement the interrupt-safe TX ring buffer
- `vcp_rx_byte`, `vcp_rx_index`, `vcp_rx_buffer`, and `vcp_callback_staging` hold the receive state and staging buffer for newline-oriented callback delivery

### Configuring Other Serial Peripherals

CubeVCP only owns the UART you configure for VCP. Other serial peripherals should continue to use the normal STM32 HAL callbacks such as `HAL_UART_TxCpltCallback()` and `HAL_UART_RxCpltCallback()`.

When a callback comes from a different UART instance, CubeVCP checks the weak hook pointers `vcp_uart_tx_cplt_callback_other` and `vcp_uart_rx_cplt_callback_other` and forwards the event there if they are set. This keeps your VCP logic isolated while allowing other UARTs to keep their own handling.

Keep those other callbacks short and non-blocking. Do not call any CubeVCP function from inside a HAL UART callback, and do not perform long work in an ISR.

## Troubleshooting

TODO

As issues are found we will extend this section. For now, please contact the Firmware Manager if you are unable to get this working.
