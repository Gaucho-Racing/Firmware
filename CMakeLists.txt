cmake_minimum_required(VERSION 3.25)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT DEFINED CMAKE_BUILD_TYPE)
	message(
		FATAL_ERROR
		"This project requires configuration using a CMake preset. Please use 'cmake --preset <preset-name>' or select a preset in your IDE."
	)
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
include("${lib_path}/GlobalShare/common.cmake")
include("${lib_path}/Utils/CircularBuffer/circular-buffer-lib.cmake")
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

add_gr_project(STM32G474xE BLINKY G4HELLO)

add_gr_project(STM32G474xE BLINKY G4BLINKY)
add_gr_project(STM32U5A9xJ BLINKY U5BLINKY)
add_gr_project(STM32L476xG BLINKY L4BLINKY)

add_gr_project(STM32G474xE ECU)

add_gr_project(STM32G474xE Onboarding W3_G4SPI_Receive)
add_gr_project(STM32G474xE Onboarding W3_G4SPI_Transmit)
