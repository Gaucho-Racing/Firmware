add_library(ADC INTERFACE)

target_sources(ADC INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Src/gr_adc.c)

target_include_directories(ADC INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_link_libraries(ADC INTERFACE GLOBALSHARE_LIB)
