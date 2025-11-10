add_library(CircularBuffer_Lib INTERFACE)

target_sources(
	CircularBuffer_Lib
	INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Src/circularBuffer.c
)
target_include_directories(
	CircularBuffer_Lib
	INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc
)

# link test to this library
if(CMAKE_BUILD_TYPE STREQUAL "Test")
	# Initialization
	add_executable(
		CircularBuffer_Lib_Initialization_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testInitialization.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Initialization_test
		CircularBuffer_Lib
	)
	add_test(
		CircularBuffer_Lib_Initialization
		CircularBuffer_Lib_Initialization_test
	)

	# Push/Pop
	add_executable(
		CircularBuffer_Lib_Push_Pop_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testPushPop.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Push_Pop_test
		CircularBuffer_Lib
	)
	add_test(
		CircularBuffer_Lib_Push_Pop
		CircularBuffer_Lib_Push_Pop_test
	)

	# Peek
	add_executable(
		CircularBuffer_Lib_Peek_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testPeek.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Peek_test
		CircularBuffer_Lib
	)
	add_test(
		CircularBuffer_Lib_Peek
		CircularBuffer_Lib_Peek_test
	)

	# Get Capacity
	add_executable(
		CircularBuffer_Lib_Get_Capacity_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testGetCapacity.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Get_Capacity_test
		CircularBuffer_Lib
	)
	add_test(
		CircularBuffer_Lib_Get_Capacity
		CircularBuffer_Lib_Get_Capacity_test
	)

	# Get Current Size
	add_executable(
		CircularBuffer_Lib_Get_Current_Size_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testGetCurrentSize.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Get_Current_Size_test
		CircularBuffer_Lib
	)
	add_test(
		CircularBuffer_Lib_Get_Current_Size
		CircularBuffer_Lib_Get_Current_Size_test
	)

	# If Full
	add_executable(
		CircularBuffer_Lib_If_Full_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testIfFull.c
	)
	target_link_libraries(
		CircularBuffer_Lib_If_Full_test
		CircularBuffer_Lib
	)
	add_test(
		CircularBuffer_Lib_If_Full
		CircularBuffer_Lib_If_Full_test
	)

	# Is Empty
	add_executable(
		CircularBuffer_Lib_Is_Empty_test
		${CMAKE_CURRENT_LIST_DIR}/Test/testIsEmpty.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Is_Empty_test
		CircularBuffer_Lib
	)
	add_test(
		CircularBuffer_Lib_Is_Empty
		CircularBuffer_Lib_Is_Empty_test
	)
endif()
