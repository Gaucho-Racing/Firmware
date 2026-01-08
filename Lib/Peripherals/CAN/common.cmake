add_library(PERIPHERAL_CAN_LIB INTERFACE)

target_sources(PERIPHERAL_CAN_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/can.c)

target_link_libraries(
	PERIPHERAL_CAN_LIB INTERFACE CircularBuffer_Lib 
)

# Make headers accessible as #include "Peripherals/CAN/can.h"
target_include_directories(
	PERIPHERAL_CAN_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/../..
)

# link test to this library
if(CMAKE_BUILD_TYPE STREQUAL "Test")
# Initialization
	add_executable(
		PERIPHERAL_CAN_LIB_init_test
		${CMAKE_CURRENT_LIST_DIR}/Test/can_test_init.c
	)
	target_link_libraries(
		PERIPHERAL_CAN_LIB_init_test
		PERIPHERAL_CAN_LIB
	)
	add_test(
		CircularBuffer_Lib_Initialization
		CircularBuffer_Lib_Initialization_test
	)

endif()
