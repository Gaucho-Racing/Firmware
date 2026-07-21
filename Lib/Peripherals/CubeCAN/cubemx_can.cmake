add_library(CUBEMX_CAN_LIB INTERFACE)

target_include_directories(
	CUBEMX_CAN_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

target_sources(
	CUBEMX_CAN_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/can_assert.c
		${CMAKE_CURRENT_LIST_DIR}/Src/can_global.c
		${CMAKE_CURRENT_LIST_DIR}/Src/can_init.c
		${CMAKE_CURRENT_LIST_DIR}/Src/can_rx.c
		${CMAKE_CURRENT_LIST_DIR}/Src/can_tx.c
		${CMAKE_CURRENT_LIST_DIR}/Src/can_utils.c
)

target_link_libraries(
	CUBEMX_CAN_LIB
	INTERFACE
		CANfigurator
)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	# FIXME Add HOOTL tests
endif()
