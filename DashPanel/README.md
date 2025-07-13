# DashPanel (Dashboard)

Driver information display and vehicle monitoring system.

## Overview

The Dashboard Panel provides:
- **Real-time data display** - Vehicle speed, battery status, motor temperature
- **Driver alerts** - Warning messages and fault notifications  
- **LVGL graphics** - Modern, responsive user interface
- **CAN communication** - Receives data from ECU and other systems

## Hardware
- **Target**: STM32U599NJ microcontroller with 5" Riverdi display
- **Memory**: 4MB Flash, 2.5MB RAM
- **Display**: 800x480 TFT with capacitive touch
- **Graphics**: LVGL 9.3.0-dev with hardware acceleration

## Quick Start

1. **Build**: `cmake --preset=debug && cmake --build --preset=debug --target DashPanel`
2. **Debug**: Use VS Code "Debug DashPanel (ST-Link)" configuration
3. **UI Testing**: Touch interface supports real-time interaction during debug

## Features

- **Multi-screen UI** - Home screen, detailed views, settings
- **Touch controls** - Intuitive gesture support
- **Data visualization** - Gauges, graphs, and status indicators
- **Customizable themes** - Color schemes and layouts

## Key Files

- **`Application/Src/ui/`** - LVGL user interface components
- **`Application/Src/data_handler.c`** - CAN data processing and display updates
- **`Application/Src/touch_handler.c`** - Touch input processing
- **`TouchGFX/`** - UI design assets and generated code

## Documentation

For complete build, debug, and development instructions, see:

- **[BUILD.md](../BUILD.md)** - Build system and compilation
- **[DEBUGGING.md](../DEBUGGING.md)** - Debug setup and techniques  
- **[EXTENDING.md](../EXTENDING.md)** - Adding UI features and modifications

## LVGL Integration

Uses the shared LVGL library from `lib/LVGL/` ensuring consistency with the SteeringPanel and efficient memory usage. Custom widgets and themes are available for consistent vehicle UI design.

## Development

Touch interface can be tested during debugging sessions. Use the ST-Link debug configuration for best performance with the display system running.
