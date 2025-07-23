# ACU-25 [STM32 Version]

-----

## Desription
Transferring from Teensy to STM32

----

## Setup

`launch.json`:

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

## Tree
---
#### Application/
---
```
- Inc/
    - acu.h       -> struct defs & function protos for ACU, CAN functions too
    - battery.h   -> struct defs & function protos for BCC-related things (Battery)
    - config.h    -> definitions & macros, CAN ID mapping
    - debug.h     -> not really used rn, but maybe later
    - mcu.h       -> functions definitions for extern functions in BCC SDK
    - state.h     -> struct defs & fcuntion protos for ACU state machine
    - others      -> from BCC SDK
- Src/
    - acu.c       -> see acu.h
    - battery.c   -> see battery.h
    - debug.c     -> see debug.h
    - mcu.c       -> see mcu.h
    - state.c     -> see state.h
    - others      -> from BCC SDK
- Test/...
```

#### Core/
---
```
- Inc/ 
    - main.h      -> self explanatory
    - ...
- Src/
    - main.c      -> where everything starts
    - ...
```
