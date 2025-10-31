add_library(GLOBALSHARE_LIB INTERFACE)

target_include_directories(
	GLOBALSHARE_LIB
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Inc>
)

if(
	CMAKE_BUILD_TYPE
		STREQUAL
		"Debug"
	OR
		CMAKE_BUILD_TYPE
			STREQUAL
			"RelWithDebInfo"
)
	target_compile_definitions(GLOBALSHARE_LIB INTERFACE LOGOMATIC_ENABLED)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -u _printf_float")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Test")
	target_compile_definitions(GLOBALSHARE_LIB INTERFACE LOGOMATIC_ENABLED)

	add_executable(logomatic_simple)
	target_include_directories(
		logomatic_simple PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/Inc
	)
	target_sources(
		logomatic_simple PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/Test/logomatic_simple_print.c
	)
	target_link_libraries(logomatic_simple PRIVATE GLOBALSHARE_LIB)
	add_test(logomatic_simple_test logomatic_simple)

	add_executable(logomatic_float)
	target_include_directories(
		logomatic_float PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/Inc
	)
	target_sources(
		logomatic_float PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/Test/logomatic_float_print.c
	)
	target_link_libraries(logomatic_float PRIVATE GLOBALSHARE_LIB)
	add_test(logomatic_float_test logomatic_float)
endif()
