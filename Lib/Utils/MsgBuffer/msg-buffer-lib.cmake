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
	target_link_libraries(
		MsgBuffer_Lib_Initialization_test
		MsgBuffer_Lib
	)
	add_test(
		MsgBuffer_Lib_Initialization
		MsgBuffer_Lib_Initialization_test
	)

	# Push/Pop
	add_executable(
		MsgBuffer_Lib_Push_Pop_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testPushPop.c
	)
	target_link_libraries(MsgBuffer_Lib_Push_Pop_test MsgBuffer_Lib)
	add_test(MsgBuffer_Lib_Push_Pop MsgBuffer_Lib_Push_Pop_test)

	# Peek
	add_executable(
		MsgBuffer_Lib_Peek_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testPeek.c
	)
	target_link_libraries(MsgBuffer_Lib_Peek_test MsgBuffer_Lib)
	add_test(MsgBuffer_Lib_Peek MsgBuffer_Lib_Peek_test)

	# Get Capacity
	add_executable(
		MsgBuffer_Lib_Get_Capacity_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testGetCapacity.c
	)
	target_link_libraries(MsgBuffer_Lib_Get_Capacity_test MsgBuffer_Lib)
	add_test(MsgBuffer_Lib_Get_Capacity MsgBuffer_Lib_Get_Capacity_test)

	# Get Current Size
	add_executable(
		MsgBuffer_Lib_Get_Current_Size_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testGetCurrentSize.c
	)
	target_link_libraries(
		MsgBuffer_Lib_Get_Current_Size_test
		MsgBuffer_Lib
	)
	add_test(
		MsgBuffer_Lib_Get_Current_Size
		MsgBuffer_Lib_Get_Current_Size_test
	)

	# If Full
	add_executable(
		MsgBuffer_Lib_If_Full_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testIfFull.c
	)
	target_link_libraries(MsgBuffer_Lib_If_Full_test MsgBuffer_Lib)
	add_test(MsgBuffer_Lib_If_Full MsgBuffer_Lib_If_Full_test)

	# Is Empty
	add_executable(
		MsgBuffer_Lib_Is_Empty_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testIsEmpty.c
	)
	target_link_libraries(MsgBuffer_Lib_Is_Empty_test MsgBuffer_Lib)
	add_test(MsgBuffer_Lib_Is_Empty MsgBuffer_Lib_Is_Empty_test)
endif()
