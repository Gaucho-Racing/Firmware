add_library(LOGOMATIC_LIB INTERFACE)

# Include directories and source files for the logomatic library
target_include_directories(
	LOGOMATIC_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)
target_sources(
	LOGOMATIC_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/Logomatic.c
)
target_link_libraries(LOGOMATIC_LIB INTERFACE GLOBALSHARE_LIB)

# Default logomatic to be disabled
if(NOT DEFINED CMAKE_LOGOMATIC_ENABLED)
	set(CMAKE_LOGOMATIC_ENABLED OFF)
endif()

# Main compilation flag to enable logomatic
if(CMAKE_LOGOMATIC_ENABLED)
	message(STATUS "Logomatic enabled")
	target_compile_definitions(LOGOMATIC_LIB INTERFACE LOGOMATIC_ENABLED)
endif()

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	add_executable(logomatic_simple)
	target_sources(
		logomatic_simple
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Src/logomatic_simple_print.c
	)
	target_include_directories(
		logomatic_simple
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Inc
	)
	target_link_libraries(logomatic_simple PRIVATE LOGOMATIC_LIB)
	add_test(logomatic_simple_test logomatic_simple)

	add_executable(logomatic_float)
	target_sources(
		logomatic_float
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Src/logomatic_float_print.c
	)
	target_include_directories(
		logomatic_float
		PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/Test/Inc
	)
	target_link_libraries(logomatic_float PRIVATE LOGOMATIC_LIB)
	add_test(logomatic_float_test logomatic_float)
else()
	# Add floating point support for printf, which goes unused on MacOS hootl builds and throws a warning
	target_compile_options(
		LOGOMATIC_LIB
		INTERFACE
			-u
			_printf_float
	)
	target_link_options(
		LOGOMATIC_LIB
		INTERFACE
			-u
			_printf_float
	)
endif()
