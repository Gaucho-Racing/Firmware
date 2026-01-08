cmake_minimum_required(VERSION 3.25)

project(Firmware)

enable_language(
	C
	ASM
)

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

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(lib_path "Lib")
set(platform_path "${lib_path}/Platform")

include("${platform_path}/STM32G474xE/chip.cmake")
include("${platform_path}/STM32U5A9xJ/chip.cmake")
include("${platform_path}/STM32L476xG/chip.cmake")
include("${lib_path}/GlobalShare/common.cmake")
include("${lib_path}/Utils/CircularBuffer/circular-buffer-lib.cmake")
include("${lib_path}/cmake/gr-lib.cmake")

# Peripherals
include("${lib_path}/Peripherals/USART/common.cmake")
include("${lib_path}/Peripherals/CAN/common.cmake")

message(
	STATUS
	"Build type: "
	${CMAKE_BUILD_TYPE}
)

# Projects
add_gr_project(STM32G474xE ECU)

# Development
add_gr_project(STM32G474xE G4PERTESTING)

# BLINKY Demos
add_gr_project(STM32G474xE BLINKY G4HELLO)
add_gr_project(STM32G474xE BLINKY G4BLINKY)
add_gr_project(STM32U5A9xJ BLINKY U5BLINKY)
add_gr_project(STM32L476xG BLINKY L4BLINKY)

# Onboarding Workshops
add_gr_project(STM32G474xE Onboarding W3_G4SPI_Receive)
add_gr_project(STM32G474xE Onboarding W3_G4SPI_Transmit)


# Onboarding Projects
# TODO Add onboarding projects here as needed
