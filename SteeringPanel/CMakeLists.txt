cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-variable")

add_compile_definitions(LOGOMATIC_ENABLED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -u _printf_float")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME SteeringPanel)

# Set MCU-specific configuration
include("../lib/cmake/stm32u5-config.cmake")
set(LINKER_SCRIPT "STM32U599xx_FLASH.ld")

# Include shared toolchain file
include("../lib/cmake/gcc-arm-none-eabi.cmake")

# Include shared STM32CubeMX functions
include("../lib/stm32cubemx.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(LV_CONF_BUILD_DISABLE_DEMOS 1)
set(LV_CONF_BUILD_DISABLE_EXAMPLES 1)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})


# Add STM32CubeMX generated sources
add_subdirectory(../lib/LVGL/lvgl ${CMAKE_BINARY_DIR}/lvgl)

# Create STM32U5 CubeMX target using shared function
create_stm32u5_cubemx_target(stm32cubemx)

# Add board-specific sources that differ from the common STM32U5 set
add_board_specific_sources(stm32cubemx
    # Add any SteeringPanel-specific STM32CubeMX sources here
    # Currently using default STM32U5 sources
)

# The ordering is bad sorry
target_include_directories(lvgl PUBLIC
    # Use shared CMSIS from lib directory
    ../lib/CMSIS/Device/ST/STM32U5xx/Include
    ../lib/CMSIS/Include
    ../lib/LVGL
    ../lib/FreeRTOS/Source/include
    ../lib/FreeRTOS/CMSIS_RTOS2/Include
    Core/Inc
    # Use shared HAL driver from lib directory
    ../lib/STM32U5xx_HAL_Driver/Inc
    ../lib/STM32U5xx_HAL_Driver/Inc/Legacy
    ../lib/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/
    ../lib/FreeRTOS/Source/CMSIS_RTOS_V2/
    Application/Inc/
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/lvgl_port_display.c
    Core/Src/mx25lm51245g.c
    Application/Src/CANdler.c
    Application/Src/utils.c
    Application/Src/steering.c
    Application/Src/gui.c
    Application/Src/encoders.c

    Application/Src/Fonts/lv_font_gr_bold_16.c
    Application/Src/Fonts/lv_font_gr_bold_24.c
    Application/Src/Fonts/lv_font_gr_bold_32.c
    Application/Src/Fonts/lv_font_gr_bold_40.c
    Application/Src/Fonts/lv_font_gr_regular_16.c
    Application/Src/Fonts/lv_font_gr_regular_24.c
    Application/Src/Fonts/lv_font_gr_regular_32.c
    Application/Src/Fonts/lv_font_gr_regular_40.c
    Application/Src/Fonts/lv_font_gr_light_16.c
    Application/Src/Fonts/lv_font_gr_light_24.c
    Application/Src/Fonts/lv_font_gr_light_32.c
    Application/Src/Fonts/lv_font_gr_light_40.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ../lib/LVGL
    ../lib/include
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    lvgl
)

# Enable Link Time Optimization for smaller binaries and better performance
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG TRUE)

# Note: CSV header generation is now handled by the root CMakeLists.txt
# Headers are generated once for all boards before building
