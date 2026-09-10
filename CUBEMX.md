# STM32CubeMX

> [!IMPORTANT]
> Note that using a peripheral abstraction (eg our old version of CAN) means it will no longer be compatible with CubeMX. As a result please **disable** the peripheral within CubeMX and let the peripheral abstraction handle it.

STM32CubeMX2 is a different product that coexists with STM32CubeMX and (as of now) cannot be used.

## Instructions

The process to add a new MCU to the monorepo is complex. Please see `CMAKE.md` or ask the current Firmware Manager for assistance and they can help set it up for you.

### Modification

1. Open the appropriate `.ioc` file, corresponding to the project you are interested in, in CubeMX
2. Make changes as relevant, being careful not to violate the [Configuration](#configuration) steps
3. Generate code and then make sure to add/remove from your project `CMakeLists.txt` as relevant

### Creation

## Configuration

> [!IMPORTANT]
> Please ensure you follow these *exactly*, or it will not be guaranteed to work correctly.

### Project Manager

| Field | Value |
| :-- | :-- |
| Project Name | (Shortname, ie CCU, ECU) |
| Project Location | (Root of this Git repository or a subfolder, eg [`Testbench`](/Testbench)) |
| Application Structure | Advanced |
| Do not generate the main() | FALSE / unchecked |
| Toolchain Location | (Root of this Git repository)/(Shortname from above) |
| Toolchain / IDE | CMake |
| Default Compiler/Linker | GCC |

### Code Generator

| Field | Value |
| :-- | :-- |
| STM32Cube MCU packages and embedded software packs | Add necessary library files as reference in the toolchain project configuration file |
| Generate peripheral initialization as a pair of '.c/.h' files per peripheral | TRUE / checked |
| Backup previously generated files when regenerating | FALSE / unchecked |
| Keep User Code when regenerating | TRUE / checked |
| Delete previously generated files when not regenerating | TRUE / checked |
| Set all free pins as analog (to optimize power consumption) | TRUE / checked |

## Website

See [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) for more from ST.

To install please visit the ["Get Software"](https://www.st.com/en/development-tools/stm32cubemx.html#section-get-software-table) tab.

## Purpose

CubeMX provides a simple way to change clocks, setup peripheral initializations, and much more (including calculating power consumption!).
