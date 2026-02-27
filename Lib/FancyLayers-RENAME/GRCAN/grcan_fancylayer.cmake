add_library(GRCAN_FANCYLAYER INTERFACE)

target_include_directories(
	GRCAN_FANCYLAYER
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

target_sources(
	GRCAN_FANCYLAYER
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/grcan_fancylayer.c
)

target_link_libraries(
	GRCAN_FANCYLAYER
	INTERFACE
		GLOBALSHARE_LIB
		GRCAN_LIB
)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	# target_compile_definitions(LOGOMATIC_LIB INTERFACE LOGOMATIC_ENABLED)

	# add_executable(logomatic_simple)
	# target_sources(
	# 	logomatic_simple
	# 	PRIVATE
	# 		${CMAKE_CURRENT_LIST_DIR}/Test/Src/logomatic_simple_print.c
	# )
	# target_include_directories(
	# 	logomatic_simple
	# 	PRIVATE
	# 		${CMAKE_CURRENT_LIST_DIR}/Test/Inc
	# )
	# target_link_libraries(logomatic_simple PRIVATE LOGOMATIC_LIB)
	# add_test(logomatic_simple_test logomatic_simple)

	# add_executable(logomatic_float)
	# target_sources(
	# 	logomatic_float
	# 	PRIVATE
	# 		${CMAKE_CURRENT_LIST_DIR}/Test/Src/logomatic_float_print.c
	# )
	# target_include_directories(
	# 	logomatic_float
	# 	PRIVATE
	# 		${CMAKE_CURRENT_LIST_DIR}/Test/Inc
	# )
	# target_link_libraries(logomatic_float PRIVATE LOGOMATIC_LIB)
	# add_test(logomatic_float_test logomatic_float)
endif()
