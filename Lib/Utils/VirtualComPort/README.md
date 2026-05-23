# Virtual COM Port
> [!WARNING]
> Not all ST-Link devices are physically wired to support VCP!
>
> This will only work if you plug it in and already see an available serial port!

A wrapper around USART to provide easier control of the VCP.

Does not conflict with Logomatic using SWO.

## Quick Setup

0. Link agains the `VCP_LIB` target library
1. Copy the example header into your project and edit accordingly
2. Run `VCP_Setup` on your `VCP_Config` struct (do not leave the Rx callback uninitialized)
3. Use `VCP_Send` and your Rx callback to send and receive messages

## What it does
- Initializes USART for simple serial transmission
- Provides a callback for receiving characters
- Allows sending strings over VCP using interrupts

## Setup

0. Link against the interface library `VCP_LIB`
1. Copy `vcp_config_example.h` into your `Application/Inc`, rename the file to `vcp_config.h` and edit as instructed
2. Include `vcp.h` into your project files that you wish to interact with
3. Fill a `VCP_Config` with your USART configuration: GPIO port/pins, baud, and framing settings. Optionally define your callback or use `NULL` (do not leave it uninitialized)
4. Call `VCP_Setup(&config)` exactly once during startup
5. Use `VCP_Send` to send data, to receive use the callback you previously defined

## HOOTL Test

Simply add the `VCP_LIB` linked library for any executable test you would like to use, no other changes are necesary.

If you run into warnings while printing 32-bit integers using the `l` specifier in `snprintf` or family simply use `inttypes.h`
