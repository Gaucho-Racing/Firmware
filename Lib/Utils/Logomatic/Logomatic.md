# Logomatic

A lightweight, header-only logging library for embedded systems that provides `printf`-style logging with automatic output routing based on the build target.

## Overview

Logomatic enables unified logging across Hardware-on-the-Loop (HOOTL) testing and embedded targets using a single API. The library automatically routes log output to the appropriate backend:

- **HOOTL builds**: Standard output (stdout) for test integration
- **Embedded targets**: ITM (Instrumentation Trace Macrocell) for SWO (Serial Wire Output) debugging

## Features

- **Header-only**: Just `#include "Logomatic.h"` - no linking required
- **Printf-compatible API**: Drop-in replacement for `printf` with identical syntax
- **Automatic target detection**: Uses ITM when available, standard library otherwise
- **Compile-time configuration**: Zero overhead when disabled
- **CMSIS-compliant**: Uses standard ARM CMSIS ITM interface
- **Weak symbol implementation**: Can be overridden for custom backends

## Usage

### Basic Example

```c
#include "Logomatic.h"

void example_function(int count, float value) {
    LOGOMATIC("Sensor reading: count=%d, value=%.2f\n", count, value);
}
```

The `LOGOMATIC()` macro expands to `printf()` when `LOGOMATIC_ENABLED` is defined, or to nothing when disabled - providing zero runtime overhead in release builds.

## Build Targets

### HOOTL Testing

In HOOTL (simulation/test) builds, logs are written to standard output:

```c
LOGOMATIC("Hello, World! I hope I %s!\n", "PASS");
// Output: Hello, World! I hope I PASS!
```

This allows integration with test frameworks and CI/CD pipelines that capture stdout.

### Embedded Hardware

On ARM Cortex-M targets with ITM support, logs are transmitted via the ITM peripheral using the Serial Wire Output (SWO) protocol:

```c
LOGOMATIC("ADC value: %d\n", adc_reading);
// Transmitted over SWO to debugger console
```

**Requirements:**

- ITM must be initialized in firmware (enable TRCENA, ITM ports, etc.)
- SWO pin connected to debug probe
- Debug probe configured to capture SWO data (see Configuration below)

**Automatic Detection:**

The library checks for `ITM` definition at compile-time. If ITM is available (defined by CMSIS headers), the custom `__io_putchar` implementation routes output to SWO. Otherwise, standard library printf is used.

## Configuration

### Build Configuration

Logomatic is automatically enabled in Debug and RelWithDebInfo builds via `GlobalShare/common.cmake`:

```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_definitions(GLOBALSHARE_LIB INTERFACE LOGOMATIC_ENABLED)
endif()
```

### Firmware Initialization

Initialize ITM in your firmware before using `LOGOMATIC()`:

```c


int main(void)
{
    Setup_Logomatic();
    LOGOMATIC("System initialized\n");
    // ...
}
```

### SWO Setup for STM32

Configure your debugger to capture SWO data on ITM Port 0:

**VS Code cortex-debug with OpenOCD:**

```json
{
    "swoConfig": {
        "enabled": true,
        "cpuFrequency": 170000000,  // Match your system clock
        "swoFrequency": 2000000,
        "source": "probe",
        "decoders": [
            { "port": 0, "type": "console", "label": "SWO:ITM" }
        ]
    }
}
```

## Implementation Details

### Output Mechanism

The library provides a weak `__io_putchar()` implementation that's called by the standard library's `printf()`.

- **HOOTL builds**: `ITM` is not defined, so standard library `__io_putchar` is used and the output is sent to stdout
- **Embedded builds**: Custom weak implementation routes characters to ITM and the output is sent over SWO
- **Weak linkage**: Can be overridden in user code for custom output backends

### Performance Characteristics

- **Overhead**: Zero when disabled (macro compiles to nothing)
- **Timing impact**: ~1-5 µs per character at 2 MHz SWO frequency
- **Buffer behavior**: ITM hardware FIFO handles buffering transparently
- **Interrupt safety**: Uses CMSIS `ITM_SendChar()` which handles port readiness
