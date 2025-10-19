# BLINKY

This folder contains small "blinky" example projects used to validate toolchains, linker scripts, and basic board bring-up across supported STM32 platforms. Each supported chip family in `Lib/Platform/` should have a corresponding simple blinky application under this directory.

## Goals

- Provide a minimal, working example for each supported platform.
- Ensure every platform can produce correct code
- Provide a canonical pattern for new projects to follow.

## Running

1. Select the `Debug` preset inside of the CMake Extension in VS Code
2. Select the appropriate `<CHIP>BLINKY` inside of the "Run and Debug" page in VS Code
3. Click the Start Debugging icon and observe the board blinking

## Troubleshooting

### LibUSB Errors

Commonly found on MacOS, this appears to be a hardware problem please use a dongle and try again.

### CMake Errors

Make sure to select the `Debug` preset, also confirm that the following programs are found on your system:

```sh
cmake --version
```

```sh
ninja --version
```

```sh
arm-none-eabi-gcc --version
```

```sh
openocd --version
```

### Linker Errors

Sometimes a bug pops up if using unicode or spaces inside of paths, please ensure the entire path to this cloned repository does not contain any spaces (eg `/foo/bar/Firmware (1)` or `C:\foo\bar\Firmware (1)`) or similarly difficult to parse characters.

### Header File Missing

Make sure to have the CMake preset set to `Debug`, `Release`, `RelWithDebInfo`, or `MinSizeRel`.  Not to `HOOTLTest`.

### Missing Extensions

Ensure extensions specified in [`extensions.json`](/.vscode/extensions.json) are installed inside of VS Code

### Unable to Access Hardware

Verify that the repository is cloned onto your local machine and not onto the ECI or CSIL machines.

### VS Code Command Does Not Exist CMake Build

Sometimes, CMake will attempt to use the language of your operating system, but the language we use to configure CMake is English. Such commands like `CMake: build` inside of a prelaunch task will fail as CMake will provide the word "build" in your native language instead of the specified English "build".

To fix, set your VS Code "Display Language" to use English via the `Configure Display Language` command. It would be nice to support more languages and if you find a work around please submit a PR.
