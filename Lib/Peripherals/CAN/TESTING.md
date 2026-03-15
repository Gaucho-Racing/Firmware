
# Initializing a CAN Test

This section describes how to initialize and run a CAN test using the project `CAN_STRESS_TEST` as an example.
The test entry point is implemented in `main.c`, and the project is built through the top-level `CMakeLists.txt`.

---

# 1. Enabling the Test in CMake

CAN tests are built as independent firmware projects through the helper macro:

```
add_gr_project()
```

In `CMakeLists.txt`, the stress test is enabled with:

```
add_gr_project(STM32G474xE G4CANTESTING CAN_STRESS_TEST)
```

This line instructs CMake to:

- Create a firmware target for the **STM32G474xE platform**
- Use the **G4CANTESTING project template**
- Build the firmware located in the **CAN_STRESS_TEST directory**

Other CAN test variants are defined in the same section:

```
add_gr_project(STM32G474xE G4CANTESTING CAN_EXTERNAL_TEST)
add_gr_project(STM32G474xE G4CANTESTING CAN_INTERNAL_TEST)
add_gr_project(STM32G474xE G4CANTESTING CAN_RELEASE_TEST)
add_gr_project(STM32G474xE G4CANTESTING CAN_STRESS_TEST)
```

These targets allow different test scenarios to be compiled independently.

---

# 2. Required CAN Library Inclusion

The CAN API and related utilities are included through CMake modules:

```
include("${lib_path}/Peripherals/CAN/common.cmake")
```

These modules provide:

- CAN peripheral drivers
- configuration helpers
- buffer utilities
- CAN configuration code

---

# 3. Entry Point (`main.c`)

The CAN stress test is started from `main.c`.

Key includes:

```
#include "can.h"
#include "can_tests.h"
#include "Logomatic.h"
```

These provide:

- the CAN API
- the test framework
- logging utilities

---

# 4. System Initialization

Before running the test, the firmware initializes the MCU and all required peripherals in main.c.

Typical initialization sequence:

```
HAL_Init();

SystemClock_Config();

MX_GPIO_Init();
MX_LPUART1_UART_Init();
```

This setup ensures that:

- clocks are configured
- debugging output works
- peripherals required by the test environment are initialized

---

# 5. Logging Setup

Logging is performed using the `LOGOMATIC` utility.

Example:

```
LOGOMATIC("Booted!\n");
```

This typically outputs through:

- semihosting
- SWO
- a debug UART

---

# 6. Starting the CAN Stress Test

After initialization, the test is executed directly from `main()`:

```
can_stress_test();
```

This function is defined in the CAN test framework (``can_tests.h`) and performs the following tasks:

- initializes CAN peripherals
- configures loopback or external modes
- transmits multiple test messages
- verifies message reception
- logs test results

---

# 7. Main Loop

After the test completes, the firmware enters the main loop:

```
while (1) {
    LOGOMATIC("Main Loop\n");
    LL_mDelay(1000);
}
```

This loop keeps the firmware alive and allows continued debugging output.

---

# 8. Typical CAN Test Flow

A typical CAN test function performs the following steps:

1. Configure CAN instances
2. Set the CAN clock source
3. Initialize the CAN handles
4. Start the CAN peripherals
5. Send test messages
6. Verify received messages
7. Release the CAN resources

Example flow:

```
get_cfg(FDCAN1, rx_callback1, &cfg1, FDCAN_MODE_INTERNAL_LOOPBACK);
get_cfg(FDCAN2, rx_callback2, &cfg2, FDCAN_MODE_INTERNAL_LOOPBACK);

can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

CANHandle *can1 = can_init(&cfg1);
CANHandle *can2 = can_init(&cfg2);

can_start(can1);
can_start(can2);

can_send(can1, &msg);
can_send(can2, &msg);

can_release(can1);
can_release(can2);
```

---

# 9. Summary

To initialize and run a CAN test:

1. Enable the test in `CMakeLists.txt` using `add_gr_project`.
2. Build the project using the desired CMake preset.
3. Flash the firmware containing `CAN_STRESS_TEST`.
4. The test is executed automatically from `main()` through:

```
can_stress_test();
```

5. Results and diagnostics are printed through `LOGOMATIC`.

This design allows each CAN test to run as a standalone firmware target while sharing the same CAN API implementation.



---
