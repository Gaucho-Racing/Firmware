add_library(GLOBALSHARE_LIB INTERFACE)

target_include_directories(
    GLOBALSHARE_LIB
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Inc>
)
