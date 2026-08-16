add_library(LOGOMATIC_LIB INTERFACE)

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

if(NOT DEFINED CMAKE_LOGOMATIC_ENABLED)
	set(CMAKE_LOGOMATIC_ENABLED OFF)
endif()

if(CMAKE_LOGOMATIC_ENABLED)
	message(STATUS "Logomatic enabled")
	target_compile_definitions(LOGOMATIC_LIB INTERFACE LOGOMATIC_ENABLED)
endif()

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	# TODO
else()
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
