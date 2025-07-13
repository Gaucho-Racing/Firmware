# Firmware Suite - GR26

## Quick Start

1. **Prerequisites**: Install CMake 3.25+, Ninja, ARM GCC toolchain, and VS Code (recommended)
2. **Open in VS Code**: `code .` - Project will auto-configure
3. **Build All**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Boards"
4. **Flash**: `Ctrl+Shift+P` → "Tasks: Run Task" → Select board flashing task
5. **Debug**: Use VS Code launch configurations for each board

For detailed instructions, see [BUILD.md](BUILD.md).

## Development

- **Building**: Modern CMake with ExternalProject for incremental builds → [BUILD.md](BUILD.md)
- **Debugging**: ST-Link/OpenOCD with VS Code Cortex Debug → [DEBUGGING.md](DEBUGGING.md)
- **Extending**: Branch-based development, see feature development guide → [EXTENDING.md](EXTENDING.md)

## System Architecture

### Shared Components
- **LVGL 9.3.0-dev**: Unified graphics library for both display panels
- **CSV ID Management**: Centralized CAN message ID definitions
- **Common Utilities**: Shared data structures and helper functions
- **ARM Toolchains**: Consistent build environments across boards
- **CMSIS Drivers**: Shared ARM Cortex-M and STM32 device drivers for all boards
- **HAL Drivers**: Shared STM32 HAL driver libraries by MCU family
  - STM32G4xx HAL drivers for ECU
  - STM32U5xx HAL drivers for DashPanel and SteeringPanel

### Communication
- **CAN Bus**: Primary communication between ECU and panels
- **SWO Debug**: Real-time debug output from all boards
- **USB**: Programming and debug interface

## Branch Usage

Please use feature branches and avoid pushing directly to `main` until implementations are stable and tested.
