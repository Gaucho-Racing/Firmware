cmake_minimum_required(VERSION 3.25)

add_library(CircularBuffer_Lib INTERFACE)
target_sources(CircularBuffer_Lib INTERFACE Src/circularBuffer.c)
target_include_directories(CircularBuffer_Lib INTERFACE Inc)
if(WIN32)
	message(STATUS "Detected Windows — disabling Linux-only features.")
	set(SKIP_UNIX_FEATURES TRUE)
else()
	message(STATUS "Non-Windows system — enabling full features.")
endif()
# link test to this library
if(CMAKE_BUILD_TYPE STREQUAL "Test")
	# set the debug flag for debugger to work in case codes need fixings.
	set(CMAKE_BUILD_TYPE Debug)
	# add integrity check of syntax and memory
	set(CMAKE_C_FLAGS_PREV "${CMAKE_C_FLAGS}")
	# Windows MinGW does not seem to support address sanatizers
	if(NOT WIN32)
		set(
			CMAKE_C_FLAGS
			"${CMAKE_C_FLAGS} -Werror -fsanitize=undefined -fsanitize=address"
		)
	endif()

	add_executable(
		CircularBuffer_Lib_Initialization_test
		Test/testInitialization.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Initialization_test
		CircularBuffer_Lib
	)
	if(NOT WIN32)
		target_link_options(
			CircularBuffer_Lib_Initialization_test
			BEFORE
			PUBLIC
				-fsanitize=undefined
			PUBLIC
				-fsanitize=address
		)
	endif()
	add_test(
		CircularBuffer_Lib_Initialization
		CircularBuffer_Lib_Initialization_test
	)

	add_executable(CircularBuffer_Lib_Push_Pop_test Test/testPushPop.c)
	target_link_libraries(CircularBuffer_Lib_Push_Pop_test CircularBuffer_Lib)
	if(NOT WIN32)
		target_link_options(
			CircularBuffer_Lib_Push_Pop_test
			BEFORE
			PUBLIC
				-fsanitize=undefined
			PUBLIC
				-fsanitize=address
		)
	endif()
	add_test(CircularBuffer_Lib_Push_Pop CircularBuffer_Lib_Push_Pop_test)

	add_executable(CircularBuffer_Lib_Peek_test Test/testPeek.c)
	target_link_libraries(CircularBuffer_Lib_Peek_test CircularBuffer_Lib)
	if(NOT WIN32)
		target_link_options(
			CircularBuffer_Lib_Peek_test
			BEFORE
			PUBLIC
				-fsanitize=undefined
			PUBLIC
				-fsanitize=address
		)
	endif()
	add_test(CircularBuffer_Lib_Peek CircularBuffer_Lib_Peek_test)

	add_executable(CircularBuffer_Lib_Get_Capacity_test Test/testGetCapacity.c)
	target_link_libraries(CircularBuffer_Lib_Get_Capacity_test CircularBuffer_Lib)
	if(NOT WIN32)
		target_link_options(
			CircularBuffer_Lib_Get_Capacity_test
			BEFORE
			PUBLIC
				-fsanitize=undefined
			PUBLIC
				-fsanitize=address
		)
	endif()
	add_test(CircularBuffer_Lib_Get_Capacity CircularBuffer_Lib_Get_Capacity_test)

	add_executable(
		CircularBuffer_Lib_Get_Current_Size_test
		Test/testGetCurrentSize.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Get_Current_Size_test
		CircularBuffer_Lib
	)
	if(NOT WIN32)
		target_link_options(
			CircularBuffer_Lib_Get_Current_Size_test
			BEFORE
			PUBLIC
				-fsanitize=undefined
			PUBLIC
				-fsanitize=address
		)
	endif()
	add_test(
		CircularBuffer_Lib_Get_Current_Size
		CircularBuffer_Lib_Get_Current_Size_test
	)

	add_executable(CircularBuffer_Lib_If_Full_test Test/testIfFull.c)
	target_link_libraries(CircularBuffer_Lib_If_Full_test CircularBuffer_Lib)
	if(NOT WIN32)
		target_link_options(
			CircularBuffer_Lib_If_Full_test
			BEFORE
			PUBLIC
				-fsanitize=undefined
			PUBLIC
				-fsanitize=address
		)
	endif()
	add_test(CircularBuffer_Lib_If_Full CircularBuffer_Lib_If_Full_test)

	add_executable(CircularBuffer_Lib_Is_Empty_test Test/testIsEmpty.c)
	target_link_libraries(CircularBuffer_Lib_Is_Empty_test CircularBuffer_Lib)
	if(NOT WIN32)
		target_link_options(
			CircularBuffer_Lib_Is_Empty_test
			BEFORE
			PUBLIC
				-fsanitize=undefined
			PUBLIC
				-fsanitize=address
		)
	endif()
	add_test(CircularBuffer_Lib_Is_Empty CircularBuffer_Lib_Is_Empty_test)

	# Revert the variables to not disturb other processes.
	set(CMAKE_BUILD_TYPE Test)
	if(NOT WIN32)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_PREV}")
	endif()
endif()
