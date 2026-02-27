add_library(MsgBuffer_Lib INTERFACE)

target_sources(
	MsgBuffer_Lib
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Src/msgBuffer.c
)
target_include_directories(
	MsgBuffer_Lib
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

# link test to this library
if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	# Initialization
	add_executable(
		MsgBuffer_Lib_Initialization_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testInitialization.c
	)
	target_link_libraries(MsgBuffer_Lib_Initialization_test MsgBuffer_Lib)
	add_test(MsgBuffer_Lib_Initialization MsgBuffer_Lib_Initialization_test)

	# Push/Pop
	add_executable(
		MsgBuffer_Lib_Push_Pop_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testPushPop.c
	)
	target_link_libraries(MsgBuffer_Lib_Push_Pop_test MsgBuffer_Lib)
	add_test(MsgBuffer_Lib_Push_Pop MsgBuffer_Lib_Push_Pop_test)

	# Is Empty
	add_executable(
		MsgBuffer_Lib_Is_Empty_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testIsEmpty.c
	)
	target_link_libraries(MsgBuffer_Lib_Is_Empty_test MsgBuffer_Lib)
	add_test(MsgBuffer_Lib_Is_Empty MsgBuffer_Lib_Is_Empty_test)
endif()
