add_library(WS2812SPI_LIB INTERFACE)

target_include_directories(
	WS2812SPI_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

target_sources(
	WS2812SPI_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/ws2812spi_assert.c
		${CMAKE_CURRENT_LIST_DIR}/Src/ws2812spi_global.c
		${CMAKE_CURRENT_LIST_DIR}/Src/ws2812spi_init.c
		${CMAKE_CURRENT_LIST_DIR}/Src/ws2812spi_tx.c
)

target_link_libraries(WS2812SPI_LIB INTERFACE LOGOMATIC_LIB)

if(BUILD_TESTING)
	# TODO
endif()
