cmake_minimum_required(VERSION 3.25)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Firmware)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(${CMAKE_PROJECT_NAME})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
message("Build type: " ${CMAKE_BUILD_TYPE})

set(platform_path "Lib/Platform")
# include the STMG474RE cmake
if (CMAKE_BUILD_TYPE STREQUAL "Test")

message("using test build type, not going to do anything")

else()

message("not using test")

include(${platform_path}/STM32G474/stm32g474.cmake)

add_executable_stmg474re(BLINKY)

add_subdirectory(MVP)

add_library(plzwork INTERFACE)

target_link_libraries(plzwork INTERFACE
  STM32G474_LIB
  MVP
)

target_link_libraries(BLINKY
  plzwork
)



endif()

# target_link_library(BLINKY BLINK_lib)

# add_executable_stm32g474re(ACU)
# target_sources(ACU
#   ACU/Application/Src/acu.c
#   ACU/Application/Src/battery.c
#   ACU/Application/Src/debug.c
#   ACU/Application/Src/mcu.c
#   ACU/Application/Src/state.c
#   ACU/Core/Src/adc.c
#   ACU/Core/Src/dma.c
#   ACU/Core/Src/fdcan.c
#   ACU/Core/Src/gpio.c
#   ACU/Core/Src/main.c
#   ACU/Core/Src/spi.c
#   ACU/Core/Src/stm32g4xx_hal_msp.c
#   ACU/Core/Src/stm32g4xx_it.c
#   ACU/Core/Src/syscalls.c
#   ACU/Core/Src/sysmem.c
#   ACU/Core/Src/system_stm32g4xx.c
#   ACU/Core/Src/tim.c
#   ACU/Core/Src/usart.c
#)
# target_include_directory(ACU
#   ACU/Application/Inc
#   ACU/Core/Inc
# )
# target_link_library(ACU ...)
