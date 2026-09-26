add_library(GR_ADC INTERFACE)

target_sources(GR_ADC INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Src/gr_adc.c)

target_include_directories(GR_ADC INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_link_libraries(GR_ADC INTERFACE GLOBALSHARE_LIB)

set_target_properties(
	GR_ADC
	PROPERTIES
		DEPRECATION
			"GR_ADC is deprecated and should be removed in favor of STM32CubeMX functionality."
)
