add_library(SPI_Lib INTERFACE)

target_sources(
	SPI_Lib
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/spi.c
)
target_include_directories(
	SPI_Lib
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

target_link_libraries(
	SPI_Lib
	INTERFACE
		GLOBALSHARE_LIB
		CircularBuffer_Lib
)