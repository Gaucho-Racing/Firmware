# GR Firmware

[![CMake](https://github.com/Gaucho-Racing/Firmware/actions/workflows/BuildAllPresets.yml/badge.svg)](https://github.com/Gaucho-Racing/Firmware/actions/workflows/BuildAllPresets.yml)
[![CTest](https://github.com/Gaucho-Racing/Firmware/actions/workflows/RunCTests.yml/badge.svg)](https://github.com/Gaucho-Racing/Firmware/actions/workflows/RunCTests.yml)
[![Auto Format](https://github.com/Gaucho-Racing/Firmware/actions/workflows/AutoFormat.yml/badge.svg)](https://github.com/Gaucho-Racing/Firmware/actions/workflows/AutoFormat.yml)
[![ValidateConfigs](https://github.com/Gaucho-Racing/Firmware/actions/workflows/ValidateConfigs.yml/badge.svg)](https://github.com/Gaucho-Racing/Firmware/actions/workflows/ValidateConfigs.yml)

**_DO NOT DELETE ANY LICENSE INFORMATION ON ANY COPIED FILE OR LINE OF CODE_**

When able, please add VS Code comptatible doc comments! These will help debugging and appear on the tool tip (hover) of any known function.

## Install Tools/Dependencies
*For Windows:*
1. `winget install Ninja-build.Ninja Kitware.CMake Arm.GnuArmEmbeddedToolchain`
2. [OpenOCD](https://github.com/openocd-org/openocd/releases/latest) -> Download the `.tar.gz` file, extract to directory, add the bin directory to your `PATH`
3. Relaunch your terminal

*For MacOS:*
1. `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
2. `brew install cmake ninja open-ocd`
3. `brew install --cask gcc-arm-embedded`
4. Relaunch your terminal

*For Linux / BSD:*
* `sudo apt install cmake ninja gcc-arm-none-eabi openocd`
* `sudo dnf install cmake ninja openocd arm-none-eabi-gcc`
* `sudo pacman -S cmake ninja openocd arm-none-eabi-gcc`
* `pkg install cmake ninja openocd arm-none-eabi-gcc`
* If you run into issues verify that binutils and newlib are installed for `arm-none-eabi`

### Verify that you have all these dependencies installed:
- CMake - `cmake --version`
- Ninja - `ninja --version`
- Arm - `arm-none-eabi-gcc --version`
- OpenOCD - `openocd --version`

**CMake** (metabuild system) --> **Ninja** (build system) --> **Arm Toolchain** (provides tools to compile & link code)
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
example: your_chip = stm32l4x, your_project = L4BLINKY <br>
example: your_chip = stm32g4x, your_project = G4BLINKY

This openocd command loads a configuration file 'target/{your_chip}.cfg' (defined in launch.json), and uses that to flash the '{your_project}.elf' executable onto the microcontroller.

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
