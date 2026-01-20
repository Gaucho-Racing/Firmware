cmake_minimum_required(VERSION 3.25)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

# what, does in fact not get the filename of somthing but rather the name of the project from the path
get_filename_component(GR_PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	add_executable(dummy_test)
	target_include_directories(
		dummy_test
		PRIVATE
			Application/Inc
			Core/Inc
	)
	target_sources(dummy_test PRIVATE Test/dummy_test.c)
	target_link_libraries(dummy_test PRIVATE GLOBALSHARE_LIB)
	add_test(dummy_test dummy_test)

	# FIXME Add more HOOTL tests
endif()

# Enable CMake support for ASM and C languages
enable_language(
	C
	ASM
)

# Core project settings
project(${CMAKE_PROJECT_NAME})

# Questionable...
# set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ")
# set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-T${CHIP_PATH}/CompileDependencies/STM32G474RETx_FLASH.ld")

add_library(${GR_PROJECT_NAME}_USER_CODE INTERFACE)
target_sources(
	${GR_PROJECT_NAME}_USER_CODE
	INTERFACE
		# Core
		Core/Src/adc.c
		Core/Src/dma.c
		Core/Src/fdcan.c
		Core/Src/gpio.c
		Core/Src/main.c
		Core/Src/stm32g4xx_hal_msp.c
		Core/Src/stm32g4xx_it.c
		Core/Src/syscalls.c
		Core/Src/sysmem.c
		Core/Src/system_stm32g4xx.c
		Core/Src/usart.c
		# Application
		Application/Src/CANdler.c
		Application/Src/StateTicks.c
		Application/Src/StateUtils.c
		Application/Src/CANutils.c
)

target_link_libraries(
	${GR_PROJECT_NAME}_USER_CODE
	INTERFACE
		GR_OLD_CAN_MESSAGES
		ADC
		PERIPHERAL_CAN_LIB
)

target_include_directories(
	${GR_PROJECT_NAME}_USER_CODE
	INTERFACE
		Application/Inc
		Core/Inc
)
