# Gaucho Racing Firmware Suite - Season 26

An implementation of all GR26 low voltage firmware in mega-repo format. This repository contains firmware for three embedded systems that work together to control the Gaucho Racing electric vehicle.

## Project Overview

This firmware suite manages critical vehicle systems including power management, data acquisition, display interfaces, and safety controls for the GR26 electric race car.

## Project Structure

```
Firmware-26/
├── ECU/                    # Engine Control Unit (STM32G4)
│   ├── Application/        # ECU-specific application code
│   └── Core/              # STM32 HAL and system files
├── DashPanel/             # Dashboard Panel (STM32U5 + LVGL)
│   ├── Application/        # Dashboard UI and logic
│   ├── TouchGFX/          # UI framework files
│   └── Core/              # STM32 HAL and system files  
├── SteeringPanel/         # Steering Wheel Panel (STM32U5 + LVGL)
│   ├── Application/        # Steering wheel UI and controls
│   └── Core/              # STM32 HAL and system files
└── lib/                   # Shared libraries and utilities
    ├── LVGL/              # Shared LVGL graphics library
    ├── CSVGenerator/      # CAN message ID management
    ├── cmake/             # ARM toolchain configurations
    └── utils.c/h          # Common utility functions
```

## Quick Start

1. **Prerequisites**: Install CMake 3.25+, Ninja, ARM GCC toolchain, and VS Code (recommended)
2. **Open in VS Code**: `code .` - Project will auto-configure
3. **Build All**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Boards"
4. **Flash**: `Ctrl+Shift+P` → "Tasks: Run Task" → Select board flashing task
5. **Debug**: Use VS Code launch configurations for each board

For detailed instructions, see [BUILD.md](BUILD.md).

## Firmware Boards

### ECU (Engine Control Unit)
- **Target**: STM32G474RE 
- **Role**: Vehicle state management, safety systems, CAN communication
- **Features**: State machine, inverter control, sensor monitoring

### DashPanel (Dashboard)
- **Target**: STM32U599NJ with 5" Riverdi display
- **Role**: Driver information display and vehicle monitoring
- **Features**: LVGL graphics, real-time data visualization, alerts

### SteeringPanel (Steering Wheel)
- **Target**: STM32U599NJ with 5" Riverdi display  
- **Role**: Driver controls and steering wheel interface
- **Features**: LVGL graphics, touch controls, driver inputs

## Development

- **Building**: Modern CMake with ExternalProject for incremental builds → [BUILD.md](BUILD.md)
- **Debugging**: ST-Link/OpenOCD with VS Code Cortex Debug → [DEBUGGING.md](DEBUGGING.md)
- **Contributing**: Branch-based development, see feature development guide → [EXTENDING.md](EXTENDING.md)

## System Architecture

### Shared Components
- **LVGL 9.3.0-dev**: Unified graphics library for both display panels
- **CSV ID Management**: Centralized CAN message ID definitions
- **Common Utilities**: Shared data structures and helper functions
- **ARM Toolchains**: Consistent build environments across boards

### Communication
- **CAN Bus**: Primary communication between ECU and panels
- **SWO Debug**: Real-time debug output from all boards
- **USB**: Programming and debug interface

## Build System

The project uses a modern CMake build system with Ninja and ExternalProject for efficient development:

- **Fast Ninja builds** - Superior build performance and dependency tracking
- **Incremental builds** - Only rebuilds changed components
- **Parallel builds** - Multiple boards build simultaneously  
- **Cross-platform** - Linux, macOS, and Windows support
- **Dependency tracking** - Automatic CSV header generation
- **IDE integration** - Native VS Code task and debug support
- **Multiple flash tools** - st-flash, OpenOCD, STM32CubeProgrammer

## Cross-Platform Support

Developed and tested on multiple operating systems:

| Platform | Status | Build Tools | Flash Tools |
|----------|--------|-------------|-------------|
| **Linux** | Primary | CMake, Ninja, GCC ARM | st-flash, OpenOCD, STM32CubeProgrammer |
| **macOS** | Supported | Homebrew packages | st-flash, OpenOCD, STM32CubeProgrammer |  
| **Windows** | Supported | Downloaded binaries | OpenOCD, STM32CubeProgrammer |

See [BUILD.md](BUILD.md) for platform-specific installation instructions.

## Documentation

- **[BUILD.md](BUILD.md)** - Complete build instructions and troubleshooting
- **[DEBUGGING.md](DEBUGGING.md)** - Debug setup, tools, and techniques
- **[EXTENDING.md](EXTENDING.md)** - Adding features, modifying boards, project structure

## Development Status

This is an active development repository. Please use feature branches and avoid pushing directly to `main` until implementations are stable and tested.
