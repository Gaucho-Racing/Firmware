add_library(ADC INTERFACE)

target_include_directories(
	ADC
	INTERFACE
		${CMAKE_CURRENT_LIST_DIR}/Inc
		${CMAKE_CURRENT_LIST_DIR}/Src
)
