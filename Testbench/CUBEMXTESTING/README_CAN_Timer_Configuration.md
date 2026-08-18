# CAN Timer Configuration Guide

This guide explains how to configure, modify, and extend the CubeMX-based CAN timer implementation.

The current design uses STM32CubeMX for hardware configuration and a custom CAN runtime for software queue handling.

---

# 1. Ownership Model

## CubeMX controls

CubeMX owns all hardware-level configuration:

- enabled FDCAN peripherals
- enabled timer peripherals
- FDCAN GPIO pins
- FDCAN clock source
- FDCAN bit timing
- timer clock source
- timer prescaler
- initial timer period
- NVIC interrupt enable
- NVIC interrupt priority
- generated HAL handles
- generated interrupt handlers

Examples:

```c
FDCAN_HandleTypeDef hfdcan1;
FDCAN_HandleTypeDef hfdcan2;
TIM_HandleTypeDef htim5;
```

CubeMX also generates:

```c
MX_FDCAN1_Init();
MX_FDCAN2_Init();
MX_TIM5_Init();
```

Do not manually duplicate this hardware initialization inside the custom CAN library.

## User-controlled runtime settings

The user-controlled values are stored in the generated `can_cfg.h` file inside a preserved user-code section.

Example:

```c
/* USER CODE BEGIN CAN_USER_CONFIG */

#ifndef TX_BUFFER_1_SIZE
#define TX_BUFFER_1_SIZE 20U
#endif

#ifndef TX_BUFFER_2_SIZE
#define TX_BUFFER_2_SIZE 20U
#endif

#ifndef TX_BUFFER_3_SIZE
#define TX_BUFFER_3_SIZE 20U
#endif

#ifndef CAN_TIMER_INDEX
#define CAN_TIMER_INDEX 5
#endif

#ifndef CAN_TIMER_TICK_US
#define CAN_TIMER_TICK_US 100U
#endif

#ifndef CAN_DEQUEUE_PERIOD_US
#define CAN_DEQUEUE_PERIOD_US 500U
#endif

/* USER CODE END CAN_USER_CONFIG */
```

CubeMX preserves values inside this section when code is regenerated.

---

# 2. Editable Values

## `TX_BUFFER_1_SIZE`

Controls the number of software-queued transmit messages for CAN1.

```c
#define TX_BUFFER_1_SIZE 20U
```

Example:

```c
#define TX_BUFFER_1_SIZE 50U
```

Use a larger value when CAN1 may temporarily produce messages faster than the hardware can transmit them.

A larger queue consumes more RAM.

## `TX_BUFFER_2_SIZE`

Controls the software TX queue size for CAN2.

```c
#define TX_BUFFER_2_SIZE 20U
```

## `TX_BUFFER_3_SIZE`

Controls the software TX queue size for CAN3.

```c
#define TX_BUFFER_3_SIZE 20U
```

This value is ignored when CAN3 is not enabled.

Queue sizes must be greater than zero.

---

## `CAN_TIMER_INDEX`

Selects the CubeMX-generated timer used for CAN queue servicing.

```c
#define CAN_TIMER_INDEX 5
```

This maps to:

```c
htim5
```

Examples:

```c
#define CAN_TIMER_INDEX 1
```

selects:

```c
htim1
```

```c
#define CAN_TIMER_INDEX 2
```

selects:

```c
htim2
```

```c
#define CAN_TIMER_INDEX 3
```

selects:

```c
htim3
```

The selected timer must also be enabled in CubeMX.

If the selected timer is not enabled, the generated configuration should produce a compile-time error.

---

## `CAN_TIMER_TICK_US`

Defines the duration of one timer counter tick in microseconds.

```c
#define CAN_TIMER_TICK_US 100U
```

This value must match the timer prescaler configured in CubeMX.

The equation is:

```text
Timer counter frequency =
Timer input clock / (Prescaler + 1)
```

Then:

```text
Timer tick duration =
1 / Timer counter frequency
```

For a 160 MHz timer clock and:

```c
Prescaler = 15999
```

the counter frequency is:

```text
160,000,000 / 16,000 = 10,000 Hz
```

Therefore:

```text
1 count = 100 us
```

The matching runtime value is:

```c
#define CAN_TIMER_TICK_US 100U
```

### Example: 10 us timer tick

To create a 10 us timer tick with a 160 MHz timer clock:

```text
Prescaler = 1599
```

because:

```text
160,000,000 / 1600 = 100,000 Hz
```

Therefore:

```text
1 count = 10 us
```

The matching configuration is:

```c
#define CAN_TIMER_TICK_US 10U
```

Always change the prescaler through CubeMX.

Do not manually edit the generated `tim.c` file.

---

## `CAN_DEQUEUE_PERIOD_US`

Controls how often the runtime attempts to dequeue and transmit a queued CAN message.

```c
#define CAN_DEQUEUE_PERIOD_US 500U
```

For:

```c
#define CAN_TIMER_TICK_US 100U
```

the timer uses:

```text
500 us / 100 us = 5 counts
```

The auto-reload value becomes:

```text
ARR = 5 - 1 = 4
```

The generated macros calculate this automatically:

```c
#define CAN_TIMER_PERIOD_COUNTS \
    (CAN_DEQUEUE_PERIOD_US / CAN_TIMER_TICK_US)

#define CAN_TIMER_AUTORELOAD \
    (CAN_TIMER_PERIOD_COUNTS - 1U)
```

### Example periods

With a 100 us timer tick:

| Desired dequeue period | `CAN_DEQUEUE_PERIOD_US` | ARR |
|---:|---:|---:|
| 500 us | `500U` | 4 |
| 1 ms | `1000U` | 9 |
| 2 ms | `2000U` | 19 |
| 5 ms | `5000U` | 49 |
| 10 ms | `10000U` | 99 |
| 100 ms | `100000U` | 999 |

The dequeue period must be evenly divisible by the timer tick.

Valid:

```c
#define CAN_TIMER_TICK_US 100U
#define CAN_DEQUEUE_PERIOD_US 500U
```

Invalid:

```c
#define CAN_TIMER_TICK_US 100U
#define CAN_DEQUEUE_PERIOD_US 550U
```

The second example is invalid because 550 us cannot be represented exactly using 100 us timer counts.

---

# 3. Changing the Active Timer

To change from TIM5 to TIM2:

## Step 1: Enable TIM2 in CubeMX

In CubeMX:

```text
Pinout & Configuration
    Timers
        TIM2
            Clock Source: Internal Clock
```

Configure the desired prescaler and period.

Enable:

```text
TIM2 global interrupt
```

## Step 2: Regenerate code

CubeMX should generate:

```c
TIM_HandleTypeDef htim2;
void MX_TIM2_Init(void);
```

and:

```c
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}
```

## Step 3: Change `can_cfg.h`

```c
#define CAN_TIMER_INDEX 2
```

## Step 4: Update the timer tick

Set `CAN_TIMER_TICK_US` to match TIM2's prescaler.

Example:

```c
#define CAN_TIMER_TICK_US 100U
```

## Step 5: Initialize TIM2 in `main.c`

CubeMX should generate:

```c
MX_TIM2_Init();
```

before:

```c
CAN_Timer_Start();
```

---

# 4. Using One Shared Timer

The current implementation uses one selected timer for all enabled CAN buses.

Example:

```text
TIM5
  |
  +--> CAN_Timer_Tick()
          |
          +--> service CAN1 queue
          +--> service CAN2 queue
```

This is the recommended design in most cases.

Advantages:

- fewer hardware timers used
- simpler configuration
- simpler startup
- one callback
- easier CubeMX integration
- easier synchronization between CAN buses

The callback is:

```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &CAN_TIMER_HANDLE) {
        CAN_Timer_Tick();
    }
}
```

---

# 5. Servicing Multiple CAN Buses

## Priority-based scheduling

This approach always checks CAN1 first:

```c
void CAN_Timer_Tick(void)
{
#ifdef USECAN1
    if (CAN_ProcessOneTxMessage(&can1)) {
        return;
    }
#endif

#ifdef USECAN2
    if (CAN_ProcessOneTxMessage(&can2)) {
        return;
    }
#endif

#ifdef USECAN3
    (void)CAN_ProcessOneTxMessage(&can3);
#endif
}
```

Advantages:

- simple
- supports bus priority

Disadvantage:

- lower-priority buses can be starved

## Round-robin scheduling

This approach alternates between buses:

```c
void CAN_Timer_Tick(void)
{
    static uint8_t next_bus = 0U;

#if defined(USECAN1) && defined(USECAN2)

    if (next_bus == 0U) {
        (void)CAN_ProcessOneTxMessage(&can1);
        next_bus = 1U;
    } else {
        (void)CAN_ProcessOneTxMessage(&can2);
        next_bus = 0U;
    }

#elif defined(USECAN1)

    (void)CAN_ProcessOneTxMessage(&can1);

#elif defined(USECAN2)

    (void)CAN_ProcessOneTxMessage(&can2);

#endif
}
```

Advantages:

- fairer between buses
- avoids CAN1 permanently dominating CAN2

---

# 6. Using Multiple Active Timers

The current template selects one shared timer.

It can be extended so each CAN peripheral uses a separate timer.

Example:

```text
TIM1 services CAN1
TIM2 services CAN2
TIM3 services CAN3
```

Use separate configuration values:

```c
#define CAN1_TIMER_INDEX 1
#define CAN2_TIMER_INDEX 2
#define CAN3_TIMER_INDEX 3

#define CAN1_TIMER_TICK_US 100U
#define CAN2_TIMER_TICK_US 100U
#define CAN3_TIMER_TICK_US 100U

#define CAN1_DEQUEUE_PERIOD_US 500U
#define CAN2_DEQUEUE_PERIOD_US 1000U
#define CAN3_DEQUEUE_PERIOD_US 2000U
```

Create separate timer handles:

```c
#define CAN1_TIMER_HANDLE \
    CAN_CFG_JOIN(htim, CAN1_TIMER_INDEX)

#define CAN2_TIMER_HANDLE \
    CAN_CFG_JOIN(htim, CAN2_TIMER_INDEX)

#define CAN3_TIMER_HANDLE \
    CAN_CFG_JOIN(htim, CAN3_TIMER_INDEX)
```

Create separate auto-reload values:

```c
#define CAN1_TIMER_AUTORELOAD \
    ((CAN1_DEQUEUE_PERIOD_US / CAN1_TIMER_TICK_US) - 1U)

#define CAN2_TIMER_AUTORELOAD \
    ((CAN2_DEQUEUE_PERIOD_US / CAN2_TIMER_TICK_US) - 1U)

#define CAN3_TIMER_AUTORELOAD \
    ((CAN3_DEQUEUE_PERIOD_US / CAN3_TIMER_TICK_US) - 1U)
```

The callback would route each timer separately:

```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#ifdef USECAN1
    if (htim == &CAN1_TIMER_HANDLE) {
        CAN1_Timer_Tick();
        return;
    }
#endif

#ifdef USECAN2
    if (htim == &CAN2_TIMER_HANDLE) {
        CAN2_Timer_Tick();
        return;
    }
#endif

#ifdef USECAN3
    if (htim == &CAN3_TIMER_HANDLE) {
        CAN3_Timer_Tick();
        return;
    }
#endif
}
```

Each timer must also be started separately:

```c
if (CAN1_Timer_Start() != HAL_OK) {
    Error_Handler();
}

if (CAN2_Timer_Start() != HAL_OK) {
    Error_Handler();
}

if (CAN3_Timer_Start() != HAL_OK) {
    Error_Handler();
}
```

Use multiple timers only when the buses require:

- independent transmission periods
- strict timing separation
- different interrupt priorities
- separate bandwidth policies

For most applications, one shared timer with round-robin queue servicing is simpler and more efficient.

---

# 7. Timer Startup Function

The shared-timer implementation should use:

```c
HAL_StatusTypeDef CAN_Timer_Start(void)
{
    TIM_HandleTypeDef *htim = &CAN_TIMER_HANDLE;

    __HAL_TIM_DISABLE(htim);

    __HAL_TIM_SET_AUTORELOAD(
        htim,
        CAN_TIMER_AUTORELOAD);

    __HAL_TIM_SET_COUNTER(htim, 0U);

    htim->Instance->EGR = TIM_EGR_UG;

    __HAL_TIM_CLEAR_FLAG(
        htim,
        TIM_FLAG_UPDATE);

    return HAL_TIM_Base_Start_IT(htim);
}
```

This function changes only the runtime period.

It does not configure:

- timer clocks
- timer prescaler
- NVIC
- timer instance
- timer IRQ handler

CubeMX owns those settings.

---

# 8. Required Startup Order

The hardware initialization must happen before custom runtime initialization.

Correct:

```c
MX_GPIO_Init();
MX_FDCAN1_Init();
MX_FDCAN2_Init();
MX_TIM5_Init();

GR_CAN_Init();

if (CAN_Timer_Start() != HAL_OK) {
    Error_Handler();
}
```

Incorrect:

```c
CAN_Timer_Start();
MX_TIM5_Init();
```

The timer handle is not ready until `MX_TIM5_Init()` runs.

---

# 9. Files That May Be Edited

## Edit directly

These files contain custom code:

```text
can.c
can.h
gr_can_init.c
gr_can_init.h
can_cfg_h.ftl
README
CMakeLists.txt
```

## Edit only inside user-code sections

```text
main.c
```

Examples:

```c
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
```

```c
/* USER CODE BEGIN 2 */
/* USER CODE END 2 */
```

```c
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
```

## Do not manually edit

These files are generated by CubeMX:

```text
fdcan.c
fdcan.h
tim.c
tim.h
gpio.c
gpio.h
stm32g4xx_it.c
stm32g4xx_it.h
stm32g4xx_hal_conf.h
```

Modify their settings through CubeMX and regenerate.

---

# 10. Regeneration Checklist

After regenerating CubeMX code, verify:

```text
main.c includes tim.h
main.c calls MX_TIM5_Init()
tim.c exists
tim.h exists
htim5 is declared
TIM5_IRQHandler exists
HAL_TIM_MODULE_ENABLED is defined
FDCAN1 and FDCAN2 still initialize
can_cfg.h contains USECAN1 and USECAN2
Core/Src/tim.c is included in the build
```

Also verify that custom code remains inside user-code sections.

---

# 11. Testing Changes

## Test timer frequency

Temporarily replace queue servicing with a counter:

```c
volatile uint32_t can_timer_ticks = 0U;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &CAN_TIMER_HANDLE) {
        can_timer_ticks++;
    }
}
```

With:

```c
#define CAN_DEQUEUE_PERIOD_US 500U
```

the counter should increment approximately 2,000 times per second.

For easier debugging:

```c
#define CAN_DEQUEUE_PERIOD_US 100000U
```

This creates one callback every 100 ms.

## Test CAN queue servicing

1. Initialize FDCAN.
2. Initialize the custom CAN runtime.
3. Start the timer.
4. Add one frame to the software queue.
5. Confirm `CAN_Timer_Tick()` runs.
6. Confirm one frame is removed from the queue.
7. Confirm the HAL transmit function succeeds.
8. Confirm another CAN node receives the frame.

---

# 12. Common Errors

## Selected timer is not enabled

Example:

```text
CAN_TIMER_INDEX selects TIM2, but TIM2 is not enabled in CubeMX
```

Fix:

- enable TIM2 in CubeMX, or
- select an enabled timer

## `htim5` linker error

Confirm:

```text
Core/Src/tim.c
```

is included in the build.

## No timer callback

Confirm:

- `MX_TIM5_Init()` is called
- `CAN_Timer_Start()` returns `HAL_OK`
- the timer interrupt is enabled
- `TIM5_IRQHandler()` exists
- `HAL_TIM_IRQHandler(&htim5)` is called
- global interrupts are enabled

## Wrong timer rate

Confirm:

- timer input clock
- prescaler
- `CAN_TIMER_TICK_US`
- `CAN_DEQUEUE_PERIOD_US`

All four values must agree.

## Duplicate interrupt handler

Do not define another:

```c
TIM5_IRQHandler()
```

inside the CAN library.

CubeMX owns the IRQ handler.

## Messages remain queued

Check:

- FDCAN was started
- the hardware TX FIFO has room
- another node is present to acknowledge messages
- the transceiver is enabled
- bus bit timing matches
- queue dequeue code is being called

---

# 13. Current Default Configuration

Current timer:

```text
TIM5
```

Timer input clock:

```text
160 MHz
```

Prescaler:

```text
15999
```

Timer tick:

```text
100 us
```

CAN dequeue period:

```text
500 us
```

Calculated auto-reload:

```text
4
```

Default queue sizes:

```text
CAN1: 20 messages
CAN2: 20 messages
CAN3: 20 messages
```

The current implementation uses one shared timer for all enabled CAN buses.
