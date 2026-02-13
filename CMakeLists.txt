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

if(NOT DEFINED CMAKE_PRESET_NAME)
	message(
		FATAL_ERROR
		"This project requires configuration using a CMake preset. Please use 'cmake --preset <preset-name>' or select a preset in your IDE."
	)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(lib_path "Lib")
set(platform_path "${lib_path}/Platform")

# FIXME Temporary holdover for CAN messages until URCA is ready for merge
include("${lib_path}/FancyLayers-RENAME/GRCAN/TemporaryHoldover/temp.cmake")

# CMSIS and HAL/LL
include("${lib_path}/Vendor/CMSIS_5/cmsis_5.cmake")
include("${lib_path}/Vendor/STM32_HAL_LL/cube.cmake")

# Platforms
include("${platform_path}/STM32G474xE/chip.cmake")
include("${platform_path}/STM32U5A9xJ/chip.cmake")
include("${platform_path}/STM32L476xG/chip.cmake")

# Utils
include("${lib_path}/GlobalShare/common.cmake")
include("${lib_path}/Utils/Logomatic/logomatic.cmake")
include("${lib_path}/Utils/ConvenienceMacros/macros.cmake")
include("${lib_path}/Utils/CircularBuffer/circular-buffer-lib.cmake")
include("${lib_path}/cmake/gr-lib.cmake")
include("${lib_path}/Utils/BitManipulations/bit-utils.cmake")

# Autogen
include("Autogen/CAN/CANfigurator.cmake")

# Peripherals
include("${lib_path}/Peripherals/USART/common.cmake")
include("${lib_path}/Peripherals/CAN/common.cmake")
include("${lib_path}/FancyLayers-RENAME/ADC/adc.cmake")

message(
	STATUS
	"Build type: "
	${CMAKE_BUILD_TYPE}
)

message(STATUS "Target type: ${TARGET_TYPE}")

message(
	STATUS
	"Active CMake Preset: "
	${CMAKE_PRESET_NAME}
)

# Projects
add_gr_project(STM32G474xE ECU)
add_gr_project(STM32G474xE CCU)
add_gr_project(STM32G474xE AnalogCalibration)

# Development
add_gr_project(STM32G474xE G4ADCTESTING)
add_gr_project(STM32G474xE G4PERTESTING)
add_gr_project(STM32G474xE G4CANTESTING)

# BLINKY Demos
add_gr_project(STM32G474xE BLINKY G4HELLO)
add_gr_project(STM32G474xE BLINKY G4BLINKY)
add_gr_project(STM32U5A9xJ BLINKY U5BLINKY)
add_gr_project(STM32L476xG BLINKY L4BLINKY)

# Onboarding Workshops
add_gr_project(STM32G474xE Onboarding W3_G4SPI_Receive)
add_gr_project(STM32G474xE Onboarding W3_G4SPI_Transmit)
