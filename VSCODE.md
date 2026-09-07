# Visual Studio Code

## Quick Start

1. Download VS Code from [`code.visualstudio.com`](https://code.visualstudio.com/download)
2. Install the extensions (or a subset of the extensions if you know what you are doing) listed in [`extensions.json`](.vscode/extensions.json)
3. Navigate to the `CMake` tab
    - Select the `Debug` preset under `Configure` (change later as relevant)
    - Select `all (META)` under `Build` (change later as relevant)
4. Navigate to the `Run and Debug` tab (Ctrl+Shift+D)
    - Select the project you wish to run

- Ensure the board has been connected for a few seconds already
- Press the green arrow to begin running
  - See [Debug C++ in Visual Studio Code](https://code.visualstudio.com/docs/cpp/cpp-debug) and [Debug code with Visual Studio Code](https://code.visualstudio.com/docs/debugtest/debugging) for how to do more debugging

## Overall Summary

[`launch.json`](.vscode/launch.json) provides all of the launch configurations for the `Run and Debug` tab

[`tasks.json`](.vscode/tasks.json) provides all of the compilation tasks for the `preLaunchTask` in the launch configurations

### Launch Configurations

If you are not using logomatic then the entry is fairly simple, see the below example from the Nucleo-G474 (STM32G474RET6) BLINKY testbench project

```json
{
 "cwd": "${workspaceFolder}",
 "executable": "${command:cmake.buildDirectory}/NucleoG4-Blinky.elf",
 "name": "NucleoG4-Blinky",
 "request": "launch",
 "type": "cortex-debug",
 "servertype": "openocd",
 "configFiles": [
  "interface/stlink.cfg",
  "target/stm32g4x.cfg"
 ],
 "searchDir": [],
 "preLaunchTask": "CMake: configure and build NucleoG4-Blinky",
 "showDevDebugOutput": "raw",
 "svdPath": "${workspaceFolder}/Lib/Vendor/SVD/STM32G474.svd",
 "openOCDLaunchCommands": [
  "reset_config srst_only separate connect_assert_srst",
 ],
 "showDevDebugTimestamps": true
}
```

If you are using logomatic then see [Logomatic's `README.md`](Lib/Utils/Logomatic/README.md) or the below example. Note that the CPU frequency must be exact, while the SWO frequency is determined at runtime and can be changed.

```json
{
 "cwd": "${workspaceFolder}",
 "executable": "${command:cmake.buildDirectory}/NucleoG4-Hello.elf",
 "name": "NucleoG4-Hello",
 "request": "launch",
 "type": "cortex-debug",
 "servertype": "openocd",
 "configFiles": [
  "interface/stlink.cfg",
  "target/stm32g4x.cfg"
 ],
 "searchDir": [],
 "preLaunchTask": "CMake: configure and build NucleoG4-Hello",
 "showDevDebugOutput": "raw",
 "svdPath": "${workspaceFolder}/Lib/Vendor/SVD/STM32G474.svd",
 "swoConfig": {
  "enabled": true,
  "cpuFrequency": 160000000,
  "swoFrequency": 2000000,
  "source": "probe",
  "decoders": [
   {
    "type": "console",
    "label": "ITM",
    "showOnStartup": true,
    "port": 0,
    "encoding": "ascii"
   }
  ]
 }
}
```

### Task Configuration

Compilation consists of two steps

1. Configuration
    - `CMake: configure`
    - `CMake: configure NODE=FL`
    - `CMake: configure NODE=FR`
    - `CMake: configure NODE=RL`
    - `CMake: configure NODE=RR`
2. Compilation
    - One task per build

Compilation tasks are near identical, simply matching the label and command

```json
{
 "label": "CMake: configure and build NucleoG4-Blinky",
 "type": "shell",
 "dependsOrder": "sequence",
 "dependsOn": [
  "CMake: configure"
 ],
 "command": "cmake --build --preset ${command:cmake.activeBuildPresetName} --target NucleoG4-Blinky"
},
```

They can be more complicated and specify the `NODE` as well, you would set up a seperate launch profile for each valid `NODE` variation of the given task

```json
{
 "label": "CMake: configure and build TireTemp FL",
 "type": "shell",
 "dependsOrder": "sequence",
 "dependsOn": [
  "CMake: configure NODE=FL"
 ],
 "command": "cmake --build --preset ${command:cmake.activeBuildPresetName} --target TireTemp"
},
```

## Advanced

### C/C++ Properties

Configures VS Code IntelliSense for GCC11 using CMake, it also specifies the `compile_commands.json` files

### Settings

Sets the default VS Code IntelliSense mode for the different projects, provides default file associations, and adds some configuration/formatting options

### Extensions

A literal list of extensions which our workspace recommends you install, the first few are required but others are technically optional. Note specifically `ms-vscode.cpptools-extension-pack`, `ms-vscode.cmake-tools`, and `marus25.cortex-debug` are critical to success

See the root [README.md](README.md) for more on initial setup
