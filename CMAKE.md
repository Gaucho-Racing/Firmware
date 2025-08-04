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

```mermaid
flowchart TD
    %% Configuration Layer
    Presets["CMakePresets.json<br/>configurePresets:<br/>• Debug → ${CMAKE_BUILD_TYPE}=Debug<br/>• Release → ${CMAKE_BUILD_TYPE}=Release<br/>• HOOTLTest → ${CMAKE_BUILD_TYPE}=Test<br/>• MinSizeRel → ${CMAKE_BUILD_TYPE}=MinSizeRel<br/>• RelWithDebInfo<br/>buildPresets:<br/>• Corresponding builds for each configuration"]
    
    Root["Root CMakeLists.txt<br/>Variables:<br/>• ${CMAKE_PROJECT_NAME}=Firmware<br/>• ${lib_path}=Lib<br/>• ${platform_path}=${lib_path}/Platform<br/>Includes:<br/>• include(${platform_path}/${CHIP}/chip.cmake)<br/>• include(${lib_path}/cmake/gr-lib.cmake)<br/>Commands:<br/>• add_GR_project(${PLATFORM} ${PROJECT})"]
    
    Toolchain1["gcc-arm-none-eabi.cmake<br/>ARM Cross-Compilation Toolchain:<br/>• set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)<br/>• set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${TARGET_FLAGS})<br/>• --specs=nano.specs<br/>• -Wl,--print-memory-usage<br/>• Configures linker settings<br/>• Enables debug symbols and optimization"]
    
    Toolchain2["HOOTL.cmake<br/>Host-Based Testing Toolchain:<br/>• set(CMAKE_SYSTEM_NAME Generic)<br/>• Host-based testing configuration<br/>• No cross-compilation<br/>• Allows running tests on development machine"]
    
    %% Core Library Layer
    GRLib["gr-lib.cmake<br/>function add_GR_project(${Platform} ${Project} [${Optional_Path}]):<br/>1. cmake_language(CALL add_executable_${Platform} ${Project})<br/>2. add_subdirectory(${Project} or ${Optional_Path})<br/>3. add_library(Combinator_${Project} INTERFACE)<br/>4. target_link_libraries(Combinator_${Project} INTERFACE<br/>     ${Platform}_LIB ${Project}_USER_CODE)<br/>5. target_link_libraries(${Project} Combinator_${Project})"]
    
    %% Platform Layer
    ChipConfig["${CHIP}/chip.cmake<br/>Platform Variables:<br/>• ${CHIP} identifier (STM32G474xE, STM32U5A9xJ)<br/>• ${CHIP_PATH} = ${CMAKE_SOURCE_DIR}/Lib/Platform/${CHIP}<br/>• ${TARGET_FLAGS} architecture-specific flags<br/>  (STM32G474xE: -mcpu=cortex-m4...)<br/>  (STM32U5A9xJ: -mcpu=cortex-m33...)<br/>Library Creation:<br/>• add_library(${CHIP}_LIB INTERFACE)<br/>Function:<br/>• add_executable_${CHIP}() with linker script config"]
    
    PlatformLib["${CHIP}_LIB Interface Library<br/>Compile Definitions:<br/>• HAL driver enable flags (USE_HAL_DRIVER)<br/>• Low-level driver enable flags (USE_FULL_LL_DRIVER)<br/>• ${CHIP}-specific preprocessor defines<br/>Include Directories:<br/>• ${CHIP_PATH}/Drivers/CMSIS/Include<br/>• ${CHIP_PATH}/Drivers/stm32-hal-driver/Inc<br/>• ${CHIP_PATH}/Drivers/CMSIS/Device/ST/${CHIP_FAMILY}/Include<br/>Source Files:<br/>• ${CHIP_PATH}/CompileDependencies/startup_*.s<br/>• ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/*_hal_*.c<br/>• ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/*_ll_*.c"]
    
    %% Project Layer
    ProjectCMake["${PROJECT}/CMakeLists.txt<br/>Project Name Extraction:<br/>• get_filename_component(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR} NAME)<br/>User Code Library Creation:<br/>• add_library(${PROJECT_NAME}_USER_CODE INTERFACE)<br/>Source File Specification:<br/>• target_sources(${PROJECT_NAME}_USER_CODE INTERFACE<br/>  - Main application entry point<br/>  - Peripheral driver configurations<br/>  - Hardware abstraction layer customizations<br/>  - Interrupt service routines<br/>  - System configuration files<br/>  - Memory management utilities)<br/>Include Directory Specification:<br/>• target_include_directories(${PROJECT_NAME}_USER_CODE INTERFACE Core/Inc)"]
    
    UserCode["${PROJECT}/ Directory Structure<br/>Standard Layout:<br/>• ${PROJECT}/ (project root)<br/>  └── CMakeLists.txt (build configuration)<br/>  └── Core/<br/>      ├── Inc/ (Header files)<br/>      │   ├── Main application headers<br/>      │   ├── HAL configuration headers<br/>      │   ├── Interrupt handler headers<br/>      │   └── Peripheral driver headers<br/>      └── Src/ (Source files)<br/>          ├── Main application source<br/>          ├── Interrupt service routines<br/>          ├── HAL middleware support<br/>          └── Peripheral driver implementations"]
    
    %% Linking Layer
    Combinator["Combinator_${PROJECT} Library<br/>INTERFACE Library<br/>Purpose: Links platform and project components together<br/>Dependencies:<br/>• ${PLATFORM}_LIB (HAL + hardware abstraction)<br/>• ${PROJECT}_USER_CODE (application-specific code)<br/>Complete dependency chain:<br/>Combinator_${PROJECT} → ${PLATFORM}_LIB + ${PROJECT}_USER_CODE"]
    
    %% Final Output
    Executable["Final ${PROJECT}.elf Executable<br/>Creation Process:<br/>• add_executable(${PROJECT})<br/>• target_link_libraries(${PROJECT} Combinator_${PROJECT})<br/>Configuration:<br/>• LINK_FLAGS: -T${CHIP_PATH}/CompileDependencies/${LINKER_SCRIPT}.ld<br/>• Compiler flags: ${TARGET_FLAGS}<br/>• Linker specifications: --specs=nano.specs<br/>• Memory mapping: -Wl,-Map=${CMAKE_PROJECT_NAME}.map<br/>• Memory reporting: -Wl,--print-memory-usage<br/>Output Files:<br/>• ${PROJECT}.elf (executable binary)<br/>• ${CMAKE_PROJECT_NAME}.map (memory map)"]
    
    %% Build Process
    Configure["Configure Phase<br/>cmake --preset ${preset}<br/>Process Overview:<br/>1. Read ${CMAKE_BUILD_TYPE} from presets<br/>2. Set build configuration<br/>3. Load toolchain file<br/>4. Process root build file<br/>5. Include chip configuration<br/>6. Include library utilities<br/>7. Call project function<br/>8. Generate build files"]
    
    Build["Build Phase<br/>cmake --build build/${preset}<br/>Compilation Steps:<br/>1. Compile HAL drivers from ${CHIP}_LIB<br/>2. Compile user sources from ${PROJECT}_USER_CODE<br/>3. Link with ${TARGET_FLAGS}<br/>4. Apply linker script<br/>5. Generate ${PROJECT}.elf<br/>6. Show memory usage report"]
    
    %% Memory Output
    Memory["Memory Usage Report<br/>Shows resource utilization:<br/>• RAM usage vs available<br/>• Flash memory usage vs available<br/>• Percentage utilization<br/>Generated Output Files:<br/>• build/${preset}/${PROJECT}.elf<br/>• build/${preset}/${CMAKE_PROJECT_NAME}.map<br/>Example:<br/>Memory region    Used Size  %<br/>RAM:            2408 B    1.84%<br/>FLASH:         34860 B    6.65%"]
    
    %% Available Projects
    Projects["Available Projects<br/>Current Projects in Repository:<br/>• G4BLINKY/ - Example STM32G474 blinky project<br/>• G4MVP/ - STM32G474 MVP project<br/>• DASHBLINKY/ - Dashboard blinky project<br/>• ACU/ - Accumulator Control Unit<br/>• MVP/ - Main MVP project<br/>To add a new project:<br/>1. Create directory with CMakeLists.txt following<br/>   ${PROJECT_NAME}_USER_CODE pattern<br/>2. Add add_GR_project(${PLATFORM} ${PROJECT_NAME})<br/>   to root CMakeLists.txt"]
    
    %% Connections
    Presets --> Root
    Presets -.-> Toolchain1
    Presets -.-> Toolchain2
    Root --> GRLib
    Root --> ChipConfig
    
    GRLib --> ProjectCMake
    ChipConfig --> PlatformLib
    
    ProjectCMake --> UserCode
    PlatformLib --> Combinator
    UserCode --> Combinator
    
    Combinator --> Executable
    
    Configure --> Build
    Build --> Memory
    Build --> Executable
    
    Root -.-> Projects
    
    %% Styling
    classDef config fill:#e3f2fd,stroke:#1976d2,stroke-width:2px,color:#000000
    classDef platform fill:#f3e5f5,stroke:#7b1fa2,stroke-width:2px,color:#000000
    classDef project fill:#e8f5e8,stroke:#388e3c,stroke-width:2px,color:#000000
    classDef build fill:#fff8e1,stroke:#f57c00,stroke-width:2px,color:#000000
    classDef output fill:#fce4ec,stroke:#c2185b,stroke-width:2px,color:#000000
    
    class Presets,Root,Toolchain1,Toolchain2 config
    class ChipConfig,PlatformLib platform
    class ProjectCMake,UserCode,Combinator,Projects project
    class Configure,Build build
    class Executable,Memory output
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

### Memory Usage

The build system automatically displays memory usage after linking, such as:
```
Memory region         Used Size  Region Size  %age Used
             RAM:        2408 B       128 KB      1.84%
           FLASH:       34860 B       512 KB      6.65%
```

This information helps ensure the firmware fits within the target microcontroller's constraints.
