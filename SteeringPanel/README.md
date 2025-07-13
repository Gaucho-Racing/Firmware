# SteeringPanel (Steering Wheel)

Driver controls and steering wheel interface system.

## Overview

The Steering Panel provides:
- **Driver controls** - Touch buttons for vehicle functions
- **Quick access displays** - Critical information at driver's fingertips
- **LVGL graphics** - Responsive touch interface optimized for racing
- **CAN communication** - Sends driver inputs and receives vehicle status

## Hardware
- **Target**: STM32U599NJ microcontroller with 5" Riverdi display
- **Memory**: 4MB Flash, 2.5MB RAM  
- **Display**: 800x480 TFT with capacitive touch
- **Mounting**: Integrated with steering wheel assembly

## Quick Start

1. **Build**: `cmake --preset=debug && cmake --build --preset=debug --target SteeringPanel`
2. **Debug**: Use VS Code "Debug SteeringPanel (ST-Link)" configuration
3. **Testing**: Touch controls work during debug sessions

## Features

- **Racing-optimized UI** - Quick access to essential controls
- **Touch buttons** - Start/stop, drive modes, system toggles  
- **Status displays** - Critical vehicle parameters
- **Driver preferences** - Customizable layouts and shortcuts

## Key Files

- **`Application/Src/ui/`** - LVGL steering wheel interface components
- **`Application/Src/controls.c`** - Driver input processing and CAN transmission
- **`Application/Src/display_manager.c`** - Screen updates and data visualization

## Documentation

For complete build, debug, and development instructions, see:

- **[BUILD.md](../BUILD.md)** - Build system and compilation
- **[DEBUGGING.md](../DEBUGGING.md)** - Debug setup and techniques  
- **[EXTENDING.md](../EXTENDING.md)** - Adding controls and UI modifications

## LVGL Integration

Shares the LVGL library from `lib/LVGL/` with the DashPanel, ensuring consistent UI behavior and optimal resource usage. Specialized widgets are designed for steering wheel ergonomics.

## Development

The steering wheel interface is optimized for quick driver interaction during racing. Touch responsiveness and visual feedback are prioritized for safety and performance.
