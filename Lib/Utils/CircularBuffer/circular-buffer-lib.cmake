set(CircularBuffer_DIR $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

add_library(CircularBuffer_Lib INTERFACE)
target_sources(
	CircularBuffer_Lib
	INTERFACE
	${CircularBuffer_DIR}/Src/circularBuffer.c
)
target_include_directories(
	CircularBuffer_Lib
	INTERFACE
	${CircularBuffer_DIR}/Inc
)

# link test to this library
if(CMAKE_BUILD_TYPE STREQUAL "Test")
	# set the debug flag for debugger to work in case codes need fixings.
	set(CMAKE_BUILD_TYPE Debug)
	# Windows MinGW does not seem to support address sanatizers
	if(UNIX)
		# add integrity check of syntax and memory
		set(CMAKE_C_FLAGS_PREV "${CMAKE_C_FLAGS}")
		set(
			CMAKE_C_FLAGS
			"${CMAKE_C_FLAGS} -Werror -fsanitize=undefined -fsanitize=address"
		)
	endif()

	add_executable(
		CircularBuffer_Lib_Initialization_test
		${CircularBuffer_DIR}/Test/testInitialization.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Initialization_test
		CircularBuffer_Lib
	)
	if(UNIX)
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

	add_executable(CircularBuffer_Lib_Push_Pop_test ${CircularBuffer_DIR}/Test/testPushPop.c)
	target_link_libraries(CircularBuffer_Lib_Push_Pop_test CircularBuffer_Lib)
	if(UNIX)
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

	add_executable(CircularBuffer_Lib_Peek_test ${CircularBuffer_DIR}/Test/testPeek.c)
	target_link_libraries(CircularBuffer_Lib_Peek_test CircularBuffer_Lib)
	if(UNIX)
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

	add_executable(CircularBuffer_Lib_Get_Capacity_test ${CircularBuffer_DIR}/Test/testGetCapacity.c)
	target_link_libraries(CircularBuffer_Lib_Get_Capacity_test CircularBuffer_Lib)
	if(UNIX)
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
		${CircularBuffer_DIR}/Test/testGetCurrentSize.c
	)
	target_link_libraries(
		CircularBuffer_Lib_Get_Current_Size_test
		CircularBuffer_Lib
	)
	if(UNIX)
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

	add_executable(CircularBuffer_Lib_If_Full_test ${CircularBuffer_DIR}/Test/testIfFull.c)
	target_link_libraries(CircularBuffer_Lib_If_Full_test CircularBuffer_Lib)
	if(UNIX)
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

	add_executable(CircularBuffer_Lib_Is_Empty_test ${CircularBuffer_DIR}/Test/testIsEmpty.c)
	target_link_libraries(CircularBuffer_Lib_Is_Empty_test CircularBuffer_Lib)
	if(UNIX)
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
	if(UNIX)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_PREV}")
	endif()
endif()
