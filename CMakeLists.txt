cmake_minimum_required(VERSION 3.25)
project(Firmware)
enable_language(
	C
	ASM
)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Messages
if(NOT DEFINED CMAKE_PRESET_NAME)
	message(
		FATAL_ERROR
		"This project requires configuration using a CMake preset. Please use 'cmake --preset <preset-name>' or select a preset in your IDE."
	)
endif()
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

# Shared Compiler Cache Configuration
find_program(SCCACHE_PROGRAM sccache)
if(
	SCCACHE_PROGRAM
	AND
		NOT
			CMAKE_C_COMPILER_LAUNCHER
	AND
		NOT
			CMAKE_ASM_COMPILER_LAUNCHER
)
	message(STATUS "Found sccache: ${SCCACHE_PROGRAM}")
	set(SCCACHE_DIR_PATH "${CMAKE_SOURCE_DIR}/sccache")
	set(
		CMAKE_C_COMPILER_LAUNCHER
		"${CMAKE_COMMAND}"
		-E
		env
		"SCCACHE_DIR=${SCCACHE_DIR_PATH}"
		"${SCCACHE_PROGRAM}"
	)
	set(
		CMAKE_ASM_COMPILER_LAUNCHER
		"${CMAKE_COMMAND}"
		-E
		env
		"SCCACHE_DIR=${SCCACHE_DIR_PATH}"
		"${SCCACHE_PROGRAM}"
	)
	message(STATUS "sccache directory: ${SCCACHE_DIR_PATH}")
elseif(CMAKE_C_COMPILER_LAUNCHER OR CMAKE_ASM_COMPILER_LAUNCHER)
	message(
		STATUS
		"Not using sccache: CMAKE_C_COMPILER_LAUNCHER already set to '${CMAKE_C_COMPILER_LAUNCHER}' or CMAKE_ASM_COMPILER_LAUNCHER already set to '${CMAKE_ASM_COMPILER_LAUNCHER}'"
	)
else()
	message(STATUS "Not using sccache: sccache not found in PATH")
endif()

# Compiler Cache Configuration
find_program(CCACHE_PROGRAM ccache)
if(
	CCACHE_PROGRAM
	AND
		NOT
			CMAKE_C_COMPILER_LAUNCHER
	AND
		NOT
			CMAKE_ASM_COMPILER_LAUNCHER
)
	message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
	set(CCACHE_DIR_PATH "${CMAKE_SOURCE_DIR}/ccache")
	set(
		CMAKE_C_COMPILER_LAUNCHER
		"${CMAKE_COMMAND}"
		-E
		env
		"CCACHE_DIR=${CCACHE_DIR_PATH}"
		"${CCACHE_PROGRAM}"
	)
	set(
		CMAKE_ASM_COMPILER_LAUNCHER
		"${CMAKE_COMMAND}"
		-E
		env
		"CCACHE_DIR=${CCACHE_DIR_PATH}"
		"${CCACHE_PROGRAM}"
	)
	message(STATUS "ccache directory: ${CCACHE_DIR_PATH}")
elseif(CMAKE_C_COMPILER_LAUNCHER OR CMAKE_ASM_COMPILER_LAUNCHER)
	message(
		STATUS
		"Not using ccache: CMAKE_C_COMPILER_LAUNCHER already set to '${CMAKE_C_COMPILER_LAUNCHER}' or CMAKE_ASM_COMPILER_LAUNCHER already set to '${CMAKE_ASM_COMPILER_LAUNCHER}'"
	)
else()
	message(STATUS "Not using ccache: ccache not found in PATH")
endif()

# Globals
set(LIB_PATH "Lib")
set(platform_path "${LIB_PATH}/Platform")

include("${LIB_PATH}/cmake/cube.cmake")

add_arm_cmsis_core(
    MAJOR "5"
    GIT_TAG "5.9.0" # https://github.com/ARM-software/CMSIS_5/tree/5.9.0
)

# Vendor
include("${LIB_PATH}/Vendor/CMSIS_5/cmsis_5.cmake")
include("${LIB_PATH}/Vendor/STM32_HAL_LL/cube.cmake")
add_subdirectory("${LIB_PATH}/Vendor/BCC")
include("${LIB_PATH}/Vendor/MLX90640/combined.cmake")
include("${LIB_PATH}/Vendor/MLX90614/combined.cmake")

# Platforms
include("${platform_path}/STM32G474xE/chip.cmake")
include("${platform_path}/STM32G431x8/chip.cmake")
include("${platform_path}/STM32U5A9xJ/chip.cmake")
include("${platform_path}/STM32L476xG/chip.cmake")
include("${platform_path}/STM32H523xE/chip.cmake")

# Utils
include("${LIB_PATH}/GlobalShare/common.cmake")
include("${LIB_PATH}/Utils/OldLogomatic/logomatic.cmake")
include("${LIB_PATH}/Utils/Logomatic/log.cmake")
include("${LIB_PATH}/Utils/CubeVCP/vcp.cmake")
include("${LIB_PATH}/Utils/VirtualComPort/vcp.cmake")
include("${LIB_PATH}/Utils/CircularBuffer/circular-buffer-lib.cmake")
include("${LIB_PATH}/Utils/MsgBuffer/msg-buffer-lib.cmake")
include("${LIB_PATH}/cmake/gr-lib.cmake")
include("${LIB_PATH}/Utils/BitManipulations/bit-utils.cmake")

# Autogen
include("Autogen/CAN/CANfigurator.cmake")

# Peripherals
include("${LIB_PATH}/Peripherals/USART/common.cmake")
include("${LIB_PATH}/Peripherals/TimedCAN/common.cmake")
include("${LIB_PATH}/Peripherals/CAN/common.cmake")
include("${LIB_PATH}/Peripherals/CubeCAN/cube_can.cmake")

include("${LIB_PATH}/FancyLayers-RENAME/GRCAN/grcan_fancylayer.cmake")
include("${LIB_PATH}/FancyLayers-RENAME/ADC/adc.cmake")
include("${LIB_PATH}/FancyLayers-RENAME/SPI/spi.cmake")
include("${LIB_PATH}/FancyLayers-RENAME/NeoPixel/neopixel.cmake")

# Projects
include(BLINKY/blinky.cmake)
include(Testbench/testbench.cmake)

add_gr_project(STM32G474xE ECU)
add_gr_project(STM32G474xE CCU)
add_gr_project(STM32G474xE CANine)
add_gr_project(STM32G431x8 OutboardSensors TireTemp)
add_gr_project(STM32G431x8 OutboardSensors BrakeTemp)
add_gr_project(STM32L476xG BLINKY L4BLINKY)
add_gr_project(STM32L476xG BLINKY L4HELLO)

# add_project(
#     NAME "ECU"
#     PATH "ECU"
#     SERIES "g4"
#     SPECIFIER "g474re"
#     HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
#     CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
#     CMSIS_MAJOR "5"
# )

# add_project(
#     NAME "CCU"
#     PATH "CCU"
#     SERIES "g4"
#     SPECIFIER "g474re"
#     HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
#     CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
#     CMSIS_MAJOR "5"
# )

# add_project(
#     NAME "CANine"
#     PATH "CANine"
#     SERIES "g4"
#     SPECIFIER "g474re"
#     HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
#     CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
#     CMSIS_MAJOR "5"
# )

# add_project(
#     NAME "TireTemp"
#     PATH "OutboardSensors/TireTemp"
#     SERIES "g4"
#     SPECIFIER "g474re"
#     HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
#     CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
#     CMSIS_MAJOR "5"
# )

# add_project(
#     NAME "BrakeTemp"
#     PATH "OutboardSensors/BrakeTemp"
#     SERIES "g4"
#     SPECIFIER "g474re"
#     HAL_GIT_TAG "v1.2.7" # https://github.com/STMicroelectronics/stm32g4xx-hal-driver/tree/v1.2.7
#     CMSIS_GIT_TAG "v1.2.6" # https://github.com/STMicroelectronics/cmsis-device-g4/tree/v1.2.6
#     CMSIS_MAJOR "5"
# )
