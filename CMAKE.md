# CMake System Architecture Documentation

This document describes the CMake build system architecture for the Gaucho Racing Firmware repository. The system is designed to support multiple STM32 microcontroller platforms and different build configurations.

## Table of Contents

1. [Overview](#overview)
2. [Directory Structure](#directory-structure)
3. [System Architecture Diagram](#system-architecture-diagram)
4. [Core Components](#core-components)
5. [Build Process Flow](#build-process-flow)
6. [Adding New Platforms](#adding-new-platforms)
7. [Adding New Projects](#adding-new-projects)
8. [Build Configuration](#build-configuration)

## Overview

The CMake system is structured around a platform-agnostic architecture that allows easy addition of new STM32 microcontrollers and project configurations. Key features include:

- **Multi-platform support**: Easy switching between different STM32 families
- **Preset-based builds**: Pre-configured build types (Debug, Release, Test, etc.)
- **Modular design**: Platform libraries separate from user code
- **Cross-compilation**: ARM GCC toolchain integration
- **Testing support**: HOOTL (Hardware Out Of The Loop) testing capabilities

## Directory Structure

```
Firmware/
├── CMakeLists.txt              # Root build configuration
├── CMakePresets.json           # Build preset definitions
├── Lib/                        # Library components
│   ├── cmake/
│   │   ├── gcc-arm-none-eabi.cmake  # ARM toolchain configuration
│   │   └── gr-lib.cmake        # Gaucho Racing CMake utilities
│   └── Platform/               # Platform-specific configurations
│       ├── STM32G474xE/        # G474 platform files
│       │   └── chip.cmake      # G474 configuration & HAL drivers
│       └── STM32U5A9xJ/        # U5A9 platform files
│           └── chip.cmake      # U5A9 configuration & HAL drivers
├── cmake/
│   └── HOOTL.cmake            # Hardware-out-of-the-loop testing
├── G4BLINKY/                  # Example project directory
│   ├── CMakeLists.txt         # Project-specific build config
│   └── Core/                  # Project source code
└── [OTHER_PROJECTS]/          # Additional project directories
```

## System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              CMake Build System                              │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────┐    ┌──────────────────┐    ┌─────────────────────────────┐
│ CMakePresets.json│    │  Root CMakeLists │    │    Toolchain Files          │
│                 │    │                  │    │                             │
│ • Debug         │───▶│ • Project setup  │───▶│ • gcc-arm-none-eabi.cmake   │
│ • Release       │    │ • Include chips  │    │ • HOOTL.cmake (for testing) │
│ • Test (HOOTL)  │    │ • Call add_GR_   │    │                             │
│ • MinSizeRel    │    │   project()      │    │                             │
└─────────────────┘    └──────────────────┘    └─────────────────────────────┘
         │                       │                            │
         │                       ▼                            │
         │              ┌──────────────────┐                  │
         │              │   gr-lib.cmake   │                  │
         │              │                  │                  │
         │              │ add_GR_project() │◀─────────────────┘
         │              │ function:        │
         │              │ • Creates target │
         │              │ • Links platform │
         │              │ • Links user code│
         │              │ • Creates combo  │
         │              └──────────────────┘
         │                       │
         │              ┌────────▼────────┐
         │              │ Platform Chip   │
         │              │ Configuration   │
         │              │                 │
         │              │ STM32G474xE/    │
         │              │ ├─chip.cmake    │
         │              │ │ • CPU flags   │
         │  ┌───────────┤ │ • HAL drivers │
         │  │           │ │ • CMSIS files │
         │  │           │ │ • Linker      │
         │  │           │ └─ script       │
         │  │           │                 │
         │  │           │ STM32U5A9xJ/    │
         │  │           │ └─chip.cmake    │
         │  │           └─────────────────┘
         │  │                    │
         │  │           ┌────────▼────────┐
         │  │           │  Platform_LIB   │
         │  │           │  Interface      │
         │  │           │                 │
         │  │           │ • Include dirs  │
         │  │           │ • Compile defs  │
         │  │           │ • HAL sources   │
         │  │           │ • Startup files │
         │  │           └─────────────────┘
         │  │                    │
         │  │                    ▼
         │  │           ┌─────────────────┐      ┌─────────────────┐
         │  │           │ Project User    │      │ Combinator_XXX  │
         │  │           │ Code Library    │      │ Interface Lib   │
         │  │           │                 │      │                 │
         │  │           │ PROJECT_USER_   │◀────▶│ Links:          │
         │  │           │ CODE Interface  │      │ • Platform_LIB  │
         │  │           │                 │      │ • User Code     │
         │  │           │ • Source files  │      │                 │
         │  │           │ • Include dirs  │      └─────────────────┘
         │  │           └─────────────────┘               │
         │  │                    │                        │
         │  │                    ▼                        │
         │  │           ┌─────────────────────────────────▼────┐
         │  │           │         Final Executable            │
         │  │           │                                     │
         │  │           │ • Links Combinator                  │
         │  │           │ • Uses platform linker script      │
         │  │           │ • Configured with TARGET_FLAGS     │
         │  │           └─────────────────────────────────────┘
         │  │
         │  └─────────── Build Process ─────────────┐
         │                                          │
         ▼                                          ▼
┌─────────────────┐                        ┌───────────────────┐
│ Configure Step  │                        │   Build Step      │
│                 │                        │                   │
│ cmake --preset  │                        │ cmake --build     │
│ • Sets toolchain│                        │ • Compiles        │
│ • Sets build    │                        │ • Links           │
│   type          │                        │ • Generates .elf  │
│ • Configures    │                        │ • Shows memory    │
│   generators    │                        │   usage           │
└─────────────────┘                        └───────────────────┘
```

## Core Components

### 1. Root CMakeLists.txt

The main build configuration file that:
- Sets up C11 standard and compiler settings
- Defines project name and build type defaults
- Includes platform and utility CMake files
- Calls `add_GR_project()` to create executables

### 2. CMakePresets.json

Defines build presets for different configurations:
- **Debug/Release/MinSizeRel**: ARM cross-compilation builds
- **HOOTLTest**: Host-based testing (Hardware Out Of The Loop)

### 3. Toolchain Files

#### gcc-arm-none-eabi.cmake
- Configures ARM GCC cross-compiler
- Sets CPU-specific flags via `TARGET_FLAGS`
- Configures linker settings (nano.specs, memory layout)
- Enables debug symbols and optimization levels

#### HOOTL.cmake
- Minimal toolchain for host-based testing
- Allows running tests on development machine

### 4. Platform Configuration (chip.cmake)

Each platform directory contains a `chip.cmake` file that:
- Defines chip-specific settings (`CHIP`, `CHIP_PATH`, `TARGET_FLAGS`)
- Creates a `PLATFORM_LIB` interface library
- Sets compile definitions (HAL drivers, chip-specific defines)
- Includes HAL driver source files and CMSIS headers
- Provides `add_executable_PLATFORM()` function

### 5. Gaucho Racing Library (gr-lib.cmake)

Provides the `add_GR_project()` function that:
1. Creates an executable using platform-specific function
2. Adds the project subdirectory
3. Creates a `Combinator_PROJECT` interface library
4. Links platform library and user code to the combinator
5. Links the combinator to the final executable

### 6. Project Structure

Each project (e.g., G4BLINKY) contains:
- `CMakeLists.txt`: Defines `PROJECT_USER_CODE` interface library
- `Core/Src/`: Source files (main.c, HAL configurations, etc.)
- `Core/Inc/`: Header files

## Build Process Flow

1. **Configuration Phase** (`cmake --preset <preset>`)
   - CMake reads `CMakePresets.json` 
   - Loads appropriate toolchain file
   - Processes root `CMakeLists.txt`
   - Includes platform `chip.cmake`
   - Includes utility `gr-lib.cmake`
   - Calls `add_GR_project(PLATFORM PROJECT)`

2. **Project Setup** (via `add_GR_project()`)
   - Calls `add_executable_PLATFORM(PROJECT)`
   - Processes project's `CMakeLists.txt`
   - Creates `PROJECT_USER_CODE` library
   - Creates `Combinator_PROJECT` library
   - Links everything together

3. **Build Phase** (`cmake --build`)
   - Compiles HAL drivers from platform library
   - Compiles user code from project library
   - Links using platform-specific linker script
   - Generates executable with memory usage report

## Adding New Platforms

To add a new STM32 platform (e.g., STM32H7):

1. **Create platform directory**:
   ```
   Lib/Platform/STM32H7xx/
   ├── chip.cmake
   ├── Drivers/
   │   ├── CMSIS/
   │   └── stm32h7xx-hal-driver/
   └── CompileDependencies/
       ├── startup_stm32h7xx.s
       └── STM32H7XX_FLASH.ld
   ```

2. **Configure chip.cmake**:
   ```cmake
   set(CHIP "STM32H7xx")
   set(TARGET_FLAGS "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard")
   # ... (similar to existing platforms)
   
   function(add_executable_STM32H7xx TARGET_NAME)
       add_executable(${TARGET_NAME})
       set_target_properties(${TARGET_NAME} PROPERTIES 
           LINK_FLAGS "-T${CHIP_PATH}/CompileDependencies/STM32H7XX_FLASH.ld")
   endfunction()
   ```

3. **Update root CMakeLists.txt**:
   ```cmake
   include(${platform_path}/STM32H7xx/chip.cmake)
   ```

## Adding New Projects

To create a new project:

1. **Create project directory** with `CMakeLists.txt`:
   ```cmake
   get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
   
   add_library(${PROJECT_NAME}_USER_CODE INTERFACE)
   target_sources(${PROJECT_NAME}_USER_CODE INTERFACE
       Core/Src/main.c
       # ... other source files
   )
   target_include_directories(${PROJECT_NAME}_USER_CODE INTERFACE
       Core/Inc
   )
   ```

2. **Update root CMakeLists.txt**:
   ```cmake
   add_GR_project(STM32G474xE MYPROJECT)
   ```

## Build Configuration

### Available Build Types

- **Debug**: `-O0 -g3` - Full debug symbols, no optimization
- **Release**: `-Os -g0 -Werror` - Size optimization, no debug, warnings as errors  
- **RelWithDebInfo**: Release optimization with debug symbols
- **MinSizeRel**: Minimal size release build
- **Test**: Host-based compilation for unit testing

### Common CMake Commands

```bash
# Configure for debug build
cmake --preset Debug

# Build the project  
cmake --build build/Debug

# Configure and build for release
cmake --preset Release
cmake --build build/Release

# Run tests
cmake --preset HOOTLTest
cmake --build build/HOOTLTest
```

### Memory Usage

The build system automatically displays memory usage after linking:
```
Memory region         Used Size  Region Size  %age Used
             RAM:        2408 B       128 KB      1.84%
           FLASH:       34860 B       512 KB      6.65%
```

This information helps ensure the firmware fits within the target microcontroller's constraints.
