add_library(VCP_LIB INTERFACE)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	target_include_directories(
		VCP_LIB
		INTERFACE
			${CMAKE_CURRENT_LIST_DIR}/Test/Inc
	)
	target_sources(VCP_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Test/Src/vcp.c)
else()
	target_include_directories(VCP_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)
	target_sources(VCP_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Src/vcp.c)
endif()

target_link_libraries(
	VCP_LIB
	INTERFACE
		GLOBALSHARE_LIB
		LOGOMATIC_OLD_LIB
)
