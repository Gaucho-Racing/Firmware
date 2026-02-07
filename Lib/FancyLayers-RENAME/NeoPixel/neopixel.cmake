add_library(GR_NEOPIXEL INTERFACE)

target_sources(
	GR_NEOPIXEL
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/gr_neopixel.c
)

target_include_directories(GR_NEOPIXEL INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_link_libraries(GR_NEOPIXEL INTERFACE GLOBALSHARE_LIB)
