# Logomatic

## Quick Start

In STM32CubeMX configure `SYS` Debug to be "Trace Asynchronous Sw". Other settings will require you to set `TPI` settings as needed.

By default logomatic will use ITM with your clock-configured baud rate on port 0. You can configure this trivially if you want.

1. Link against `LOGOMATIC_LIB` as another library in your CMake project
2. Add `#include "Logomatic.h"` to your files and add calls to `LOGOMATIC_<LEVEL>()` as appopriate
3. Call `Logomatic_SetLogLevel()` to configure your global verbosity logging level
4. Add the `swoConfig` key to your  [`launch.json](/.vscode/launch.json) entry for your project, see the default below

> ```json
> "swoConfig": {
>     "enabled": true,
>     "cpuFrequency": 160000000,
>     "swoFrequency": 115,
>     "source": "probe",
>     "decoders": [
>         {
>             "type": "console",
>             "label": "ITM",
>             "showOnStartup": true,
>             "port": 0,
>             "encoding": "ascii"
>         }
>     ]
> }
> ```

## Drivers

You may configure any LPUART, UART, or USART to serve as your dedicated channel as well (almost) out of the box.

To do that, you must define a strong symbol for `global_logomatic_driver` setting it to your own function, defaults are provided in the form `Logomatic_Driver_<Peripheral>`.

You can setup your own as well. Any you setup should have the same function signature and should

- On success return the written character
- On failure return `EOF`
- Be very quick, as logomatic is often used inside ISRs for debugging

## Log Levels

Logs are defined in a X-Macro list `_LOGOMATIC_LOGLEVEL_X_LIST` where a higher number means more output

| Status | Level | Description |
| --- | --- | --- |
| :zipper_mouth_face: | Off | Nothing will be printed, less performant than using `CMAKE_LOGOMATIC_ENABLED` |
| :rotating_light: | Critical | An important operation is failing and the car will not behave correctly |
| :x: | Error | An operation is failing but the car as a whole may be able to recover on its own |
| :warning: | Warning | An operation is not proceeding as expected but may recover on its own |
| :bell: | Info | An operation updates the status of the car, but nothing bad has happened |
| :bug: | Debug | An operation would like to report diagnostics that may be helpful |
| :loudspeaker: | Verbose | An operation has generated information and later inspection of the logs could be helpful, spammy |

## Advanced

It is strongly suggested to use ITM and the default settings, it will compile out fully on release builds with no overhead.

### CMake

Logomatic is exposed to others as an interface library called `LOGOMATIC_LIB`. From here you can include `Logomatic.h` which will automagically try and find a working peripheral to attach to.

The CMake file for the library is [`log.cmake`](log.cmake) and uses the symbol `CMAKE_LOGOMATIC_ENABLED` to allow [`CMakePresets.json`](/CMakePresets.json) to enable or disable logging based on if the build is a debug or release build. If it is enabled then we add the compiler definition `LOGOMATIC_ENABLED`.

By default, we also enable printing floats which adds significant memory and CPU overhead. It is strongly recommend to never print raw floating point values.

If we are building for testing then [`CMakePresets.json`](/CMakePresets.json) ensures that logomatic is enabled and we attempt to signal the library to default to a verbose log level, see [log levels](#log-levels)

### Macros

#### Log Levels X-Macro

See [log levels](#log-levels) for more.

We define `_LOGOMATIC_LOGLEVEL_PRIVATE_LIST` which consists of [X-Macros](https://wikipedia.org/wiki/X_macro) for the enum name, value, and text label. It is recommended to understand the nuances of the X-Macro pattern before continuing reading.

We define `X` operations to translate that list into different forms, namely an enum, an array for value to text lookups, and type checking on the enum.

It is required that more verbose log levels have a higher value than less verbose log levels.

#### Logging Macros

We provide macros, manually made, for each log level to simplify calling. These are in the form `LOGOMATIC_<LEVEL>` and they take a single format string to print (exactly `printf`).

These macros are variants of `_LOGOMATIC_PRIVATE_LOG()` which does runtime checks of the configured global log level against the requested log level, only printing if it is equal or higher.
This function also prints the log level in front of them, verbose logs also get the file and line number it was invoked on  for easier debugging.

As we have warnings for double promotion, since STM32 chips support FPU-accelerated operations only for 32-bit floats, we also have to push and pop a diagnostic to ignore that warning for that specific line.

If logomatic is not enabled then each of the user-callable macros compiles to nothing, while not changing the used/unused status of the variables and not affecting the vardic variable.

### Color Codes

If you really want to, you can print colors using ANSI color codes. Be aware that this will tank performance with lots of unneeded data and is not supported.

We also ask that you do not use color codes because different setups on different computers may behave differently.

### HOOTL Testing

Calls to logomatic functions inside of HOOTLTest configurations will work as you expect, following the same logging level filter.

Note that by default the logging level for embedded is informational, but for HOOTL testing it defaults to verbose and calls to change the logging level are ignored by default.

## Troubleshooting

TODO

As issues are found we will extend this section. For now, please contact the Firmware Manager if you are unable to get this working. Be aware that specific hardware is required to use Logomatic
