# CMSIS Core (Arm CMSIS_5)

Upstream Arm CMSIS Core subset vendored for STM32 families used in the monorepo.

## Upstream Reference

Included per-family core header versions presently differ (due to ST packaging cadence):

| Family | `cmsis_version.h` Core Version | Device CMSIS Version | Source Package |
|--------|--------------------------------|----------------------|----------------|
| STM32G4xx | 5.3 | 1.2.5 | ST G4 HAL bundle |
| STM32L4xx | 5.3 | 1.7.4 | ST L4 HAL bundle |
| STM32U5xx | 5.6 | 1.4.1 | ST U5 HAL bundle |

The unified CMake interface target (`CMSIS_5_CORE`) abstracts these differences; user code selects its MCU via a single device preprocessor macro (e.g. `STM32G474xx`).

## Future (CMSIS 6)

Adopt CMSIS 6 only when ST offers official device-specific support (startup + system + HAL alignment). Plan for a transition layer to isolate direct register/system calls.
