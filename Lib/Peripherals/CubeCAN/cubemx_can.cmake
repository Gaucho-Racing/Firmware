add_library(CUBEMX_CAN_LIB INTERFACE)

target_include_directories(
	CUBEMX_CAN_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

target_sources(
	CUBEMX_CAN_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/can_it.c
		${CMAKE_CURRENT_LIST_DIR}/Src/can_utils.c
		${CMAKE_CURRENT_LIST_DIR}/Src/can_init.c
)

target_link_libraries(
	CUBEMX_CAN_LIB
	INTERFACE
		CANfigurator
)

# TODO Unit testing
