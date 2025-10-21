cmake_minimum_required(VERSION 3.25)

add_library(CircularBuffer_Lib INTERFACE)
target_sources(CircularBuffer_Lib INTERFACE Src/circularBuffer.c)
target_include_directories(CircularBuffer_Lib INTERFACE Inc)

# link test to this library
if(CMAKE_BUILD_TYPE STREQUAL "Test")
	add_executable(
		CircularBuffer_Lib_Initialization_test
		Test/testInitialization.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Initialization_test
		CircularBuffer_Lib
	)
	add_test(
		CircularBuffer_Lib_Initialization
		CircularBuffer_Lib_Initialization_test
	)

	add_executable(CircularBuffer_Lib_Push_Pop_test Test/testPushPop.c)
	target_link_libraries(CircularBuffer_Lib_Push_Pop_test CircularBuffer_Lib)
	add_test(CircularBuffer_Lib_Push_Pop CircularBuffer_Lib_Push_Pop_test)

	add_executable(CircularBuffer_Lib_Peek_test Test/testPeek.c)
	target_link_libraries(CircularBuffer_Lib_Peek_test CircularBuffer_Lib)
	add_test(CircularBuffer_Lib_Peek CircularBuffer_Lib_Peek_test)
endif()
