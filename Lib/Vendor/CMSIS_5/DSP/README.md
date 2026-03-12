# DSP

Copied directly from [Sensor-Nodes](https://github.com/Gaucho-Racing/Sensor-Nodes/tree/main/Software/STM32G4/G431-MLX90640/Drivers/CMSIS/DSP)

Note that this is a temporary solution, if others need DSP we should investigate a full migration

Also note that the `CMSIS_5_DSP` interface library only packages the header files, you must in your project specifically link against the C files that you want using the `CMSIS_5_DSP_SOURCE_ROOT` CMake variable.

This allows linking against things like `${CMSIS_5_DSP_SOURCE_ROOT}/FastMathFunctions/arm_sqrt_q31.c` much more selectively.
