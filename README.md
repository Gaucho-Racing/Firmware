# GR Firmware

**_DO NOT DELETE ANY LICENSE INFORMATION ON ANY COPIED FILE OR LINE OF CODE_**

# REPO RULES (follow if you want your builds to work)
- Add a folder for each board with Core and Application, with Src and Inc in both
- Do not question naming conventions please
- Each project needs it's own directory where the name of the directory is the name of the project/executable
  - this is because I am lazy and wanted to hardcode stuff for `add_GR_project` (see `gr-lib.cmake`)

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
