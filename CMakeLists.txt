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

# Vendor
include("${lib_path}/Vendor/CMSIS_5/cmsis_5.cmake")
include("${lib_path}/Vendor/STM32_HAL_LL/cube.cmake")
add_subdirectory("${lib_path}/Vendor/BCC")
include("${lib_path}/Vendor/MLX90640/combined.cmake")

# Platforms
include("${platform_path}/STM32G474xE/chip.cmake")
include("${platform_path}/STM32G431x8/chip.cmake")
include("${platform_path}/STM32U5A9xJ/chip.cmake")
include("${platform_path}/STM32L476xG/chip.cmake")
include("${platform_path}/STM32H523xE/chip.cmake")

# Utils
include("${lib_path}/GlobalShare/common.cmake")
include("${lib_path}/Utils/Logomatic/logomatic.cmake")
include("${lib_path}/Utils/VirtualComPort/vcp.cmake")
include("${lib_path}/Utils/CircularBuffer/circular-buffer-lib.cmake")
include("${lib_path}/Utils/MsgBuffer/msg-buffer-lib.cmake")
include("${lib_path}/cmake/gr-lib.cmake")
include("${lib_path}/Utils/BitManipulations/bit-utils.cmake")

# Autogen
include("Autogen/CAN/CANfigurator.cmake")

# Peripherals
include("${lib_path}/Peripherals/USART/common.cmake")
include("${lib_path}/Peripherals/CAN/common.cmake")

include("${lib_path}/FancyLayers-RENAME/GRCAN/grcan_fancylayer.cmake")
include("${lib_path}/FancyLayers-RENAME/ADC/adc.cmake")
include("${lib_path}/FancyLayers-RENAME/SPI/spi.cmake")
include("${lib_path}/FancyLayers-RENAME/NeoPixel/neopixel.cmake")

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
add_gr_project(STM32G474xE DashPanel)
add_gr_project(STM32G474xE AnalogCalibration)
add_gr_project(STM32G474xE CANine)
add_gr_project(STM32G431x8 TireTemp)

# Development
add_gr_project(STM32G474xE G4PERTESTING)
add_gr_project(STM32G474xE G4SPITESTING)
add_gr_project(STM32G474xE G4ADCTESTING)
add_gr_project(STM32G474xE G4NEOTESTING)

# CAN Peripheral Testing
add_gr_project(STM32G474xE G4CANTESTING CAN_EXTERNAL_TEST)
add_gr_project(STM32G474xE G4CANTESTING CAN_INTERNAL_TEST)
add_gr_project(STM32G474xE G4CANTESTING CAN_RELEASE_TEST)
add_gr_project(STM32G474xE G4CANTESTING CAN_STRESS_TEST)
add_gr_project(STM32G474xE G4CANTESTING CAN_LOST_RX_TEST)
add_gr_project(STM32G474xE G4CANTESTING CAN_FILTER_TEST)
add_gr_project(STM32G474xE G4CANTESTING PROFILER_TEST)

add_gr_project(STM32G474xE G4FANCYCANTESTING CAN_LOOPBACK_TEST)

# BLINKY Demos
add_gr_project(STM32G474xE BLINKY G4BLINKY)
add_gr_project(STM32U5A9xJ BLINKY U5BLINKY)
add_gr_project(STM32L476xG BLINKY L4BLINKY)

add_gr_project(STM32L476xG BLINKY L4HELLO)
add_gr_project(STM32G474xE BLINKY G4HELLO)
add_gr_project(STM32G474xE BLINKY G4ECHO)

# Onboarding Workshops
add_gr_project(STM32G474xE Onboarding W3_G4SPI_Receive)
add_gr_project(STM32G474xE Onboarding W3_G4SPI_Transmit)
