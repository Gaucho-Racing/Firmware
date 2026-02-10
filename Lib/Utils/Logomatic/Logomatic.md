# Logomatic

## What it does

- **HOOTL builds**: Writes to stdout (tests/CI can capture output)
- **Embedded targets**: Uses ITM over SWO when CMSIS ITM and the LL drivers are available
- **Release builds**: Compiles out entirely (no runtime cost)

## Quick use

```c
#include "Logomatic.h"

void example_function(int count, float value)
{
    LOGOMATIC("Sensor reading: count=%d, value=%.2f\n", count, value);
}
```

## Logomatic Initialization

When ITM and LL GPIO are available, `Setup_Logomatic()` expects a `LogomaticConfig` that describes the LPUART1 setup, on a STM32G474xE for LPUART1 this could look like:

```c
LogomaticConfig logomaticConfig = {
    .clock_source = LOGOMATIC_SYSCLK,
    .bus = LOGOMATIC_BUS,
    .gpio_port = LOGOMATIC_GPIOA,
    .gpio_pin_rx_tx_mask = LL_GPIO_PIN_2 | LL_GPIO_PIN_3,
    .baud_rate = 115200,
    .data_width = LOGOMATIC_DATAWIDTH_8B,
    .stop_bits = LOGOMATIC_STOPBITS_1,
    .parity = LOGOMATIC_PARITY_NONE,
    .transfer_direction = LOGOMATIC_DIRECTION_TX_RX,
    .hardware_flow_control = LOGOMATIC_HWCONTROL_NONE,
    .prescaler = LOGOMATIC_PRESCALER_DIV1,
    .tx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8,
    .rx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8,
};

int main(void)
{
    // ...
    Setup_Logomatic(&logomaticConfig);
    // ...
    SystemClock_Config();
    // ...
    LOGOMATIC("System initialized\n");
    // ...
}
```

## What ITM is

ITM (Instrumentation Trace Macrocell) is an on-chip debug trace block on ARM Cortex-M. It can stream characters over the SWO pin to your debugger. Logomatic uses CMSIS `ITM_SendChar()` and enables ITM port 0 in `Setup_Logomatic()`.

## VS Code cortex-debug SWO setup

Use cortex-debug with SWO enabled so the debugger can show ITM output:

```json
{
    "swoConfig": {
        "enabled": true,
        "cpuFrequency": 170000000,
        "swoFrequency": 2000000,
        "source": "probe",
        "decoders": [
            { "port": 0, "type": "console", "label": "SWO:ITM" }
        ]
    }
}
```

## How output is routed

- With `LOGOMATIC_ENABLED` and `ITM` defined, Logomatic provides a weak `__io_putchar()` that writes to ITM.
- Without `ITM`, `printf()` uses the normal C library path (stdout in HOOTL or your platform retarget).
- The weak symbol can be overridden for a custom backend.
