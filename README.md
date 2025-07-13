# Firmware Suite - GR26

## Branch Usage
Please use feature branches and avoid pushing directly to `main` until implementations are stable and tested.

## Quick Start

1. **Prerequisites**: Install CMake 3.25+, Ninja, ARM GCC toolchain, and VS Code (recommended)
2. **Open in VS Code**: `code .` - Project will auto-configure
3. **Build All**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Boards"
4. **Flash**: `Ctrl+Shift+P` → "Tasks: Run Task" → Select board flashing task
5. **Debug**: Use VS Code launch configurations for each board

For detailed instructions, see [BUILD.md](BUILD.md).

## System Architecture

### Shared Components
- **LVGL 9.3.0-dev**: Unified graphics library for both display panels
- **CSV ID Management**: Centralized CAN message ID definitions
- **Common Utilities**: Shared data structures and helper functions
- **ARM Toolchains**: Consistent build environments across boards
- **CMSIS Drivers**: Shared ARM Cortex-M and STM32 device drivers for all boards
- **HAL Drivers**: Shared STM32 HAL driver libraries by MCU family
  - STM32G4xx HAL drivers
  - STM32U5xx HAL drivers

## Shared vs Board-Specific Code

### Shared Components (`lib/`)
- **LVGL**: Graphics library used by both panels
- **CMSIS**: ARM Cortex-M and STM32 device drivers shared across all boards
- **utils.c/h**: Common data structures, math, utilities
- **CSV Generator**: CAN message ID definitions
- **CMake configs**: ARM toolchain setups
- **STM32CubeMX Library**: Shared STM32 HAL and middleware code
  - `lib/stm32cubemx.cmake`: Functions for creating STM32U5 and STM32G4 targets
  - `lib/cmake/cmsis-config.cmake`: Shared CMSIS configuration functions

### Board-Specific Code
- **Application/**: Business logic unique to each board
- **Core/**: STM32 HAL configuration files, system initialization
