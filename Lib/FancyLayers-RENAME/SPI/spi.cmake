add_library(SPI_Lib INTERFACE)

target_sources(SPI_Lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Src/spi.c)
target_include_directories(SPI_Lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_link_libraries(
	SPI_Lib
	INTERFACE
		MsgBuffer_Lib
		GLOBALSHARE_LIB
)

set_target_properties(
	SPI_Lib
	PROPERTIES
		DEPRECATION
			"SPI_Lib is deprecated and should be removed in favor of STM32CubeMX functionality."
)
