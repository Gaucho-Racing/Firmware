add_library(gr_ADC INTERFACE)

target_sources(gr_ADC INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Src/gr_adc.c)

target_include_directories(gr_ADC INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_link_libraries(gr_ADC INTERFACE GLOBALSHARE_LIB)
