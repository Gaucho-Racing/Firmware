# Shared Driver Architecture

This document describes the shared driver architecture used in the Gaucho Racing Firmware Suite.

## Overview

All STM32 drivers (CMSIS and HAL) are now centralized in the `lib/` directory to eliminate code duplication and ensure consistency across all boards. This approach reduces repository size, simplifies maintenance, and ensures all boards use the same driver versions.

## Directory Structure

```
lib/
├── CMSIS/                     # Shared ARM Cortex-M and STM32 drivers
│   ├── Core/Include/          # ARM Cortex-M core headers
│   ├── Include/               # CMSIS standard headers
│   └── Device/ST/             # STM32-specific device headers
│       ├── STM32G4xx/         # STM32G4 family headers for ECU
│       └── STM32U5xx/         # STM32U5 family headers for panels
├── STM32G4xx_HAL_Driver/      # Shared STM32G4 HAL drivers
│   ├── Inc/                   # Header files
│   └── Src/                   # Source files
└── STM32U5xx_HAL_Driver/      # Shared STM32U5 HAL drivers
    ├── Inc/                   # Header files
    └── Src/                   # Source files
```

## Driver Mapping

### CMSIS Drivers
- **All boards** use the shared CMSIS from `lib/CMSIS/`
- ECU uses STM32G4xx device headers
- DashPanel and SteeringPanel use STM32U5xx device headers

### HAL Drivers
- **ECU** uses shared STM32G4xx HAL drivers from `lib/STM32G4xx_HAL_Driver/`
- **DashPanel** uses shared STM32U5xx HAL drivers from `lib/STM32U5xx_HAL_Driver/`
- **SteeringPanel** uses shared STM32U5xx HAL drivers from `lib/STM32U5xx_HAL_Driver/`

## Board-Specific Components

Each board retains its own board-specific components:

```
ECU/
├── Core/                      # Board-specific STM32 configuration
├── Application/               # ECU application logic
└── (no Drivers/ directory)   # All drivers are now shared

DashPanel/
├── Core/                      # Board-specific STM32 configuration
├── Application/               # Dashboard application logic
├── Middlewares/               # FreeRTOS and other middleware
└── (no Drivers/ directory)   # All drivers are now shared

SteeringPanel/
├── Core/                      # Board-specific STM32 configuration
├── Application/               # Steering wheel application logic
├── Middlewares/               # FreeRTOS and other middleware
└── (no Drivers/ directory)   # All drivers are now shared
```

## CMake Configuration

The shared drivers are configured in `lib/stm32cubemx.cmake`:

### STM32G4 Configuration
- Include paths point to `../lib/STM32G4xx_HAL_Driver/Inc`
- Source files reference `../lib/STM32G4xx_HAL_Driver/Src/`
- CMSIS headers from `../lib/CMSIS/Device/ST/STM32G4xx/Include`

### STM32U5 Configuration
- Include paths point to `../lib/STM32U5xx_HAL_Driver/Inc`
- Source files reference `../lib/STM32U5xx_HAL_Driver/Src/`
- CMSIS headers from `../lib/CMSIS/Device/ST/STM32U5xx/Include`

## VS Code IntelliSense

The `.vscode/c_cpp_properties.json` file provides board-specific configurations:

- **ECU**: Uses compile commands from `ECU/build/` and includes shared STM32G4 drivers
- **DashPanel**: Uses compile commands from `DashPanel/build/` and includes shared STM32U5 drivers
- **SteeringPanel**: Uses compile commands from `SteeringPanel/build/` and includes shared STM32U5 drivers

## Benefits

1. **Reduced Duplication**: Eliminates multiple copies of identical driver files
2. **Consistency**: All boards use the same driver versions
3. **Easier Updates**: Update drivers in one location for all boards
4. **Smaller Repository**: Significantly reduces repository size
5. **Simplified Maintenance**: Single source of truth for all drivers

## Migration Notes

Previously, each board had its own `Drivers/` directory containing:
- `CMSIS/` - ARM Cortex-M and STM32 device drivers
- `STM32xxx_HAL_Driver/` - STM32 HAL drivers

These have been consolidated into the shared `lib/` directory. The board-specific `Drivers/` directories have been removed entirely.

## Adding New Boards

When adding new boards:

1. If using STM32G4, reference the existing shared STM32G4 drivers
2. If using STM32U5, reference the existing shared STM32U5 drivers  
3. If using a new STM32 family, add the new HAL drivers to `lib/STM32xxx_HAL_Driver/`
4. Update `lib/stm32cubemx.cmake` with a new function for the MCU family
5. Add appropriate CMSIS device headers to `lib/CMSIS/Device/ST/`

This shared architecture scales efficiently as new boards and MCU families are added to the project.
