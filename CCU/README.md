# STM32G474 CCU

Charging Control Unit

Compile and flash CCU.elf

FIXME For testing on a Nucleo, use HSI; otherwise use HSE whenever possible

GR26 CCU is the GR25 ECU (aka GR25 Big Bird)

## State Transitions

Two States: CCU_STATE_IDLE and CCU_STATE_CHARGING

## Initializations and Implementations

State: Initialized to CCU_STATE_IDLE
Software Latch: Initialized to True

In infinite while loop in main:
-CCU_State_Tick():
    -Checks for state transition for every "tick"
-CheckDebuggerPrint():
    -Checks boolean for VCP state data dump \\

## State Utils and State Data

## Usage

Requires using the serial port to send `C` to toggle charging and any other input to disable it. Send '?' for state data
