# CMSIS 5 Library

Unified, versioned import of Arm CMSIS used by all STM32 firmware targets in this monorepo.

## Version

Currently Using: [5.9.0](https://github.com/ARM-software/CMSIS_5/releases/tag/5.9.0)

## Update

Update components all together using ST Microelectronics as a reference for given chips.

### Validation Checklist

- All targets compile.
- All BLINKY projects blink.
- No new warnings.

## Migration to CMSIS 6

Migrate only when ST provides official CMSIS 6 device support (startup, system, and HAL alignment). Expect:

- Potential namespace / API shifts.
- Everything to break in interesting ways.
