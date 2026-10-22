# Vendor Target Flags

Each file should be named `stm32**xx.cmake` where `**` is the lowercase series, eg `g4`.

## Contents

Each file should have a `target_compile_options` and `target_link_options` targetting `"${TARGET_NAME}"`

Ensure that each defines at least the following machine dependent options (`-m` flags)

| Machine Option | Type | Meaning | Preference |
| --- | --- | --- | --- |
| `cpu` | Hardware identifier | Defines the specific CPU and sets `arch` and `tune` automatically | Varies |
| `fpu` | Hardware identifier | Defines the exact FPU, vector extension, percision, register count | Varies |
| `float-abi` | `soft` / `softfp` / `hard` | Floating-point calling convention and ops on hardware or software | `hard` |

## Advanced

Set the CPU and FPU flags to match your hardware, and if you have a FPU make sure you use it by specifiying your floating-point ABI to use hardware, and if you are compiling for a Cortex-M microcontroller then you must use the `T32` instruction set

### CPU and FPU

CPU and FPU should always be exactly what the chip actually has, you can grab this trivially from the datasheet or look in the git-ignored `cmake` file after CubeMX generates its own (note that we do not use that `cmake` as it is not setup for the monorepo)

Note that almost all STM32 chips FPUs only support 32-bit floats, this is why we have compiler warnings for double promotion since `double` must use software while `float` can use dedicated hardware.

### Float ABI

Determines the application binary interface (ABI) to generate floating point instructions in

> | ABI Option | Description |
> | --- | --- |
> | `soft` | Soft ABI without FPU hardare: All floating-point operations are handled by the runtime library functions. Values are passed through integer register bank |
> | `softfp` | Soft ABI with FPU hardware: This allows the compiled code to generate codes that directly access the FPU. But, if a calculation needs to use a runtime library function, a soft-float calling convention is used. Values are passed through integer register bank |
> | `hard` | Hard ABI: This allows the compiled code to generate codes that directly accesss the FPU and use FPU-specific calling conventions when calling runtime library functions |

See [ABI Options](https://support.arm.com/documentation/107656/0101/Registers/Floating-point-registers/Using-Floating-point-extension/ABI-options) from which the above table was approximately copied

When possible, we always use `hard` because it uses the included FPU which means floating point operations are signficiantly faster.
