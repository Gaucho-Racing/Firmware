add_library(NEOPIXEL_LIB INTERFACE)

target_include_directories(NEOPIXEL_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_sources(
	NEOPIXEL_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/neopixel_assert.c
		${CMAKE_CURRENT_LIST_DIR}/Src/neopixel_global.c
		${CMAKE_CURRENT_LIST_DIR}/Src/neopixel_init.c
		${CMAKE_CURRENT_LIST_DIR}/Src/neopixel_tx.c
)

target_link_libraries(NEOPIXEL_LIB INTERFACE LOGOMATIC_LIB)

if(BUILD_TESTING)
	# TODO
endif()
