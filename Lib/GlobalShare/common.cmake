add_library(GLOBALSHARE_LIB INTERFACE)

target_include_directories(
	GLOBALSHARE_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

if(
	CMAKE_PRESET_NAME
		STREQUAL
		"Debug"
	OR
		CMAKE_PRESET_NAME
			STREQUAL
			"RelWithDebInfo"
)
	target_compile_definitions(GLOBALSHARE_LIB INTERFACE LOGOMATIC_ENABLED)
	add_compile_options(
		-u
		_printf_float
	)
	add_link_options(
		-u
		_printf_float
	)
endif()

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	target_compile_definitions(GLOBALSHARE_LIB INTERFACE LOGOMATIC_ENABLED)

	add_executable(stringification)
	target_sources(
		stringification
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/stringification.c
	)
	target_link_libraries(stringification PRIVATE GLOBALSHARE_LIB)
	add_test(stringification_test stringification)

	add_executable(unused)
	target_sources(unused PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Test/unused.c)
	target_link_libraries(unused PRIVATE GLOBALSHARE_LIB)
	add_test(unused_test unused)
endif()
