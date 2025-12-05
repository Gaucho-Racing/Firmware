add_library(CMSIS_5_CORE INTERFACE)

target_include_directories(
	CMSIS_5_CORE
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc/STM32G4xx
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc/STM32L4xx
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc/STM32U5xx
)

# Add additional CMSIS components as seperate libraries as needed, e.g.:
# - DSP (Digital Signal Processing)
# - RTOS (Real-Time Operating System)
# - NN (Neural Network)
# - Drivers (Peripheral Drivers)
