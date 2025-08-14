# CMake System Architecture Documentation

This document describes the CMake build system architecture for the Gaucho Racing Firmware repository. The system is designed to support multiple STM32 microcontroller platforms and different build configurations.

## Table of Contents

1. [Directory Structure](#directory-structure)
2. [System Architecture Diagram](#system-architecture-diagram)
3. [Core Components](#core-components)
4. [Build Process Flow](#build-process-flow)
5. [Adding New Platforms](#adding-new-platforms)
6. [Adding New Projects](#adding-new-projects)
7. [Build Configuration](#build-configuration)

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
│       ├── STM32U5A9xJ/        # U5A9 platform files
│       │   └── chip.cmake      # U5A9 configuration & HAL drivers
│       └── [OTHER_PLATFORMS]/  # Additional platform directories
│           └── chip.cmake      # Platform-specific configurations
├── cmake/
│   └── HOOTL.cmake            # Hardware-out-of-the-loop testing
├── G4BLINKY/                  # Example project directory
│   ├── CMakeLists.txt         # Project-specific build config
│   └── Core/                  # Project source code
└── [OTHER_PROJECTS]/          # Additional project directories
    ├── CMakeLists.txt         # Project build configurations
    └── Core/                  # Project source files
```

## System Architecture Diagram

```mermaid
flowchart TD
    %% Configuration Layer
    Presets["📋 CMakePresets.json<br/>Build Configurations"] --> Root["🏗️ Root CMakeLists.txt<br/>Main Build Script"]
    Presets -.-> Toolchain1["⚙️ gcc-arm-none-eabi.cmake<br/>ARM Cross-Compiler"]
    Presets -.-> Toolchain2["🧪 HOOTL.cmake<br/>Host Testing"]
    
    %% Core Flow
    Root --> GRLib["📚 gr-lib.cmake<br/>Project Creation Utils"]
    Root --> ChipConfig["💾 ${CHIP}/chip.cmake<br/>Platform Config"]
    
    %% Platform & Project
    GRLib --> ProjectCMake["📁 ${PROJECT}/CMakeLists.txt<br/>Project Build Rules"]
    ChipConfig --> PlatformLib["🔌 ${CHIP}_LIB Interface<br/>HAL + Drivers"]
    
    %% Linking
    ProjectCMake --> UserCode["👨‍💻 ${PROJECT}/ Directory<br/>Application Code"]
    PlatformLib --> Combinator["🔗 Combinator_${PROJECT}<br/>Link Library"]
    UserCode --> Combinator
    
    %% Output
    Combinator --> Executable["🎯 ${PROJECT}.elf<br/>Final Binary"]
    
    %% Build Process
    Configure["⚙️ Configure Phase<br/>Setup Build System"] --> Build["🔨 Build Phase<br/>Compile & Link"]
    Build --> Memory["📊 Memory Report<br/>Usage Analysis"]
    Build --> Executable
    
    %% Enhanced Styling with better contrast and readability
    classDef config fill:#E3F2FD,stroke:#0D47A1,stroke-width:3px,color:#000000,font-weight:bold,font-size:12px
    classDef platform fill:#F1E8FF,stroke:#4A148C,stroke-width:3px,color:#000000,font-weight:bold,font-size:12px
    classDef project fill:#E8F5E8,stroke:#1B5E20,stroke-width:3px,color:#000000,font-weight:bold,font-size:12px
    classDef build fill:#FFF8E1,stroke:#E65100,stroke-width:3px,color:#000000,font-weight:bold,font-size:12px
    classDef output fill:#E8F5E8,stroke:#2E7D32,stroke-width:3px,color:#000000,font-weight:bold,font-size:12px
    
    class Presets,Root,Toolchain1,Toolchain2 config
    class ChipConfig,PlatformLib platform
    class ProjectCMake,UserCode,Combinator project
    class Configure,Build build
    class Executable,Memory output
```

## Extended System Architecture

```
╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                           CONFIGURATION LAYER                                             ║
╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝

┌─────────────────────────┐    ┌───────────────────────────┐    ┌────────────────────────────────────────┐
│   CMakePresets.json     │    │    Root CMakeLists.txt    │    │           Toolchain Files              │
│                         │    │                           │    │                                        │
│ configurePresets:       │──▶│ Variables:                │──▶│ gcc-arm-none-eabi.cmake:               │
│ • Debug                 │    │ • Project name setup      │    │ • ARM GCC compiler config              │
│ • Release               │    │ • Library path definition │    │ • Cross-compilation flags              │
│ • HOOTLTest             │    │ • Platform path setup     │    │ • Nano specs configuration             │
│ • MinSizeRel            │    │                           │    │ • Memory usage reporting               │
│ • RelWithDebInfo        │    │ File Includes:            │    │                                        │
│                         │    │ • Platform-specific       │    │ HOOTL.cmake:                           │
│ buildPresets:           │    │   chip configuration      │    │ • Host system targeting                │
│ • Corresponding builds  │    │ • Library utilities       │    │ • Native compilation setup             │
└─────────────────────────┘    │                           │    │ • Unit testing support                 │
                               │ Project Creation:         │    │                                        │
                               │ • Calls project function  │    └────────────────────────────────────────┘
                               │   with platform & name    │
                               └───────────────────────────┘
                                           │
                                           ▼

╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                          LIBRARY CORE LAYER                                              ║
╚══════════════════════════════════════════════════════════════════════════════════════════════════════════╝

                               ┌───────────────────────────────────────────────────────────────────────────┐
                               │                           gr-lib.cmake                                    │
                               │                                                                           │
                               │ add_GR_project Function:                                                  │
                               │ ┌─────────────────────────────────────────────────────────────────────┐   │
                               │ │ • Creates platform-specific executable                              │   │
                               │ │ • Adds project subdirectory to build                                │   │
                               │ │ • Creates combinator interface library                              │   │
                               │ │ • Links platform library and user code                              │   │
                               │ │   to combinator                                                     │   │
                               │ │ • Links combinator to final executable                              │   │
                               │ └─────────────────────────────────────────────────────────────────────┘   │
                               └───────────────────────────────────────────────────────────────────────────┘
                                           │
                                           ▼

╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                         PLATFORM LAYER                                                   ║
╚══════════════════════════════════════════════════════════════════════════════════════════════════════════╝

                                ┌──────────────────────────────────────┐
                                │        ${CHIP}/chip.cmake            │
                                │                                      │
                                │ Platform Variables:                  │
                                │ • ${CHIP} identifier                 │
                                │ • ${CHIP_PATH} directory             │
                                │ • ${TARGET_FLAGS} architecture       │
                                │                                      │
                                │ Library Creation:                    │
                                │ • ${CHIP}_LIB interface              │
                                │                                      │
                                │ Function:                            │
                                │ • add_executable_${CHIP}()           │
                                │   └→ Linker script configuration     │
                                └──────────────────────────────────────┘
                                                   │
                                                   ▼

              ┌─────────────────────────────────────────────────────────────────────────┐
              │                         ${CHIP}_LIB Interface                           │
              │                                                                         │
              │ Compile Definitions:                                                    │
              │ • Hardware abstraction layer enablement                                 │
              │ • Low-level peripheral driver enablement                                │
              │ • Chip-specific feature definitions                                     │
              │                                                                         │
              │ Include Directories:                                                    │
              │ • CMSIS core processor headers                                          │
              │ • STM32 HAL driver interface headers                                    │
              │ • Device-specific register definitions                                  │
              │                                                                         │
              │ Source Files:                                                           │
              │ • Processor startup and initialization code                             │
              │ • Hardware abstraction layer implementations                            │
              │ • Low-level peripheral driver implementations                           │
              └─────────────────────────────────────────────────────────────────────────┘
                                       │
                                       ▼

╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                         PROJECT LAYER                                                    ║
╚══════════════════════════════════════════════════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                               ${PROJECT}/CMakeLists.txt Structure                                       │
│                              (Each project follows this pattern)                                        │
│                                                                                                         │
│ Project Name Extraction:                                                                                │
│ • get_filename_component(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR} NAME)                              │
│                                                                                                         │
│ User Code Library Creation:                                                                             │
│ • add_library(${PROJECT_NAME}_USER_CODE INTERFACE)                                                      │
│                                                                                                         │
│ Source File Specification:                                                                              │
│ • target_sources(${PROJECT_NAME}_USER_CODE INTERFACE ...)                                               │
│   - Main application entry point                                                                        │
│   - Peripheral driver configurations                                                                    │
│   - Hardware abstraction layer customizations                                                           │
│   - Interrupt service routines                                                                          │
│   - System configuration files                                                                          │
│   - Memory management utilities                                                                         │
│                                                                                                         │
│ Include Directory Specification:                                                                        │
│ • target_include_directories(${PROJECT_NAME}_USER_CODE INTERFACE ...)                                   │
└─────────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                       │
                                       ▼

              ┌────────────────────────────────────────────────────────────────────────┐
              │                   ${PROJECT}/ Directory Structure                      │
              │                                                                        │
              │ Standard Layout:                                                       │
              │ • ${PROJECT}/ (project root)                                           │
              │   └── CMakeLists.txt (build configuration)                             │
              │   └── Core/                                                            │
              │       ├── Inc/ (Header files)                                          │
              │       │   ├── Main application headers                                 │
              │       │   ├── HAL configuration headers                                │
              │       │   ├── Interrupt handler headers                                │
              │       │   └── Peripheral driver headers                                │
              │       └── Src/ (Source files)                                          │
              │           ├── Main application source                                  │
              │           ├── Interrupt service routines                               │
              │           ├── HAL middleware support                                   │
              │           └── Peripheral driver implementations                        │
              └────────────────────────────────────────────────────────────────────────┘
                                       │
                                       ▼

╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                        LINKING LAYER                                                     ║
╚══════════════════════════════════════════════════════════════════════════════════════════════════════════╝

              ┌─────────────────────────────────────────────────────────────────────────┐
              │                    Combinator_${PROJECT} Library                        │
              │                     (Interface Library)                                 │
              │                                                                         │
              │ Purpose: Links platform and project components together                 │
              │                                                                         │
              │ Dependencies:                                                           │
              │ • ${PLATFORM}_LIB (HAL + hardware abstraction)                          │
              │ • ${PROJECT}_USER_CODE (application-specific code)                      │
              │                                                                         │
              │ This creates the complete dependency chain:                             │
              │ Combinator_${PROJECT} → ${PLATFORM}_LIB + ${PROJECT}_USER_CODE          │
              └─────────────────────────────────────────────────────────────────────────┘
                                       │
                                       ▼

╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                        FINAL OUTPUT                                                      ║
╚══════════════════════════════════════════════════════════════════════════════════════════════════════════╝

              ┌─────────────────────────────────────────────────────────────────────────┐
              │                      Final ${PROJECT}.elf Executable                    │
              │                                                                         │
              │ Creation Process:                                                       │
              │ • Creates executable with project name                                  │
              │ • Links with combinator library                                         │
              │                                                                         │
              │ Configuration:                                                          │
              │ • Linker script from platform dependencies                              │
              │ • Architecture-specific compiler flags                                  │
              │ • Memory map generation                                                 │
              │ • Memory usage reporting                                                │
              │                                                                         │
              │ Output Files:                                                           │
              │ • ${PROJECT}.elf (executable binary)                                    │
              │ • ${CMAKE_PROJECT_NAME}.map (memory map)                                │
              └─────────────────────────────────────────────────────────────────────────┘

╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                     BUILD PROCESS FLOW                                                   ║
╚══════════════════════════════════════════════════════════════════════════════════════════════════════════╝

┌────────────────────────────┐                     ┌──────────────────────────────┐
│      Configure Phase       │                     │         Build Phase          │
│                            │───────────────────▶│                              │
│                            │                     │                              │
│ Process Overview:          │                     │ Compilation Steps:           │
│ 1. Read CMAKE_BUILD_TYPE   │                     │ 1. Compile HAL drivers       │
│ 2. Set build configuration │                     │    from ${CHIP}_LIB          │
│ 3. Load toolchain file     │                     │ 2. Compile user sources      │
│ 4. Process root build file │                     │    from ${PROJECT}_USER_CODE │
│ 5. Include chip config     │                     │ 3. Link with ${TARGET_FLAGS} │
│ 6. Include library utils   │                     │ 4. Apply linker script       │
│ 7. Call project function   │                     │ 5. Generate ${PROJECT}.elf   │
│ 8. Generate build files    │                     │ 6. Show memory usage         │
└────────────────────────────┘                     └──────────────────────────────┘
                                                               │
                                                               ▼
                                   ┌─────────────────────────────────────────────┐
                                   │           Memory Usage Report               │
                                   │                                             │
                                   │ Shows resource utilization:                 │
                                   │ • RAM usage vs available                    │
                                   │ • Flash memory usage vs available           │
                                   │ • Percentage utilization                    │
                                   │                                             │
                                   │ Generated Output Files:                     │
                                   │ • build/preset/${PROJECT}.elf               │
                                   │ • build/preset/${CMAKE_PROJECT_NAME}.map    │
                                   └─────────────────────────────────────────────┘
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

TODO

## Adding New Projects

To create a new project for any supported platform:

TODO

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
