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

# Enable CMake support for ASM and C languages
enable_language(
	C
	ASM
)

# Core project settings
project(${CMAKE_PROJECT_NAME})

add_library(${GR_PROJECT_NAME}_USER_CODE INTERFACE)
target_sources(
	${GR_PROJECT_NAME}_USER_CODE
	INTERFACE
		Core/Src/main.c
		Core/Src/stm32u5xx_it.c
		Core/Src/stm32u5xx_hal_msp.c
		Core/Src/system_stm32u5xx.c
		Core/Src/sysmem.c
		Core/Src/syscalls.c
)

target_link_libraries(${GR_PROJECT_NAME}_USER_CODE INTERFACE)

target_include_directories(
	${GR_PROJECT_NAME}_USER_CODE
	INTERFACE
		Application/Inc
		Core/Inc
)
