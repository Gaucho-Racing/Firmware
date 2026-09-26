add_library(SAMM_IMU_Lib INTERFACE)

target_include_directories(SAMM_IMU_Lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_sources(SAMM_IMU_Lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Src/bmi323.c)
