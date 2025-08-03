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

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message(CMAKE_ASM_COMPILER=${CMAKE_ASM_COMPILER})

set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -T 'C:/Users/Will/Documents/School/FSAE/Firmware/Lib/Platform/STM32G474/CompileDependencies/STM32G474RETx_FLASH.ld'")

add_library(MVP INTERFACE)
target_sources(MVP INTERFACE
    Core/Src/gpio.c
    Core/Src/main.c
    Core/Src/syscalls.c
    Core/Src/system_stm32g4xx.c
)

target_link_libraries(MVP INTERFACE

)

target_include_directories(MVP INTERFACE
    Core/Inc
)
