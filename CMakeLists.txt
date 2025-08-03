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

set(platform_path "Lib/Platform")

include(${platform_path}/STM32G474xE/chip.cmake)

project(${CMAKE_PROJECT_NAME})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
message("Build type: " ${CMAKE_BUILD_TYPE})

# include the STMG474RE cmake
if (CMAKE_BUILD_TYPE STREQUAL "Test")

message("using test build type, not going to do anything")

else()

add_executable_STM32G474xE(BLINKY)

add_subdirectory(G4MVP)

add_library(BlinkyMvpCombinator INTERFACE)

target_link_libraries(BlinkyMvpCombinator INTERFACE
  STM32G474xE_LIB
  G4MVP
)

target_link_libraries(BLINKY
  BlinkyMvpCombinator
)

endif()
