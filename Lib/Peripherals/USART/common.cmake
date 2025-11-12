add_library(PERIPHERAL_USART_LIB INTERFACE)

target_sources(PERIPHERAL_USART_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/usart.c)

target_link_libraries(PERIPHERAL_USART_LIB INTERFACE CircularBuffer_Lib)

# Make headers accessible as #include "Peripherals/USART/usart.h"
target_include_directories(
	PERIPHERAL_USART_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/../..
)
