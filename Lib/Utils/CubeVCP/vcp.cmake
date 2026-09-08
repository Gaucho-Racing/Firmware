add_library(CUBE_VCP_LIB INTERFACE)

target_include_directories(CUBE_VCP_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_sources(
	CUBE_VCP_LIB
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/vcp_assert.c
		${CMAKE_CURRENT_LIST_DIR}/Src/vcp_global.c
		${CMAKE_CURRENT_LIST_DIR}/Src/vcp_init.c
		${CMAKE_CURRENT_LIST_DIR}/Src/vcp_rx.c
		${CMAKE_CURRENT_LIST_DIR}/Src/vcp_tx.c
)

target_link_libraries(CUBE_VCP_LIB INTERFACE LOGOMATIC_LIB)
