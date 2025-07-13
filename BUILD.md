# Build Instructions

Complete build guide for the Gaucho Racing Firmware Suite.

## Prerequisites

### Required Tools
- **CMake 3.25+** - Build system ([Download](https://cmake.org/download/))
- **Ninja** - Fast build system ([Download](https://ninja-build.org/))
- **ARM GCC Toolchain** - Embedded compilation ([Download](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads))
- **Git** - Version control
- **VS Code** (recommended) - IDE with debugging support

### Installation by Platform

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install cmake ninja-build git gcc-arm-none-eabi
```

#### macOS
```bash
brew install cmake ninja git
brew install --cask gcc-arm-embedded
```

#### Windows
1. Download and install [CMake](https://cmake.org/download/)
2. Download and install [Ninja](https://ninja-build.org/)
3. Download and install [ARM GCC Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
4. Add all tools to your PATH environment variable

### Verify Installation
```bash
cmake --version          # Should show 3.25+
ninja --version          # Should show ninja version
arm-none-eabi-gcc --version   # Should show ARM GCC toolchain
git --version            # Should show git version
```

## Quick Start

### 1. Clone and Setup
```bash
git clone <repository-url>
cd Firmware-26
```

### 2. Open in VS Code
```bash
code .
```

### 3. Configure and Build
1. **Open VS Code** and the project will auto-configure using CMake extension
2. **Select preset**: The CMake extension will use the `debug` preset by default
3. **Build all firmware**: Use `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Boards"

## Build Targets

### Firmware Targets
| Target | Output | Description |
|--------|--------|-------------|
| `DashPanel` | `DashPanel/build/DashPanel.elf` | Dashboard Panel firmware |
| `SteeringPanel` | `SteeringPanel/build/SteeringPanel.elf` | Steering Panel firmware |
| `ECU` | `ECU/build/ECU.elf` | ECU firmware |

### Utility Targets
| Target | Description |
|--------|-------------|
| `all-boards` | Build all firmware boards |
| `generateCSVHeaders` | Generate CAN message headers |
| `gaucho_utils` | Build shared utilities library |

## Build Presets

VS Code automatically configures builds with different optimization levels using the CMake extension:

- **Debug** - Full debug info, no optimization (default)
- **Release** - Full optimization, no debug info  
- **MinSizeRel** - Size-optimized build
- **RelWithDebInfo** - Optimized with debug info

**To change preset in VS Code**:
1. Open Command Palette (`Ctrl+Shift+P`)
2. Type "CMake: Select Configure Preset"  
3. Choose your desired preset

## Modern Build System Features

### Incremental Builds
The ExternalProject-based build system only rebuilds changed components:

**In VS Code**:
1. Make code changes to any board
2. Run "Build All Boards" task (`Ctrl+Shift+P` → "Tasks: Run Task")
3. Only changed components rebuild (much faster than initial build)

### Parallel Builds
Multiple boards build simultaneously with Ninja's fast parallel execution using VS Code tasks:

**Build all boards in parallel**:
- `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Boards"
- Builds DashPanel, SteeringPanel, and ECU simultaneously

**Build individual boards**:
- `Ctrl+Shift+P` → "Tasks: Run Task" → "Build DashPanel"
- `Ctrl+Shift+P` → "Tasks: Run Task" → "Build SteeringPanel"  
- `Ctrl+Shift+P` → "Tasks: Run Task" → "Build ECU"

Ninja automatically uses all available CPU cores for optimal build performance.

### Dependency Management
- CSV headers automatically generated before firmware builds
- Shared library changes trigger appropriate rebuilds
- Proper dependency tracking across all components

## Performance Benefits

### Ninja vs Make Comparison
| Metric | Make | Ninja | Improvement |
|--------|------|-------|-------------|
| **Clean Build Time** | ~4-5 minutes | ~2-3 minutes | ~40% faster |
| **Incremental Build** | ~30-60 seconds | ~10-30 seconds | ~50% faster |  
| **Dependency Tracking** | Good | Excellent | Better reliability |
| **Parallel Efficiency** | Moderate | Excellent | Better CPU utilization |
| **Build Status** | Basic | Detailed | Better progress info |

### Example Build Output
```
[1/553] Building C object CMakeFiles/DashPanel.dir/Core/Src/main.c.obj
[2/553] Building C object CMakeFiles/DashPanel.dir/Core/Src/gpio.c.obj
[3/553] Building C object CMakeFiles/DashPanel.dir/Application/Src/gui.c.obj
...
[553/553] Linking C executable DashPanel.elf
```

Notice the parallel compilation indicators and progress tracking!

## VS Code Integration

### Primary Development Workflow
VS Code is the recommended development environment with full integration:

### Build Tasks
Access via `Ctrl+Shift+P` → "Tasks: Run Task":

- **"Build All Boards"** - Builds all firmware (default, recommended)
- **"Build DashPanel"** - Dashboard Panel firmware only
- **"Build SteeringPanel"** - Steering Panel firmware only  
- **"Build ECU"** - ECU firmware only
- **"Generate CSV Headers"** - Regenerate CAN message headers
- **"Clean All"** - Remove all build artifacts

### CMake Extension Integration
The CMake extension provides additional functionality:

1. **Auto-Configuration**: Project configures automatically when opened
2. **Build Status**: Status bar shows current kit, build type, and target
3. **Build Targets**: Available in the CMake extension panel
4. **Quick Build**: `F7` for default build
5. **IntelliSense**: Automatic code completion and error checking

### CMake Extension Commands
- `Ctrl+Shift+P` → "CMake: Configure" - Reconfigure project
- `Ctrl+Shift+P` → "CMake: Build" - Build default target
- `Ctrl+Shift+P` → "CMake: Build Target" - Choose specific target
- `Ctrl+Shift+P` → "CMake: Clean" - Clean build artifacts

### Development Status Bar
The VS Code status bar displays:
- **CMake Kit**: GCC Host/ARM GCC toolchain
- **Build Type**: Debug/Release/etc.
- **Active Target**: Current build target
- **Build Status**: Success/failure indicators

## Cleaning Builds

### VS Code Tasks
- `Ctrl+Shift+P` → "Tasks: Run Task" → "Clean All"

This removes all build artifacts from all boards automatically.

### Alternative: Command Line Clean
If needed for troubleshooting:
```bash
# Remove all build artifacts
rm -rf build DashPanel/build SteeringPanel/build ECU/build
```

## Memory Usage

Typical memory usage per board (debug builds):

| Board | Flash | RAM | MCU Target |
|-------|-------|-----|------------|
| **DashPanel** | ~390KB (9.3%) | ~1.9MB (74%) | STM32U599NJ |
| **SteeringPanel** | ~250KB (6.0%) | ~2.5MB (97%) | STM32U599NJ |
| **ECU** | ~60KB (11.5%) | ~5.5KB (4%) | STM32G474RE |

*Release builds use ~30% less flash and ~20% less RAM.*

## Alternative Build Methods

### Method 1: VS Code Tasks (Recommended)
**Primary workflow for daily development:**
1. Open project in VS Code
2. Use `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Boards"
3. VS Code handles configuration and parallel building automatically

### Method 2: CMake Extension
**For advanced CMake users:**
1. Use CMake extension panel in VS Code
2. Select build targets from the extension UI
3. Monitor build progress in the extension

### Method 3: Command Line (Troubleshooting Only)
**For debugging build issues or CI/CD:**
```bash
# Configure and build via command line
cmake --preset=debug
cmake --build --preset=debug --target all-boards --parallel

# Or build boards individually
cd DashPanel && cmake -S . -B build -G Ninja && cmake --build build
cd SteeringPanel && cmake -S . -B build -G Ninja && cmake --build build  
cd ECU && cmake -S . -B build -G Ninja && cmake --build build
```

## Troubleshooting

### Common Issues

#### "ninja: command not found"
**Solution**: Install Ninja build system
```bash
# Linux/Ubuntu/Debian
sudo apt install ninja-build

# macOS
brew install ninja

# Windows
# Download from https://ninja-build.org/
# Add to PATH environment variable
```

#### "arm-none-eabi-gcc not found"
**Solution**: Install ARM GCC toolchain and add to PATH
```bash
# Verify installation
arm-none-eabi-gcc --version
```

#### "CMake version too old"
**Solution**: Update to CMake 3.25+
```bash
# Check version
cmake --version

# Ubuntu/Debian: Use kitware repository for latest cmake
```

#### "No such file or directory: build/debug"
**Solution**: Configure project first in VS Code
- `Ctrl+Shift+P` → "CMake: Configure"
- Or run "Build All Boards" task which auto-configures

#### Permission denied errors (Windows)
**Solution**: Run terminal as administrator or check antivirus settings

#### Linking errors (e.g., "undefined reference")
**Solutions**:
- **In VS Code**: Run "Clean All" task then "Build All Boards"
- Check for missing STM32CubeMX regeneration
- Ensure all required source files are included in board CMakeLists.txt
- Verify correct linker script and HAL configuration

#### Disk Space Issues
- **In VS Code**: Run "Clean All" task to remove old builds
- Use release builds for final testing (smaller than debug)

### Build Performance

#### Fast Builds with Ninja
- **Ninja generator**: Faster than Make, enabled by default
- **Parallel builds**: Use `--parallel` flag: `cmake --build --preset=debug --parallel`
- **Incremental builds**: Ninja tracks dependencies precisely, minimal rebuilds
- **ccache support**: Use ccache if available: `export CC="ccache arm-none-eabi-gcc"`

#### Build Speed Optimization
- **Clean builds**: ~2-3 minutes (all boards, parallel)
- **Incremental builds**: ~10-30 seconds (changed files only)
- **Memory usage**: 4-8GB RAM recommended for parallel builds

#### Disk Space Issues  
- **In VS Code**: Run "Clean All" task to remove old builds
- Use release builds for final testing (smaller than debug)

## Development Workflow

### Typical Development Cycle (VS Code)
1. **Make code changes** to any board (e.g., ECU/Application/Src/state_machine.c)
2. **Build affected board**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Build ECU"
3. **Flash and test changes** (see flashing section below)
4. **Build all before committing**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Boards"

### Feature Development Workflow
1. **Create feature branch**: Use VS Code's Git integration or terminal
2. **Develop incrementally**: Use VS Code build tasks for fast iteration
3. **Test regularly**: Use VS Code's integrated debugging and flashing
4. **Final validation**: Run "Build All Boards" task to ensure everything compiles

## Flashing Firmware

Flash compiled firmware to STM32 microcontrollers using various tools.

### Prerequisites for Flashing

You'll need one of these flashing tools installed:

#### Option 1: ST-Link Tools (Recommended for Linux/macOS)
- **Linux**: `sudo apt install stlink-tools`
- **macOS**: `brew install stlink` 
- **Windows**: Download [ST-Link Utility](https://www.st.com/en/development-tools/stsw-link004.html)

#### Option 2: OpenOCD (Cross-platform)
- **Linux**: `sudo apt install openocd`
- **macOS**: `brew install openocd`
- **Windows**: Download from [OpenOCD website](https://openocd.org/)

#### Option 3: STM32CubeProgrammer (Official ST Tool)
- Download from [STMicroelectronics](https://www.st.com/en/development-tools/stm32cubeprog.html)
- Available for Linux, macOS, and Windows
- Provides both GUI and CLI (`STM32_Programmer_CLI`)

### Quick Flash Commands

#### Using st-flash (Linux/macOS)
```bash
# Flash DashPanel
st-flash write build/debug/DashPanel-prefix/src/DashPanel-build/DashPanel.bin 0x08000000

# Flash SteeringPanel  
st-flash write build/debug/SteeringPanel-prefix/src/SteeringPanel-build/SteeringPanel.bin 0x08000000

# Flash ECU
st-flash write build/debug/ECU-prefix/src/ECU-build/ECU.bin 0x08000000
```

#### Using OpenOCD (All platforms)
```bash
# Flash DashPanel (STM32U5)
openocd -f interface/stlink-v2-1.cfg -f target/stm32u5x.cfg -c "program build/debug/DashPanel-prefix/src/DashPanel-build/DashPanel.elf verify reset exit"

# Flash SteeringPanel (STM32U5)
openocd -f interface/stlink-v2-1.cfg -f target/stm32u5x.cfg -c "program build/debug/SteeringPanel-prefix/src/SteeringPanel-build/SteeringPanel.elf verify reset exit"

# Flash ECU (STM32G4)
openocd -f interface/stlink-v2-1.cfg -f target/stm32g4x.cfg -c "program build/debug/ECU-prefix/src/ECU-build/ECU.elf verify reset exit"
```

#### Using STM32CubeProgrammer (All platforms)
```bash
# Flash DashPanel
STM32_Programmer_CLI -c port=SWD -w build/debug/DashPanel-prefix/src/DashPanel-build/DashPanel.elf -v -rst

# Flash SteeringPanel
STM32_Programmer_CLI -c port=SWD -w build/debug/SteeringPanel-prefix/src/SteeringPanel-build/SteeringPanel.elf -v -rst

# Flash ECU
STM32_Programmer_CLI -c port=SWD -w build/debug/ECU-prefix/src/ECU-build/ECU.elf -v -rst
```

### VS Code Integration (Recommended)

Flash firmware directly from VS Code using the configured tasks:

1. **Access flashing tasks**: `Ctrl+Shift+P` → "Tasks: Run Task"
2. **Select flashing method**:
   - **"Flash DashPanel (st-flash)"** - Using st-link tools
   - **"Flash DashPanel (OpenOCD)"** - Using OpenOCD
   - **"Flash DashPanel (STM32CubeProgrammer)"** - Using ST's official tool
   - Similar tasks available for SteeringPanel and ECU

**Benefits of VS Code flashing**:
- Automatic dependency handling (builds before flashing)
- Integrated with build system
- Cross-platform compatibility
- Error reporting in VS Code's Problems panel

### Platform-Specific Notes

#### Linux
- Add user to `dialout` group: `sudo usermod -a -G dialout $USER`
- Install udev rules for ST-Link: `sudo apt install stlink-tools`
- Log out and back in after group changes

#### macOS
- No special setup required after installing tools via Homebrew
- ST-Link drivers included with stlink package

#### Windows
- Install ST-Link drivers when prompted
- Add tool directories to PATH environment variable
- Use PowerShell or Command Prompt with administrator privileges if needed

### Hardware Connection

#### STM32U5 Boards (DashPanel, SteeringPanel)
- Connect ST-Link V2/V3 to SWD pins:
  - **SWDIO** → PA13
  - **SWCLK** → PA14  
  - **GND** → GND
  - **VCC** → 3.3V

#### STM32G4 Board (ECU)
- Connect ST-Link V2/V3 to SWD pins:
  - **SWDIO** → PA13
  - **SWCLK** → PA14
  - **GND** → GND
  - **VCC** → 3.3V

### Troubleshooting Flash Issues

#### "No device found"
**Solutions**:
- Check physical ST-Link connection
- Verify power to target board
- Try different USB cable/port
- Update ST-Link firmware

#### "Cannot connect to target"
**Solutions**:
- Check SWD pin connections (SWDIO, SWCLK)
- Verify target is powered (3.3V)
- Reset target board
- Try lower SWD frequency: `openocd -f interface/stlink-v2-1.cfg -c "adapter speed 1000"`

#### "Permission denied" (Linux)
**Solutions**:
- Add user to dialout group: `sudo usermod -a -G dialout $USER`
- Run with sudo (not recommended): `sudo st-flash ...`
- Check udev rules: `/etc/udev/rules.d/`

#### "Flash verification failed"
**Solutions**:
- Erase flash first: `st-flash erase`
- Try different flashing tool
- Check for board hardware issues
- Verify correct firmware for target board

### Build and Flash Workflow

**Recommended VS Code workflow**:
1. **Build firmware**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Build ECU" 
2. **Flash immediately**: `Ctrl+Shift+P` → "Tasks: Run Task" → "Flash ECU (st-flash)"
3. **Monitor output**: Use VS Code's integrated terminal or serial monitor

**Alternative command line** (for CI/CD or troubleshooting):
```bash
# 1. Build firmware
cmake --build --preset=debug --target ECU --parallel

# 2. Flash immediately (using st-flash)
st-flash write build/debug/ECU-prefix/src/ECU-build/ECU.bin 0x08000000

# 3. Monitor serial output (optional)
# See DEBUGGING.md for serial monitor setup
```

---

## Command Line Quick Reference

**For automated builds, CI/CD, or troubleshooting only. VS Code is recommended for daily development.**

### Basic Commands
```bash
# Configure project
cmake --preset=debug

# Build all boards
cmake --build --preset=debug --target all-boards --parallel

# Build individual boards  
cmake --build --preset=debug --target DashPanel --parallel
cmake --build --preset=debug --target SteeringPanel --parallel
cmake --build --preset=debug --target ECU --parallel

# Clean all builds
rm -rf build DashPanel/build SteeringPanel/build ECU/build
```

### Quick Flash Commands
```bash
# Flash using st-flash (Linux/macOS)
st-flash write build/debug/DashPanel-prefix/src/DashPanel-build/DashPanel.bin 0x08000000
st-flash write build/debug/SteeringPanel-prefix/src/SteeringPanel-build/SteeringPanel.bin 0x08000000
st-flash write build/debug/ECU-prefix/src/ECU-build/ECU.bin 0x08000000

# Flash using STM32CubeProgrammer (All platforms)
STM32_Programmer_CLI -c port=SWD -w build/debug/DashPanel-prefix/src/DashPanel-build/DashPanel.elf -v -rst
```
