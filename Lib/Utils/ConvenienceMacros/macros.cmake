add_library(ConvenienceMacros_Lib INTERFACE)

target_include_directories(
	ConvenienceMacros_Lib
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
	add_executable(
		ConvenienceMacros_Min_Max
		${CMAKE_CURRENT_LIST_DIR}/Test/verify_min_max.c
	)
	target_link_libraries(
		ConvenienceMacros_Min_Max
		ConvenienceMacros_Lib
	)
	add_test(
		ConvenienceMacros_Min_Max
		ConvenienceMacros_Min_Max
	)
endif()
