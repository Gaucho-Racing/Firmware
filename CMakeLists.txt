cmake_minimum_required(VERSION 3.25)

# Enable CMake policies for Link Time Optimization
cmake_policy(SET CMP0069 NEW)  # Enable INTERPROCEDURAL_OPTIMIZATION property

# Set project name and description
project(Firmware-26
    VERSION 26.0.0
    DESCRIPTION "Firmware Suite GR26"
    LANGUAGES C ASM
)

# Global compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Enable compile commands for better IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Define build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "Building Gaucho Racing Firmware Suite")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Link Time Optimization (LTO): ENABLED for ARM cross-compilation")

# Create a shared utilities library from lib/ (for host system)
add_library(gaucho_utils STATIC
    lib/utils.c
)

target_include_directories(gaucho_utils PUBLIC
    lib
)

# Configure shared libraries
# Both CMSIS and HAL drivers are now shared across all boards in lib/
# - lib/CMSIS/ contains shared ARM Cortex-M and STM32 device drivers
# - lib/STM32G4xx_HAL_Driver/ contains shared STM32G4 HAL drivers for ECU
# - lib/STM32U5xx_HAL_Driver/ contains shared STM32U5 HAL drivers for DashPanel
# Each board's Core/ and Application/ directories contain board-specific code

# Add shared CSVGenerator header generation
# This runs once and generates headers for all boards in lib/CAN
add_custom_target(
    generateCSVHeaders
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/lib/CSVGenerator/executeCSV.sh
    DEPENDS ${CMAKE_SOURCE_DIR}/lib/CSVGenerator/grCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/lib/CSVGenerator/GR-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/lib/CSVGenerator/msgCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/lib/CSVGenerator/MSG-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/lib/CSVGenerator/customIdCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/lib/CSVGenerator/CUSTOM-ID.csv
    COMMENT "Generating CSV header files in lib/include"
    USES_TERMINAL
)

# Important note about CSV files
message(STATUS "")
message(STATUS "=== CSV Generator Important Notice ===")
message(STATUS "Due to security limitations you will need to confirm our local CSVs match the online version")
message(STATUS "at https://docs.google.com/spreadsheets/d/1XfJhhAQoDnuSuwluNitPsDWtuQu-bP-VbEGPmSo5ujA/edit?usp=sharing")
message(STATUS "If out of date, update the CSV in lib/CSVGenerator/ and rebuild to update the header files")
message(STATUS "=======================================")

# Use ExternalProject for proper CMake integration and incremental builds
include(ExternalProject)

# Dashboard Panel firmware (STM32U5)
ExternalProject_Add(DashPanel
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/DashPanel
    BINARY_DIR ${CMAKE_SOURCE_DIR}/DashPanel/build
    CMAKE_ARGS 
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        -G Ninja
    BUILD_ALWAYS FALSE  # Enable incremental builds
    INSTALL_COMMAND ""  # No install step needed
    TEST_COMMAND ""     # No test step needed
    USES_TERMINAL_BUILD TRUE
    COMMENT "Building Dashboard Panel firmware (STM32U5)"
    DEPENDS generateCSVHeaders
)

# ECU firmware (STM32G4)
ExternalProject_Add(ECU
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/ECU
    BINARY_DIR ${CMAKE_SOURCE_DIR}/ECU/build
    CMAKE_ARGS 
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        -G Ninja
    BUILD_ALWAYS FALSE  # Enable incremental builds
    INSTALL_COMMAND ""  # No install step needed
    TEST_COMMAND ""     # No test step needed
    USES_TERMINAL_BUILD TRUE
    COMMENT "Building ECU firmware (STM32G4)"
    DEPENDS generateCSVHeaders
)

# CCU firmware (STM32G4)
ExternalProject_Add(CCU
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/CCU
    BINARY_DIR ${CMAKE_SOURCE_DIR}/CCU/build
    CMAKE_ARGS 
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        -G Ninja
    BUILD_ALWAYS FALSE  # Enable incremental builds
    INSTALL_COMMAND ""  # No install step needed
    TEST_COMMAND ""     # No test step needed
    USES_TERMINAL_BUILD TRUE
    COMMENT "Building CCU firmware (STM32G4)"
    DEPENDS generateCSVHeaders
)

# Convenience target to build all firmware boards
add_custom_target(all-boards
    DEPENDS DashPanel ECU CCU
    COMMENT "Building all firmware boards"
)
