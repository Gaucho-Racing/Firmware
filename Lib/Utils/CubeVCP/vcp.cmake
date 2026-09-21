add_library(CUBE_VCP_LIB INTERFACE)

target_include_directories(CUBE_VCP_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_sources(
    CUBE_VCP_LIB
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/Src/vcp_assert.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/vcp_global.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/vcp_init.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/vcp_rx.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/vcp_tx.c
)

target_link_libraries(CUBE_VCP_LIB INTERFACE LOGOMATIC_LIB)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
    add_executable(CubeVCP_test)
    target_sources(
        CubeVCP_test
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Test/cubevcp_test.c
        ${CMAKE_CURRENT_LIST_DIR}/Test/cubevcp_test_hal.c
    )
    target_include_directories(
        CubeVCP_test
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Test
        ${CMAKE_CURRENT_LIST_DIR}/Src
    )
    target_link_libraries(
        CubeVCP_test
        PRIVATE
        CUBE_VCP_LIB
        LOGOMATIC_LIB
    )
    add_test(CubeVCP_test CubeVCP_test)
endif()
