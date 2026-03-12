add_library(CMSIS_5_CORE INTERFACE)

target_include_directories(
	CMSIS_5_CORE
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc/STM32G4xx
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc/STM32L4xx
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc/STM32U5xx
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc/STM32H5xx
)

add_library(CMSIS_5_DSP INTERFACE)

target_include_directories(
	CMSIS_5_DSP
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/DSP/Include
)

set(CMSIS_5_DSP_SOURCE_ROOT ${CMAKE_CURRENT_LIST_DIR}/DSP/Source)

# Add additional CMSIS components as seperate libraries as needed, e.g.:
# - DSP (Digital Signal Processing)
# - RTOS (Real-Time Operating System)
# - NN (Neural Network)
# - Drivers (Peripheral Drivers)
