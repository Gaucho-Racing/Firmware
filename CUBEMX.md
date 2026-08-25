> [!IMPORTANT]
> Note that using a peripheral abstraction (eg our version of CAN) means it will no longer be compatible with CubeMX. As a result please **disable** the peripheral within CubeMX and let the peripheral abstraction handle it.

# STM32CubeMX

This was last verified on version `6.18.0`

STM32CubeMX2 is a different product that coexists with STM32CubeMX and (as of now) cannot be used.

## Instructions

The process to add a new MCU to the monorepo is complex. Please see `CMAKE.md` or ask the current Firmware Manager for assistance and they can help set it up for you.

### Modification

1. Open the appropriate `.ioc` file, corresponding to the project you are interested in, in CubeMX
2. Make changes as relevant, being careful not to violate the [Configuration](#configuration) steps
3. Generate code and then make sure to add/remove from your project `CMakeLists.txt` as relevant

### Creation

0. Ensure the MCU you are working on has been added to the monorepo, see the list in [`Lib/Platform`](Lib/Platform).
1. Open and setup a new project through CubeMX, follow the [Configuration](#configuration) steps
2. Generate code and verify that it lives in the correct area, like the other projects
3. Copy [`ProjectTemplate/CMakeLists.txt](ProjectTemplate/CMakeLists.txt) into your folder, edit as instructed by the comments
   - Note that folders/files which are git ignored can be safely deleted or ignored.
4. Add a new entry to the root [`CMakeLists.txt`](CMakeLists.txt) containing the appropriate `add_gr_project` call following [`CMAKE.md`](CMAKE.md)
5. Add the appropriate entries to [`launch.json`](.vscode/launch.json) and [`tasks.json`](.vscode/tasks.json) following [`VSCODE.md`](VSCODE.md)

## Configuration

> [!IMPORTANT]
> Please ensure you follow these *exactly*, or it will not be guaranteed to work correctly.

### Project Manager

| Field | Value |
| :-- | :-- |
| Project Name | (Shortname, ie CCU, ECU) |
| Project Location | (Root of this Git repository) |
| Application Structure | Advanced |
| Do not generate the main() | FALSE / unchecked |
| Toolchain Location | (Root of this Git repository)/(Shortname from above) |
| Toolchain / IDE | `EWARM` |

### Code Generator

| Field | Value |
| :-- | :-- |
| STM32Cube MCU packages and embedded software packs | Add necessary library files as reference in the toolchain project configuration file |
| Generate peripheral initialization as a pair of '.c/.h' files per peripheral | TRUE / checked |
| Backup previously generated files when regenerating | FALSE / unchecked |
| Keep User Code when regenerating | TRUE / checked |
| Delete previously generated files when not regenerating | TRUE / checked |

## Website

See [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) for more from ST.

To install please visit the ["Get Software"](https://www.st.com/en/development-tools/stm32cubemx.html#section-get-software-table) tab.

## Purpose

CubeMX provides a simple way to change clocks, setup peripheral initializations, and much more (including calculating power consumption!).
