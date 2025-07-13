cmake_minimum_required(VERSION 3.25)

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
set(CMAKE_PROJECT_NAME ECU)

# Set MCU-specific configuration
include("../lib/cmake/stm32g4-config.cmake")
set(LINKER_SCRIPT "stm32g474retx_flash.ld")

# Include shared toolchain file
include("../lib/cmake/gcc-arm-none-eabi.cmake")

# Include shared STM32CubeMX functions
include("../lib/stm32cubemx.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
# Create STM32G4 CubeMX target using shared function
create_stm32g4_cubemx_target(stm32cubemx)

# Add board-specific sources that differ from the common STM32G4 set
add_board_specific_sources(stm32cubemx
    # Add any ECU-specific STM32CubeMX sources here
    # Currently using default STM32G4 sources
)

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
    ../lib/CAN
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

# Enable Link Time Optimization for smaller binaries and better performance
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG TRUE)

# Note: CSV header generation is now handled by the root CMakeLists.txt
# Headers are generated once for all boards before building
