# GR Firmware

[![CMake](https://github.com/Gaucho-Racing/Firmware/actions/workflows/BuildAllPresets.yml/badge.svg)](https://github.com/Gaucho-Racing/Firmware/actions/workflows/BuildAllPresets.yml)
[![CTest](https://github.com/Gaucho-Racing/Firmware/actions/workflows/RunCTests.yml/badge.svg)](https://github.com/Gaucho-Racing/Firmware/actions/workflows/RunCTests.yml)
[![ClangFormat](https://github.com/Gaucho-Racing/Firmware/actions/workflows/ClangFormat.yml/badge.svg)](https://github.com/Gaucho-Racing/Firmware/actions/workflows/ClangFormat.yml)
[![ValidateConfigs](https://github.com/Gaucho-Racing/Firmware/actions/workflows/ValidateConfigs.yml/badge.svg)](https://github.com/Gaucho-Racing/Firmware/actions/workflows/ValidateConfigs.yml)

**_DO NOT DELETE ANY LICENSE INFORMATION ON ANY COPIED FILE OR LINE OF CODE_**

When able, please add VS Code comptatible doc comments! These will help debugging and appear on the tool tip (hover) of any known function.

## Install Tools/Dependencies
*For Windows:*
1. [CMake](https://cmake.org/download/)
2. [Ninja](https://github.com/ninja-build/ninja/releases)
3. [ARM Toolchain (arm-none-eabi)](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
4. [OpenOCD](https://github.com/openocd-org/openocd/releases/tag/v0.12.0)

*For MacOS:*
1. Install Homebrew (paste this into terminal): `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
2. `brew install cmake`
3. `brew install ninja`
4. [ARM Toolchain (arm-none-eabi)](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- Look under "AArch32 bare-metal target (arm-none-eabi)" and install the .pkg file 
- Add the ARM Toolchain to your PATH variable
- `nano ~/.zshrc`
- `export PATH="/Applications/ArmGNUToolchain/14.3.rel1/arm-none-eabi/bin:$PATH"` 
- `source ~/.zshrc`
5. `brew install open-ocd`

**Note:** DO NOT use Homebrew to install "arm-none-eabi-gcc"- their ARM toolchain package is broken

Verify that you have all these dependencies installed:
- CMAKE - `cmake --version`
- ninja - `ninja --version`
- ARM toolchain - `arm-none-eabi-gcc --version`
- openocd - `openocd --version`

**CMake** (metabuild system) --> **Ninja** (build system) --> **ARM Toolchain** (provides tools to compile & link code)
**OpenOCD** is an on-chip debugger, which allows us to examine code line-by-line in Debug mode

# REPO RULES (follow if you want your builds to work)
- Add a folder for each board with Core and Application, with Src and Inc in both
- Do not question naming conventions please
- Each project needs it's own directory where the name of the directory is the name of the project/executable
  - this is because I am lazy and wanted to hardcode stuff for `add_GR_project` (see `gr-lib.cmake`)

If you do not want GitHub Action emails please turn off notifications on your side. See something like https://stackoverflow.com/q/66116203

Rebase please, run `git config pull.rebase true`

# Testing

## Setup
While setting up your project using `add_GR_project`, have something that looks like this inside of your project's top level CMakeFile.txt
```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Test")
    include_directories(
        Application/Inc
        Core/Inc
    )

    # Can have singleton files
    add_executable(basic_application_hootl_test
        Application/Test/basic_application_hootl_test.c
    )
    add_test(basic_app basic_application_hootl_test)

    # Can link files
    add_executable(advanced_application_hootl_test
        Application/Test/advanced_application_hootl_test.c
        Application/Src/appdemo.c
    )
    add_test(advanced_app advanced_application_hootl_test)

    ...
endif()
```

Setup each of the executables as an independent program with their own `int main(int argc, char *argv[])` that links from other files as needed.

## Running

```bash
cmake --preset HOOTLTest
cmake --build build/HOOTLTest
cd build/HOOTLTest && ctest --output-on-failure
```

```bash
cmake --preset Release
cmake --build build/Release
cd build/Release
openocd -f interface/stlink.cfg -f target/{your_chip}.cfg -c "program {your_project}.elf verify reset exit"
```
example: your_chip = stm32l4x, your_project = L4BLINKY
example: your_chip = stm32g4x, your_project = G4BLINKY

If things aren't working on MacOS, read through this [wiki page](https://wiki.gauchoracing.com/books/gr26-architecture/page/cmake-notes-for-monorepo)

## VS Code Setup

Put into `.vscode/settings.json` the following starter template:
```json
{
    "files.associations": {
        "*.h": "c",
        "*.c": "c"
    },
}

```

## Chip Path Contamination
Things get moved around and we should probably have some better solution than having CHIP defined in chip.cmake and such

Bad because if there is a change in one of the chip.cmake then we need to manually / physically copy it to all of the others which is not using smart thinking

---

---

---

# LEGACY ACU-25 CONTENT

## Setup
`launch.json`:
    ```json
    "configurations": [
        {
            "name": "Debug with OpenOCD",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/build/Debug/ACU-25.elf",
            "type": "cortex-debug",
            "device": "STM32G474RE",
            "servertype": "openocd",
            "configFiles": [
                "interface/stlink.cfg",
                "target/stm32g4x.cfg"
            ],
            "searchDir": [],
            "preLaunchTask": "CMake: build",
            "showDevDebugOutput": "none",
            "svdPath": "${workspaceFolder}/.vscode/STM32G474.svd" // optional
        }
    ]
    ```

`settings.json`:
- add `"vscode-serial-monitor.customBaudRates": [1000000]`

`c_cpp_properties.json`:
- ensure you have the correct compiler path settings to `arm-none-eabi`: `"compilerPath": "/opt/homebrew/bin/arm-none-eabi-gcc",` (MacOS)

## Build & Debug
- Press Build
- Press Debug
    - if Debug doesn't work, open terminal in VSCode and run
    - `openocd -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program build/Debug/ACU-25.elf verify reset exit"`
    - if it still doesn't work, try removing the 'build' folder and configuring and rebuilding, sometimes cache stuff can screw u over

<!-- TODO: Create a section about how to make a new project from scratch, also probably explain how the infrastructure lets it work -->
