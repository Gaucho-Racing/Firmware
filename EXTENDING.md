# Extending the Firmware

Guide for adding features, modifying boards, and understanding the project architecture.

## Project Architecture

### System Overview
```
┌─────────────────┐    CAN Bus    ┌─────────────────┐
│   DashPanel     │◄─────────────►│      ECU        │
│  (STM32U5)      │               │   (STM32G4)     │
│  - LVGL UI      │               │  - State Machine│
│  - Data Display │               │  - CAN Master   │
│  - Safety Logic │               │  - Safety Logic │
└─────────────────┘               └─────────────────┘
┌─────────────────┐                        │
│ SteeringPanel   │                        │ CAN Bus
│  (STM32U5)      │◄───────────────────────┘
│  - LVGL UI      │
│  - Driver Input │
└─────────────────┘
```

### Communication Architecture
- **CAN Bus**: Primary communication between all boards
- **CSV-based IDs**: Centralized message ID management
- **SWO Debug**: Individual debug streams per board
- **USB**: Programming and advanced debugging

### Build System Architecture
- **Shared STM32CubeMX Library**: Centralized STM32 HAL and middleware code in `lib/cmake/stm32cubemx/`
- **Board-Specific Overrides**: Each board can add specific sources while sharing common HAL code
- **ExternalProject System**: Efficient incremental builds with proper dependency tracking

## Code Organization

### Directory Structure
```
Firmware-26/
├── lib/                           # Shared components
│   ├── LVGL/                     # Graphics library (shared)
│   ├── CSVGenerator/             # CAN ID management
│   ├── cmake/                    # Build system components
│   ├── include/                  # Generated headers
│   └── utils.c/h                 # Common utilities
├── ECU/
│   ├── Application/              # ECU-specific logic
│   │   ├── Inc/                  # Application headers
│   │   ├── Src/                  # Application source
│   │   └── Test/                 # Unit tests (future)
│   └── Core/                     # STM32 HAL
├── DashPanel/
│   ├── Application/              # Dashboard logic
│   │   ├── Inc/                  # Application headers
│   │   └── Src/                  # Application source
│   ├── TouchGFX/                 # UI framework
│   └── Core/                     # STM32 HAL
└── SteeringPanel/
    ├── Application/              # Steering logic
    │   ├── Inc/                  # Application headers
    │   └── Src/                  # Application source
    └── Core/                     # STM32 HAL
```

### Shared vs Board-Specific Code

#### Shared Components (`lib/`)
- **LVGL**: Graphics library used by both panels
- **CMSIS**: ARM Cortex-M and STM32 device drivers shared across all boards
- **utils.c/h**: Common data structures, math, utilities
- **CSV Generator**: CAN message ID definitions
- **CMake configs**: ARM toolchain setups
- **STM32CubeMX Library**: Shared STM32 HAL and middleware code
  - `lib/stm32cubemx.cmake`: Functions for creating STM32U5 and STM32G4 targets
  - `lib/cmake/cmsis-config.cmake`: Shared CMSIS configuration functions
  - Reduces code duplication across boards
  - Provides consistent HAL driver integration

#### Board-Specific Code
- **Application/**: Business logic unique to each board
- **Core/**: STM32 HAL configuration files, system initialization
- **Build configs**: CMakeLists.txt, linker scripts
- **Board-specific STM32 sources**: Additional peripherals not in the shared set

## Adding New Features

### 1. Adding CAN Messages

#### Step 1: Define Message in CSV
Edit `lib/CSVGenerator/CAN_IDs.csv`:
```csv
Message Name,ID (Hex),Length,Frequency (Hz),Sender,Description
NEW_SENSOR_DATA,0x123,8,100,ECU,New sensor readings
```

#### Step 2: Generate Headers
**In VS Code**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Generate CSV Headers"

**Command line alternative**:
```bash
cmake --build --preset=Debug --target generateCSVHeaders
```

#### Step 3: Use in Code
```c
// In ECU/Application/Src/can_handler.c
#include "CAN_IDs.h"

void send_new_sensor_data() {
    uint8_t data[8] = {0};
    // Pack sensor data...
    
    can_send_message(NEW_SENSOR_DATA_ID, data, 8);
}
```

### 2. Adding LVGL UI Elements

#### Step 1: Create UI Components
```c
// In DashPanel/Application/Src/ui_components.c
#include "lvgl.h"

lv_obj_t* create_new_gauge(lv_obj_t* parent) {
    lv_obj_t* gauge = lv_arc_create(parent);
    lv_arc_set_range(gauge, 0, 100);
    lv_arc_set_value(gauge, 50);
    
    // Style the gauge...
    return gauge;
}
```

#### Step 2: Integrate with Main UI
```c
// In DashPanel/Application/Src/main_screen.c
void setup_main_screen() {
    lv_obj_t* screen = lv_scr_act();
    lv_obj_t* new_gauge = create_new_gauge(screen);
    
    // Position and configure...
}
```

### 3. Adding State Machine States (ECU)

#### Step 1: Define New State
```c
// In ECU/Application/Inc/state_machine.h
typedef enum {
    STATE_INIT,
    STATE_READY,
    STATE_DRIVING,
    STATE_NEW_STATE,    // Add new state
    STATE_ERROR
} vehicle_state_t;
```

#### Step 2: Implement State Logic
```c
// In ECU/Application/Src/state_machine.c
void handle_new_state() {
    // State entry logic
    if (state_entry) {
        debug_printf("Entering NEW_STATE\n");
        // Initialize state...
    }
    
    // State update logic
    if (condition_met) {
        transition_to_state(STATE_DRIVING);
    }
    
    // State exit logic  
    if (state_exit) {
        debug_printf("Exiting NEW_STATE\n");
        // Cleanup...
    }
}
```

### 4. Adding Sensor Interfaces

#### Step 1: Create Sensor Driver
```c
// In ECU/Application/Src/sensors/new_sensor.c
#include "new_sensor.h"

typedef struct {
    float value;
    bool valid;
    uint32_t last_update;
} new_sensor_data_t;

static new_sensor_data_t sensor_data = {0};

bool new_sensor_init() {
    // Initialize sensor hardware
    return true;
}

bool new_sensor_read(float* value) {
    // Read sensor via SPI/I2C/ADC
    *value = sensor_data.value;
    return sensor_data.valid;
}

void new_sensor_update() {
    // Called periodically from main loop
    float raw_value = read_raw_sensor();
    sensor_data.value = process_raw_value(raw_value);
    sensor_data.valid = validate_reading(raw_value);
    sensor_data.last_update = HAL_GetTick();
}
```

#### Step 2: Integrate with Main Loop
```c
// In ECU/Application/Src/main.c
void main_loop() {
    while (1) {
        new_sensor_update();
        state_machine_update();
        can_handler_update();
        
        HAL_Delay(10);  // 100Hz loop
    }
}
```

## Modifying Existing Boards

### ECU Modifications

#### Key Files to Modify
- **`state_machine.c/h`**: Vehicle state logic
- **`can_handler.c/h`**: CAN message processing
- **`sensors/`**: Sensor interfaces
- **`safety.c/h`**: Safety systems
- **`main.c`**: Main application loop

#### Common Modifications
1. **Adding sensors**: Create driver in `sensors/`, integrate in main loop
2. **CAN messages**: Add to CSV, implement handlers
3. **Safety features**: Modify `safety.c` and state machine
4. **Performance tuning**: Adjust loop timing, optimize algorithms

### Panel Modifications (DashPanel/SteeringPanel)

#### Key Files to Modify
- **`ui/`**: LVGL user interface components
- **`data_handler.c/h`**: CAN data processing and display
- **`touch_handler.c/h`**: Touch input processing
- **`main.c`**: Application initialization and loop

#### Common Modifications
1. **UI changes**: Modify LVGL components in `ui/`
2. **Data display**: Update `data_handler.c` for new CAN messages
3. **Touch controls**: Add handlers in `touch_handler.c`
4. **Styling**: Update LVGL styles and themes

## 🆕 Adding New Boards

### 1. Create Board Directory
```bash
mkdir NewBoard
cd NewBoard
```

### 2. Setup CMake Configuration
Create `NewBoard/CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.25)

# Include shared ARM toolchain
include(${CMAKE_SOURCE_DIR}/lib/cmake/stm32-config.cmake)

project(NewBoard C ASM)

# Board-specific settings
set(MCU_FAMILY STM32G4xx)
set(MCU_MODEL STM32G474xx)

# Add sources
file(GLOB_RECURSE SOURCES
    "Application/Src/*.c"
    "Core/Src/*.c"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE gaucho_utils)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    Application/Inc
    Core/Inc
    ${CMAKE_SOURCE_DIR}/lib/include
)
```

### 3. Configure CMSIS and MCU Support

#### For Existing MCU Families (STM32G4, STM32U5)
If your new board uses STM32G4 or STM32U5, the shared CMSIS will work automatically:

```cmake
# In your board's CMakeLists.txt

# Include shared MCU configuration
include("../lib/cmake/stm32g4-config.cmake")  # For STM32G4
# OR
include("../lib/cmake/stm32u5-config.cmake")  # For STM32U5

# Include shared STM32CubeMX functions
include("../lib/stm32cubemx.cmake")

# Create target with shared CMSIS
create_stm32g4_cubemx_target(stm32cubemx)  # For STM32G4
# OR 
create_stm32u5_cubemx_target(stm32cubemx)  # For STM32U5
```

#### For New MCU Families
If adding a board with a new STM32 family (e.g., STM32H7):

1. **Add CMSIS Device Drivers** to `lib/CMSIS/Device/ST/`:
```bash
lib/CMSIS/Device/ST/
├── STM32G4xx/          # Existing
├── STM32U5xx/          # Existing  
└── STM32H7xx/          # New family
    ├── Include/
    │   ├── stm32h7xx.h
    │   └── system_stm32h7xx.h
    └── Source/
        └── Templates/
```

2. **Create MCU Configuration** `lib/cmake/stm32h7-config.cmake`:
```cmake
# STM32H7 (Cortex-M7) configuration
set(TARGET_FLAGS "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard")
set(DEBUG_FLAGS "-O0 -g3")
```

3. **Add CubeMX Function** in `lib/stm32cubemx.cmake`:
```cmake
function(create_stm32h7_cubemx_target TARGET_NAME)
    add_library(${TARGET_NAME} INTERFACE)
    
    target_compile_definitions(${TARGET_NAME} INTERFACE 
        USE_HAL_DRIVER 
        STM32H743xx
        $<$<CONFIG:Debug>:DEBUG>
    )
    
    target_include_directories(${TARGET_NAME} INTERFACE
        Core/Inc
        Drivers/STM32H7xx_HAL_Driver/Inc
        Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
        ../lib/CMSIS/Device/ST/STM32H7xx/Include
        ../lib/CMSIS/Include
    )
    
    # Link shared CMSIS
    link_shared_cmsis(${TARGET_NAME} "STM32H7")
    
    # Add HAL sources...
endfunction()
```

4. **Update CMSIS Config** in `lib/cmake/cmsis-config.cmake`:
```cmake
function(create_shared_cmsis_stm32h7 TARGET_NAME)
    add_library(${TARGET_NAME} INTERFACE)
    target_include_directories(${TARGET_NAME} INTERFACE
        ../lib/CMSIS/Device/ST/STM32H7xx/Include
        ../lib/CMSIS/Include
        ../lib/CMSIS/Core/Include
    )
endfunction()

# Update link_shared_cmsis function to handle STM32H7...
```

### 4. Add to Root CMake
Edit main `CMakeLists.txt`:
```cmake
# Add new board to ExternalProject
ExternalProject_Add(NewBoard
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/NewBoard"
    BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/NewBoard/build"
    CMAKE_ARGS ${ARM_CMAKE_ARGS}
    BUILD_ALWAYS TRUE
    DEPENDS generateCSVHeaders
)

# Add to convenience targets
add_custom_target(all-boards 
    DEPENDS DashPanel SteeringPanel ECU NewBoard
)
```

### 5. Create VS Code Debug Configuration
Add to `.vscode/launch.json`:
```json
{
    "name": "Debug NewBoard",
    "type": "cortex-debug",
    "request": "launch",
    "servertype": "openocd",
    "executable": "${workspaceFolder}/NewBoard/build/NewBoard.elf",
    "device": "STM32G474RE",
    "configFiles": ["interface/stlink.cfg", "target/stm32g4x.cfg"],
    "svdFile": "${workspaceFolder}/lib/svd/STM32G474.svd"
}
```

## 🧪 Testing Framework Setup

### Unit Testing Structure
```
ECU/Application/Test/
├── framework/
│   ├── unity.c           # Unity test framework
│   ├── unity.h
│   └── test_runner.c
├── mocks/
│   ├── mock_can.c        # Mock CAN interface
│   ├── mock_sensors.c    # Mock sensor readings
│   └── mock_hal.c        # Mock STM32 HAL
└── tests/
    ├── test_state_machine.c
    ├── test_can_handler.c
    └── test_safety.c
```

### Example Unit Test
```c
// ECU/Application/Test/tests/test_state_machine.c
#include "unity.h"
#include "state_machine.h"
#include "mock_can.h"

void setUp(void) {
    state_machine_init();
    mock_can_reset();
}

void test_initial_state(void) {
    TEST_ASSERT_EQUAL(STATE_INIT, get_current_state());
}

void test_ready_transition(void) {
    // Setup conditions for ready state
    mock_can_set_connected(true);
    mock_sensors_set_valid(true);
    
    // Update state machine
    state_machine_update();
    
    // Verify transition
    TEST_ASSERT_EQUAL(STATE_READY, get_current_state());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state);
    RUN_TEST(test_ready_transition);
    return UNITY_END();
}
```

## Development Workflow

### Feature Development Process
1. **Create feature branch** (VS Code Git integration or terminal)
   ```bash
   git checkout -b feature/new-dashboard-widget
   ```

2. **Make incremental changes in VS Code**
   - Update CSV IDs if needed
   - Implement core functionality  
   - Add tests (when framework is ready)
   - Update documentation

3. **Build and test frequently**
   - **VS Code**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Build DashPanel"
   - **Debug**: Use VS Code's integrated debugger
   - **Flash**: Use VS Code flashing tasks

4. **Integration testing**
   - **VS Code**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Boards"
   - Test complete system with all boards

5. **Create pull request**
   - Ensure all boards build successfully in VS Code
   - Include testing evidence
   - Update relevant documentation

### Code Style Guidelines

#### C Coding Standards
```c
// Use snake_case for functions and variables
void update_sensor_data(void);
float battery_voltage = 0.0f;

// Use UPPER_CASE for macros and constants
#define MAX_SPEED_KMH 100
#define CAN_TIMEOUT_MS 500

// Use descriptive names
typedef struct {
    float voltage;
    float current; 
    bool fault_detected;
} battery_status_t;

// Document complex functions
/**
 * @brief Updates vehicle state machine
 * @details Handles state transitions based on current conditions
 * @param sensor_data Current sensor readings
 * @return New vehicle state
 */
vehicle_state_t update_state_machine(sensor_data_t* sensor_data);
```

#### File Organization
```c
// Header files (.h)
#ifndef HEADER_NAME_H
#define HEADER_NAME_H

#include <stdint.h>
#include <stdbool.h>

// Public constants
#define PUBLIC_CONSTANT 42

// Public types
typedef struct { ... } public_type_t;

// Public function declarations
void public_function(void);

#endif // HEADER_NAME_H

// Source files (.c)
#include "header_name.h"

// Private constants
#define PRIVATE_CONSTANT 24

// Private types and variables
static private_type_t private_var;

// Private function declarations
static void private_function(void);

// Public function implementations
void public_function(void) {
    // Implementation
}

// Private function implementations
static void private_function(void) {
    // Implementation
}
```

## Advanced Topics

### Custom LVGL Widgets
```c
// Create reusable custom widgets for common UI patterns
lv_obj_t* create_data_tile(lv_obj_t* parent, const char* title, 
                          const char* units, lv_color_t color) {
    lv_obj_t* tile = lv_obj_create(parent);
    lv_obj_t* title_label = lv_label_create(tile);
    lv_obj_t* value_label = lv_label_create(tile);
    
    // Configure layout and styling...
    return tile;
}
```

### Performance Optimization
```c
// Optimize critical loops
void __attribute__((optimize("O2"))) critical_control_loop(void) {
    // Time-critical code here
}

// Use efficient data structures
typedef struct {
    uint32_t timestamp;
    uint16_t data[4];  // Packed data
} __attribute__((packed)) can_message_t;
```

### Memory Management
```c
// Static allocation for embedded systems
#define MAX_CAN_MESSAGES 32
static can_message_t message_buffer[MAX_CAN_MESSAGES];
static uint8_t message_count = 0;

// Pool-based allocation for dynamic needs
#define POOL_SIZE 1024
static uint8_t memory_pool[POOL_SIZE];
static bool pool_blocks[POOL_SIZE / 64];  // Track 64-byte blocks
```

## Integration Points

### CAN Bus Integration
- All boards communicate via CAN bus
- CSV-based message ID management ensures consistency
- Use standard 11-bit CAN IDs for efficiency
- Implement proper error handling and timeouts

### Display Integration
- Both panels share LVGL library configuration
- Use consistent color schemes and fonts
- Implement responsive layouts for different data types
- Consider performance impact of complex graphics

### Safety Integration
- ECU implements safety-critical logic
- Displays provide driver alerts and warnings
- Use redundant checking for critical parameters
- Implement fail-safe behaviors for fault conditions

## Future Enhancements

### Planned Features
- **Unit testing framework**: Comprehensive test coverage
- **Code generation**: Automatic CAN message handlers
- **OTA updates**: Over-the-air firmware updates
- **Data logging**: SD card logging capability
- **Telemetry**: Wireless data transmission

### Architecture Improvements
- **Real-time OS**: Consider FreeRTOS for complex timing
- **Modular drivers**: Plugin-based sensor architecture
- **Configuration system**: Runtime parameter adjustment
- **Diagnostics**: Built-in system health monitoring

## Cross-Platform Development

### Development Environment Setup

#### Recommended Setup (All Platforms)
1. **VS Code** with workspace file: `GauchoRacing-Firmware-26.code-workspace`
2. **CMake Tools extension** for build integration
3. **Cortex Debug extension** for debugging
4. **Platform-specific toolchain** (see below)

#### Platform-Specific Requirements

##### Linux Development
```bash
# Required packages
sudo apt install cmake ninja-build gcc-arm-none-eabi gdb-multiarch
sudo apt install openocd stlink-tools

# User permissions for hardware access
sudo usermod -a -G dialout $USER
```

##### macOS Development  
```bash
# Install via Homebrew
brew install cmake ninja gcc-arm-embedded
brew install openocd stlink

# Xcode command line tools (if not already installed)
xcode-select --install
```

##### Windows Development
1. Download and install:
   - [CMake](https://cmake.org/download/) (3.25+)
   - [Ninja](https://ninja-build.org/)
   - [ARM GCC Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
   - [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)
   
2. Add all to PATH environment variable

3. Use PowerShell or Command Prompt for development

### Cross-Platform Build Commands

The build system works identically across platforms:
```bash
# Configure (generates Ninja files)
cmake --preset=Debug

# Build all boards with parallel processing
cmake --build --preset=Debug --parallel

# Build individual board
cmake --build --preset=Debug --target ECU --parallel

# Clean build
rm -rf build      # Linux/macOS
rmdir /s build    # Windows
```

### Cross-Platform Flash Tools

Choose the appropriate tool for your platform:

| Platform | Recommended Tool | Alternative Tools |
|----------|------------------|-------------------|
| **Linux** | st-flash | OpenOCD, STM32CubeProgrammer |
| **macOS** | st-flash | OpenOCD, STM32CubeProgrammer |
| **Windows** | STM32CubeProgrammer | OpenOCD |

Example flashing commands work the same way across platforms:
```bash
# Using STM32CubeProgrammer (all platforms)
STM32_Programmer_CLI -c port=SWD -w firmware.elf -v -rst
```
