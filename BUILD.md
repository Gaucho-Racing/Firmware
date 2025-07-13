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
| `ECU` | `ECU/build/ECU.elf` | ECU firmware |
| `ABCXYZ` | `ABCXYZ/build/ABCXYZ.elf` | ABCXYZ firmware |

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

## VS Code Integration

### Build Tasks
Access via `Ctrl+Shift+P` → "Tasks: Run Task":

- **"Build All Boards"** - Builds all firmware (default, recommended)
- **"Build ABCXYZ"** - ABCXYZ firmware only
- **"Generate CSV Headers"** - Regenerate CAN message headers
- **"Clean All"** - Remove all build artifacts

### CMake Extension Commands
- `Ctrl+Shift+P` → "CMake: Configure" - Reconfigure project
- `Ctrl+Shift+P` → "CMake: Build" - Build default target
- `Ctrl+Shift+P` → "CMake: Build Target" - Choose specific target
- `Ctrl+Shift+P` → "CMake: Clean" - Clean build artifacts

### Command Line
If needed for troubleshooting:
```bash
# Remove all build artifacts
rm -rf build */build
```

```bash
# Configure and build via command line
cmake --preset=Debug
cmake --build --preset=Debug --target all-boards --parallel

# Or build board ABCXYZ individually
cd ABCXYZ && cmake -S . -B build -G Ninja && cmake --build build
```
