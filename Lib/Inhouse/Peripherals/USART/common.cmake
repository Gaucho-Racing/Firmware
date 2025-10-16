add_library(PERIPHERAL_USART_LIB INTERFACE)

set(_USART_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB _USART_SOURCES "${_USART_DIR}/*.c")

target_sources(PERIPHERAL_USART_LIB INTERFACE ${_USART_SOURCES})

# Make headers accessible as #include "Peripherals/USART/usart.h"
target_include_directories(PERIPHERAL_USART_LIB INTERFACE ${_USART_DIR}/../..)
