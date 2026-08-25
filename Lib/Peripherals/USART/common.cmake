add_library(PERIPHERAL_USART_LIB INTERFACE)

target_sources(PERIPHERAL_USART_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/usart.c)

# Make headers accessible as #include "Peripherals/USART/usart.h"
target_include_directories(
	PERIPHERAL_USART_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/../..
)

set_target_properties(
	PERIPHERAL_USART_LIB
	PROPERTIES
		DEPRECATION
			"PERIPHERAL_USART_LIB is deprecated and should be removed in favor of STM32CubeMX functionality or VCP_LIB."
)
