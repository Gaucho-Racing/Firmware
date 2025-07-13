# Shared CMSIS Configuration

> **Note**: This document is now superseded by [SHARED_DRIVERS.md](SHARED_DRIVERS.md) which covers the complete shared driver architecture including both CMSIS and HAL drivers.

## Overview

The firmware build system now uses shared CMSIS drivers from `lib/CMSIS/` for all boards, reducing duplication and ensuring consistency across different STM32 microcontrollers.

**Important**: As of the latest update, not only CMSIS but also HAL drivers are now shared across all boards. See [SHARED_DRIVERS.md](SHARED_DRIVERS.md) for complete documentation.

## Structure

```
lib/
├── CMSIS/                          # Shared CMSIS drivers for all boards
│   ├── Core/                       # ARM Cortex-M core definitions
│   ├── Device/
│   │   └── ST/
│   │       ├── STM32G4xx/         # STM32G4 device-specific CMSIS
│   │       └── STM32U5xx/         # STM32U5 device-specific CMSIS
│   ├── Include/                    # Common CMSIS includes
│   └── ...
├── cmake/
│   ├── cmsis-config.cmake         # New: CMSIS configuration functions
│   ├── stm32g4-config.cmake      # STM32G4 MCU configuration
│   └── stm32u5-config.cmake      # STM32U5 MCU configuration
└── stm32cubemx.cmake              # Updated to use shared CMSIS
```

### Board Structure (unchanged except for CMSIS)

```
DashPanel/                          # STM32U5-based board
├── Drivers/
│   ├── CMSIS/                     # ❌ No longer used for includes
│   └── STM32U5xx_HAL_Driver/      # ✅ Board-specific HAL drivers
├── Middlewares/                    # ✅ Board-specific middlewares
└── ...

ECU/                               # STM32G4-based board  
├── Drivers/
│   ├── CMSIS/                     # ❌ No longer used for includes
│   └── STM32G4xx_HAL_Driver/      # ✅ Board-specific HAL drivers
├── Middlewares/                    # ✅ Board-specific middlewares
└── ...
```

## How It Works

1. **Shared CMSIS**: All CMSIS headers and device definitions are centralized in `lib/CMSIS/`
2. **Board-specific HAL**: Each board keeps its own HAL drivers in `Drivers/STM32xxx_HAL_Driver/`
3. **Board-specific Middlewares**: Each board keeps its own middleware components unchanged
4. **Automatic Configuration**: The build system automatically selects the correct CMSIS variant based on the MCU family

## Benefits

- **Reduced Duplication**: CMSIS drivers are shared across all boards
- **Consistency**: All boards use the same CMSIS version
- **Easier Updates**: CMSIS updates only need to be done in one location
- **Cleaner Build**: No conflicts between different CMSIS versions
- **Preserved Isolation**: HAL drivers and middlewares remain board-specific as needed

## Usage

The configuration is automatic. When building any board:

1. STM32U5 boards (DashPanel, SteeringPanel) automatically use `lib/CMSIS/Device/ST/STM32U5xx/`
2. STM32G4 boards (ECU) automatically use `lib/CMSIS/Device/ST/STM32G4xx/`
3. Common CMSIS headers are shared from `lib/CMSIS/Include/` and `lib/CMSIS/Core/`

## Migration Notes

- Board-specific `Drivers/CMSIS/` directories are preserved but no longer used for include paths
- All CMSIS includes now resolve to the shared `lib/CMSIS/` directory
- No changes needed to board-specific source code
- Build commands remain the same

## Adding New Boards

When adding a new board:

1. Add the appropriate CMSIS device family to `lib/CMSIS/Device/ST/` if not already present
2. Create MCU-specific configuration in `lib/cmake/stm32xxx-config.cmake` if needed
3. Add support in `lib/stm32cubemx.cmake` following the existing patterns
4. The shared CMSIS will be automatically available

## Troubleshooting

If build issues occur:

1. Ensure `lib/CMSIS/` contains the required device family
2. Check that the correct MCU configuration is being used
3. Verify that board-specific HAL drivers are present in `Drivers/STM32xxx_HAL_Driver/`
4. Regenerate build files with `cmake --preset=debug`
