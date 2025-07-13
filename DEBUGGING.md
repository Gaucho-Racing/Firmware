# Debugging Guide

Complete debugging setup and techniques for Gaucho Racing Firmware.

## Debug Overview

This project supports comprehensive debugging for all three firmware boards using ST-Link, OpenOCD, and VS Code integration.

## Debug Tools Setup

### Required Tools
- **OpenOCD** - On-chip debugger ([Download](https://github.com/openocd-org/openocd/releases))
- **ST-Link** - STMicroelectronics debugger (included with STM32CubeIDE)
- **VS Code** - IDE with Cortex Debug extension
- **ARM GDB** - GNU debugger (included with ARM toolchain)

### VS Code Extensions
Install these essential extensions:
- **Cortex Debug** (`marus25.cortex-debug`) - ARM debugging support
- **CMake Tools** (`ms-vscode.cmake-tools`) - Build integration
- **C/C++** (`ms-vscode.cpptools`) - Language support

Optional but recommended:
- **Serial Monitor** (`ms-vscode.vscode-serial-monitor`) - UART debugging
- **Hex Editor** (`ms-vscode.hexeditor`) - Binary file inspection

### Verify Installation
```bash
openocd --version     # Should show OpenOCD version
st-util --version     # Should show ST-Link utilities
arm-none-eabi-gdb --version  # Should show ARM GDB
```

## Quick Debug Start

### 1. Build Firmware with Debug Info
```bash
# Configure for debug (includes debug symbols)
cmake --preset=debug

# Build the board you want to debug
cmake --build --preset=debug --target ECU
cmake --build --preset=debug --target DashPanel  
cmake --build --preset=debug --target SteeringPanel
```

### 2. Start Debugging in VS Code
1. **Open Run and Debug** (`Ctrl+Shift+D`)
2. **Select debug configuration**:
   - "Debug ECU"
   - "Debug DashPanel (ST-Link)" or "Debug DashPanel (ST-Util)"
   - "Debug SteeringPanel (ST-Link)" or "Debug SteeringPanel (ST-Util)"
3. **Start debugging** (`F5`)

## Debug Configurations

### ECU Debug Configuration
```json
{
    "name": "Debug ECU",
    "type": "cortex-debug",
    "servertype": "openocd",
    "device": "STM32G474RE",
    "executable": "${workspaceFolder}/ECU/build/ECU.elf",
    "configFiles": ["interface/stlink.cfg", "target/stm32g4x.cfg"],
    "svdFile": "${workspaceFolder}/lib/svd/STM32G474.svd"
}
```

### Panel Debug Configurations
Both DashPanel and SteeringPanel use similar configurations:
```json
{
    "name": "Debug DashPanel (ST-Link)",
    "type": "cortex-debug", 
    "servertype": "stlink",
    "device": "STM32U599NJ",
    "executable": "${workspaceFolder}/DashPanel/build/DashPanel.elf",
    "svdFile": "${workspaceFolder}/lib/svd/STM32U5A9.svd"
}
```

## Debug Features

### Breakpoints
- **Line breakpoints**: Click left margin or `F9`
- **Conditional breakpoints**: Right-click breakpoint → Edit condition
- **Function breakpoints**: Debug Console → `break function_name`

### Variable Inspection
- **Watch expressions**: Add variables to Watch panel
- **Local variables**: Automatically shown in Variables panel  
- **Memory viewer**: Right-click variable → View as memory
- **Register viewer**: Peripheral registers via SVD files

### Step Controls
- **Continue** (`F5`): Run until next breakpoint
- **Step Over** (`F10`): Execute current line, don't enter functions
- **Step Into** (`F11`): Enter function calls
- **Step Out** (`Shift+F11`): Exit current function

## SWO (Serial Wire Output) Debugging

### Enable SWO Output
SWO provides real-time debug output without halting execution:

```c
// In your code (ECU example)
#include "main.h"

// Send debug message via SWO
void debug_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // Send via SWO ITM port 0
    for (char* p = buffer; *p; p++) {
        ITM_SendChar(*p);
    }
}
```

### SWO Configuration
All debug configurations include SWO setup:
```json
"swoConfig": {
    "enabled": true,
    "cpuFrequency": 160000000,
    "swoFrequency": 115200,
    "source": "probe",
    "decoders": [
        {
            "type": "console",
            "label": "ITM",
            "port": 0
        }
    ]
}
```

## Debug Techniques

### State Machine Debugging (ECU)
```c
// Add state logging in state machine
typedef enum {
    STATE_INIT,
    STATE_READY, 
    STATE_DRIVING,
    STATE_ERROR
} vehicle_state_t;

volatile vehicle_state_t current_state = STATE_INIT;

void state_machine_update() {
    vehicle_state_t prev_state = current_state;
    
    // State logic here...
    
    // Log state changes via SWO
    if (current_state != prev_state) {
        debug_printf("State: %s -> %s\n", 
                    state_to_string(prev_state),
                    state_to_string(current_state));
    }
}
```

### CAN Message Debugging
```c
// Log CAN messages
void can_debug_message(uint32_t id, uint8_t* data, uint8_t len) {
    debug_printf("CAN TX: ID=0x%03X Data=", id);
    for (int i = 0; i < len; i++) {
        debug_printf("%02X ", data[i]);
    }
    debug_printf("\n");
}
```

### LVGL Debugging (Panels)
```c
// Enable LVGL logging
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

// Log callback for LVGL
void lv_log_print_g_cb(const char* buf) {
    debug_printf("LVGL: %s", buf);
}
```

### Memory Debugging
```c
// Check stack usage
void check_stack_usage() {
    extern uint32_t _estack;
    uint32_t* stack_ptr = (uint32_t*)__get_MSP();
    uint32_t stack_used = (uint32_t)&_estack - (uint32_t)stack_ptr;
    debug_printf("Stack used: %lu bytes\n", stack_used);
}

// Check heap usage (with FreeRTOS)
void check_heap_usage() {
    debug_printf("Heap free: %u bytes\n", xPortGetFreeHeapSize());
    debug_printf("Heap min: %u bytes\n", xPortGetMinimumEverFreeHeapSize());
}
```

## Advanced Debugging

### Performance Analysis
```c
// Timing measurements
uint32_t start_time = DWT_CYCCNT;
// ... code to measure ...
uint32_t end_time = DWT_CYCCNT;
uint32_t cycles = end_time - start_time;
debug_printf("Function took %lu cycles\n", cycles);
```

### Fault Handlers
```c
// Hard fault handler with debugging info
void HardFault_Handler(void) {
    debug_printf("Hard Fault! PC: 0x%08X\n", 
                 ((uint32_t*)__get_MSP())[6]);
    __BKPT(0);  // Trigger debugger breakpoint
    while(1);
}
```

### Live Variable Monitoring
Use VS Code's Watch panel to monitor:
- `current_state` - Vehicle state machine
- `can_rx_count` - CAN message counters  
- `lvgl_fps` - Display frame rate
- `heap_free` - Available memory

## Hardware Debug Setup

### ST-Link Connection
```
ST-Link V2/V3 → Target Board
VCC    → 3.3V
GND    → GND  
SWDIO  → SWDIO
SWCLK  → SWCLK
SWO    → SWO (for SWO debugging)
```

### Multiple Boards
Each board needs its own ST-Link. Configure different serial numbers in debug configurations:
```json
"serialNumber": "066BFF373349504157193520"  // Unique per ST-Link
```

## Cross-Platform Debug Setup

### Linux Setup
```bash
# Install required packages
sudo apt update
sudo apt install openocd stlink-tools gdb-multiarch

# Add user to dialout group (for ST-Link access)
sudo usermod -a -G dialout $USER
# Log out and back in for group changes to take effect

# Verify ST-Link connection
st-info --probe
```

### macOS Setup  
```bash
# Install via Homebrew
brew install openocd stlink

# No additional permissions needed
# Verify ST-Link connection
st-info --probe
```

### Windows Setup
1. **Download and install**:
   - [OpenOCD](https://github.com/openocd-org/openocd/releases) - Extract to `C:\openocd`
   - [ST-Link Utilities](https://www.st.com/en/development-tools/stsw-link004.html)
   - Add both to PATH environment variable

2. **Install ST-Link drivers**:
   - Use STM32CubeProgrammer installer (includes drivers)
   - Or download ST-Link driver package separately

3. **Verify installation**:
   ```cmd
   openocd --version
   STM32_Programmer_CLI --version
   ```

### Hardware Connection (All Platforms)
Connect ST-Link V2/V3 to target board SWD pins:

#### For STM32U5 (DashPanel, SteeringPanel)
- **SWDIO** → PA13
- **SWCLK** → PA14  
- **GND** → GND
- **VCC** → 3.3V (optional, if target not self-powered)

#### For STM32G4 (ECU)
- **SWDIO** → PA13
- **SWCLK** → PA14
- **GND** → GND  
- **VCC** → 3.3V (optional, if target not self-powered)

## Troubleshooting

### Connection Issues

#### "Could not connect to target"
**Solutions**:
- Check ST-Link connection and cables
- Verify target board power
- Try different USB port
- Update ST-Link firmware

#### "OpenOCD failed to start"
**Solutions**:
- Check OpenOCD installation and PATH
- Verify config files exist
- Try different OpenOCD version
- Check for conflicting processes

#### "Target not found"
**Solutions**:
- Verify correct device name in configuration
- Check SWD connection (SWDIO, SWCLK, GND)
- Ensure target MCU is powered and not in reset

### Debug Performance Issues

#### Slow debugging
**Solutions**:
- Use release build for faster execution
- Disable unnecessary SWO output
- Reduce breakpoint frequency
- Use conditional breakpoints

#### Lost debug connection
**Solutions**:
- Check for infinite loops without breakpoints
- Verify watchdog timer handling
- Use SWO instead of halting debugger

### SWO Issues

#### No SWO output
**Solutions**:
- Check SWO pin connection
- Verify CPU frequency in configuration
- Enable ITM in code: `CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk`
- Check SWO frequency compatibility

## Debug Best Practices

### Code Practices
1. **Use debug builds** for development (include `-g` flag)
2. **Add logging strategically** - critical state changes and errors
3. **Initialize debug early** in main() before other initialization
4. **Use assertions** for catching logic errors during development

### Debugging Workflow
1. **Reproduce issue** consistently 
2. **Add logging** around problematic area
3. **Set breakpoints** at key decision points
4. **Step through** execution to understand flow
5. **Watch variables** to see data changes
6. **Fix root cause** not just symptoms

### Performance Debugging
1. **Profile critical paths** using cycle counters
2. **Monitor memory usage** regularly
3. **Check interrupt latency** for real-time requirements
4. **Validate timing requirements** with oscilloscope/logic analyzer

## External Debug Tools

### Hardware Analyzers
- **Logic Analyzer**: CAN bus, SPI, I2C protocol analysis
- **Oscilloscope**: Signal timing and quality
- **CAN Analyzer**: Dedicated CAN message monitoring

### Software Tools
- **STM32CubeMonitor**: Real-time variable monitoring
- **STM32CubeProgrammer**: Flash programming and option bytes
- **ARM Development Studio**: Advanced profiling and optimization
