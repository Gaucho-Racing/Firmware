add_library(BitManipulations_Lib INTERFACE)

target_include_directories(BitManipulations_Lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_sources(
	BitManipulations_Lib
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/setBits.c
)

if(CMAKE_BUILD_TYPE STREQUAL "Test")
	add_executable(
		BitManipulations_VerifyGetBit_test
		${CMAKE_CURRENT_LIST_DIR}/Test/verifyGetBit.c
	)
	target_link_libraries(BitManipulations_VerifyGetBit_test BitManipulations_Lib)
	add_test(BitManipulations_VerifyGetBit BitManipulations_VerifyGetBit_test)
endif()
