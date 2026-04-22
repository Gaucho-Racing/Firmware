add_library(PERIPHERAL_CAN_LIB INTERFACE)

target_link_libraries(
	PERIPHERAL_CAN_LIB
	INTERFACE
		PERIPHERAL_CAN_TEST_LIB
		#PERIPHERAL_CAN_DMA
		CircularBuffer_Lib
)



add_library(PERIPHERAL_CAN_DMA INTERFACE)
target_sources(PERIPHERAL_CAN_DMA
	INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/Src/can_dma.c
)

target_sources(
	PERIPHERAL_CAN_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/can.c
		#${CMAKE_CURRENT_LIST_DIR}/Src/can_dma.c
		${CMAKE_CURRENT_LIST_DIR}/Src/can_cfg_helpers.c
)

# Make headers accessible as #include "Peripherals/CAN/can.h"
target_include_directories(
	PERIPHERAL_CAN_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

#tests
add_library(PERIPHERAL_CAN_TEST_LIB INTERFACE)
target_sources(
	PERIPHERAL_CAN_TEST_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Test/can_basic_test.c
		${CMAKE_CURRENT_LIST_DIR}/Test/can_release_test.c
		${CMAKE_CURRENT_LIST_DIR}/Test/can_stress_test.c
		${CMAKE_CURRENT_LIST_DIR}/Test/can_filter_test.c
		${CMAKE_CURRENT_LIST_DIR}/Test/can_lost_rx_test.c
		${CMAKE_CURRENT_LIST_DIR}/Test/profile.c
)
target_include_directories(
	PERIPHERAL_CAN_TEST_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Test
)

# link test to this library
#if(CMAKE_BUILD_TYPE STREQUAL "Test")
# Initialization
#	add_executable(
#		PERIPHERAL_CAN_LIB_external_test
#		${CMAKE_CURRENT_LIST_DIR}/Test/can_basic_test.c
#	)
#	target_link_libraries(
#		PERIPHERAL_CAN_LIB_init_test
#		PERIPHERAL_CAN_LIB
#	)
#	add_test(
#		PERIPHERAL_CAN_LIB_init
#		PERIPHERAL_CAN_LIB_init_test
#	)
#endif()
