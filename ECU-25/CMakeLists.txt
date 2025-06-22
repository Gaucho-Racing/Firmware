cmake_minimum_required(VERSION 3.25)

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

# Error highlighting
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -fdiagnostics-color")

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Enable logging
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(LOGOMATIC_ENABLED)
#   add_compile_definitions(ADVANCED_LOGGING)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -u _printf_float")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME ECU-25)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
#project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
#enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Application/Src/stateMachine.c
    Application/Src/pinging.c
    Application/Src/CANdler.c
    Application/Src/driving.c
    Application/Src/utils.c
    Application/Src/inverter.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Application/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    # Add user defined libraries
)

# Add CSV-25 integration
add_dependencies(${CMAKE_PROJECT_NAME} executeCSV25)
add_custom_target(
    executeCSV25
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/CSV-25/executeCSV.sh
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/grCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/GR-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/msgCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/MSG-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/customIdCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/CUSTOM-ID.csv
    COMMENT "Updating GR ID and MSG ID header files from CSVs"
    USES_TERMINAL
)
