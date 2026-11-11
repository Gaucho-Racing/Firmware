add_library(GLVPD_A_COMMON_LIB INTERFACE)

target_include_directories(GLVPD_A_COMMON_LIB
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/Inc/
)

target_sources(GLVPD_A_COMMON_LIB
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/Src/file.c
)
