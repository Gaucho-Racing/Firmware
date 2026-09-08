add_library(GRCAN_FANCYLAYER INTERFACE)

target_include_directories(
	GRCAN_FANCYLAYER
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
	#${CMAKE_CURRENT_LIST_DIR}/TemporaryHoldover/Inc
	#${Project_SOURCE_DIR}/Autogen/CAN//Inc
	#${PROJECT_SOURCE_DIR}/Lib/Peripherals/CAN/Inc
	#${PROJECT_SOURCE_DIR}/Lib/Utils/CircularBuffer/Inc
)

target_sources(
	GRCAN_FANCYLAYER
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/grcan_fancylayer.c
		${CMAKE_CURRENT_LIST_DIR}/Src/grcan_utils.c
)

target_link_libraries(
	GRCAN_FANCYLAYER
	INTERFACE
		GLOBALSHARE_LIB
		CANfigurator
		CircularBuffer_Lib
		PERIPHERAL_CAN_LIB
		#GR_OLD_CAN_MESSAGES
		STM32_HAL_LL_G4xx
	#GRCAN_LIB #maybe add back
)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	# target_compile_definitions(LOGOMATIC_OLD_LIB INTERFACE LOGOMATIC_ENABLED)

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
	# target_link_libraries(logomatic_simple PRIVATE LOGOMATIC_OLD_LIB)
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
	# target_link_libraries(logomatic_float PRIVATE LOGOMATIC_OLD_LIB)
	# add_test(logomatic_float_test logomatic_float)
endif()

set_target_properties(
	GRCAN_FANCYLAYER
	PROPERTIES
		DEPRECATION
			"GRCAN_FANCYLAYER is deprecated and should be replaced by CUBE_CAN_LIB."
)
