add_library(GLOBALSHARE_LIB INTERFACE)

target_include_directories(GLOBALSHARE_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

if(CMAKE_PRESET_NAME STREQUAL "Debug" OR CMAKE_PRESET_NAME STREQUAL "RelWithDebInfo")
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

	add_executable(logomatic_simple)
	target_sources(logomatic_simple PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Test/logomatic_simple_print.c)
	target_link_libraries(logomatic_simple PRIVATE GLOBALSHARE_LIB)
	add_test(logomatic_simple_test logomatic_simple)

	add_executable(logomatic_float)
	target_sources(logomatic_float PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Test/logomatic_float_print.c)
	target_link_libraries(logomatic_float PRIVATE GLOBALSHARE_LIB)
	add_test(logomatic_float_test logomatic_float)

	add_executable(stringification)
	target_sources(stringification PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Test/stringification.c)
	target_link_libraries(stringification PRIVATE GLOBALSHARE_LIB)
	add_test(stringification_test stringification)

	add_executable(unused)
	target_sources(unused PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Test/unused.c)
	target_link_libraries(unused PRIVATE GLOBALSHARE_LIB)
	add_test(unused_test unused)
endif()
