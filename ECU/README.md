# STM32G474 ECU

Electronics Control Unit

Compile and flash ECU.elf

FIXME For testing on a Nucleo, use HSI; otherwise use HSE whenever possible

TCM Messages:
- `TS-Runwy`: Tractive System voltage above safe limit in GLV ON state.
- `TS-P-ITR`: Tractive System Precharge process interrupted.
- `HV-CritE`: Critical Error happened in Precharge Complete state.
- `DA-CritE`: Critical Error happened in Drive Active state.
- `TS-D-TLE`: Tractive System Discharge fails to discharge in time.
