add_library(PERIPHERAL_CAN_LIB INTERFACE)

target_sources(PERIPHERAL_CAN_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/can.c)

target_link_libraries(
	PERIPHERAL_CAN_LIB INTERFACE CircularBuffer_Lib 
)

# Make headers accessible as #include "Peripherals/USART/usart.h"
target_include_directories(
	PERIPHERAL_CAN_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/../..
)

