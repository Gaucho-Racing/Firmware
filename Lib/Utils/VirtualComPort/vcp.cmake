add_library(VCP_LIB INTERFACE)

target_include_directories(VCP_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_sources(VCP_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Src/vcp.c)

target_link_libraries(
	VCP_LIB
	INTERFACE
		GLOBALSHARE_LIB
		LOGOMATIC_LIB
)
