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

# Questionable...
# set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ")
# set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-T${CHIP_PATH}/CompileDependencies/STM32G474RETx_FLASH.ld")

# what, does in fact not get the filename of somthing but rather the name of the project from the path
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

add_library(${PROJECT_NAME}_USER_CODE INTERFACE)
target_sources(${PROJECT_NAME}_USER_CODE INTERFACE
    Core/Src/main.c
    Core/Src/gpio.c
    Core/Src/adc.c
    Core/Src/crc.c
    Core/Src/fdcan.c
    Core/Src/spi.c
    Core/Src/stm32g4xx_it.c
    Core/Src/stm32g4xx_hal_msp.c
    Core/Src/system_stm32g4xx.c
    Core/Src/sysmem.c
    Core/Src/syscalls.c
)

target_link_libraries(${PROJECT_NAME}_USER_CODE INTERFACE

)

target_include_directories(${PROJECT_NAME}_USER_CODE INTERFACE
    Core/Inc
)
