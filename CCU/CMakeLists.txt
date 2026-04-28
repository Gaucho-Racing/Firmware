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
	add_executable(CCUStateTicksTests)
	target_include_directories(
		CCUStateTicksTests
		PRIVATE
			Application/Inc
			Test/Inc
	)
	target_sources(
		CCUStateTicksTests
		PRIVATE
			Test/Src/can.c
			Test/Src/CCUStateTicksTests.c
			Test/Src/stm32g4xx_hal.c
			Test/Src/stm32g4xx_ll_gpio.c
			Application/Src/StateTicks.c
			Application/Src/StateUtils.c
	)
	target_link_libraries(
		CCUStateTicksTests
		PRIVATE
			GLOBALSHARE_LIB
			LOGOMATIC_LIB
			BitManipulations_Lib
	)
	add_test(CCUStateTicksTests CCUStateTicksTests)
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
		Core/Src/gpio.c
		Core/Src/main.c
		Core/Src/stm32g4xx_hal_msp.c
		Core/Src/stm32g4xx_it.c
		# Application
		Application/Src/CANdler.c
		Application/Src/StateTicks.c
		Application/Src/StateUtils.c
)

target_link_libraries(
	${GR_PROJECT_NAME}_USER_CODE
	INTERFACE
		CANfigurator
		BitManipulations_Lib
		PERIPHERAL_CAN_LIB
		VCP_LIB
)

target_include_directories(
	${GR_PROJECT_NAME}_USER_CODE
	INTERFACE
		Application/Inc
		Core/Inc
)
