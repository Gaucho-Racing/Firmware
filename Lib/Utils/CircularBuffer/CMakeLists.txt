cmake_minimum_required(VERSION 3.25)

include_directories(Inc)

if(CMAKE_BUILD_TYPE STREQUAL "Test")
	add_executable(CircularBuffer_Lib_basic_test Test/basic_test.c)
	add_test(CircularBuffer_Lib_basic CircularBuffer_Lib_basic_test)
endif()

add_library(CircularBuffer_Lib INTERFACE)
target_sources(CircularBuffer_Lib INTERFACE Src/circularBuffer.c)

target_include_directories(CircularBuffer_Lib INTERFACE Inc)

# link test to this library
if(CMAKE_BUILD_TYPE STREQUAL "Test")
	target_link_libraries(CircularBuffer_Lib_basic_test CircularBuffer_Lib)
endif()
