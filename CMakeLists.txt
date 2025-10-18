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

set(lib_path "Lib")
set(platform_path "${lib_path}/Platform")

include("${platform_path}/STM32G474xE/chip.cmake")
include("${platform_path}/STM32U5A9xJ/chip.cmake")
include("${platform_path}/STM32L476xG/chip.cmake")
include("${lib_path}/cmake/gr-lib.cmake")

project(${CMAKE_PROJECT_NAME})

# Enable CMake support for ASM and C languages
enable_language(
	C
	ASM
)

# Core project settings
message(
	"Build type: "
	${CMAKE_BUILD_TYPE}
)

add_gr_project(STM32G474xE G4BLINKY)
add_gr_project(STM32U5A9xJ U5BLINKY)
add_gr_project(STM32L476xG L4BLINKY)

add_gr_project(STM32G474xE ECU)

add_gr_project(STM32G474xE G4PERTESTING)

add_gr_project(STM32G474xE W3_Workshop G4SPI_Receive)
add_gr_project(STM32G474xE W3_Workshop G4SPI_Transmit)

add_subdirectory(Lib/Utils/CircularBuffer)
