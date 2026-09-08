add_library(CUBE_CAN_LIB INTERFACE)

target_include_directories(CUBE_CAN_LIB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)

target_sources(
    CUBE_CAN_LIB
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/Src/can_assert.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/can_global.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/can_init.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/can_rx.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/can_tx.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/can_utils.c
)

target_link_libraries(
    CUBE_CAN_LIB
    INTERFACE
    CANfigurator
    LOGOMATIC_LIB
)

if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
    function(add_cubecan_test target source)
        add_executable(${target})
        target_sources(
            ${target}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/Test/${source}
            ${CMAKE_CURRENT_LIST_DIR}/Test/cubecan_test_hal.c
        )
        target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Test ${CMAKE_CURRENT_LIST_DIR}/Src)
        target_link_libraries(${target} PRIVATE CUBE_CAN_LIB LOGOMATIC_LIB GLOBALSHARE_LIB pthread)
        add_test(${target} ${target})
    endfunction()

    add_cubecan_test(CubeCAN_Init_test cubecan_init_test.c)
    add_cubecan_test(CubeCAN_Tx_test cubecan_tx_test.c)
    add_cubecan_test(CubeCAN_Rx_test cubecan_rx_test.c)
    add_cubecan_test(CubeCAN_Error_test cubecan_error_test.c)
    add_cubecan_test(CubeCAN_Utils_test cubecan_utils_test.c)
    add_cubecan_test(CubeCAN_Rate_test cubecan_rate_test.c)
    add_cubecan_test(CubeCAN_RxFailure_test cubecan_rx_failure_test.c)
    add_cubecan_test(CubeCAN_CleanupFailure_test cubecan_cleanup_failure_test.c)
    add_cubecan_test(CubeCAN_MultiInstance_test cubecan_multi_instance_test.c)
    add_cubecan_test(CubeCAN_TxSizes_test cubecan_tx_sizes_test.c)
    add_cubecan_test(CubeCAN_RecoveryFailure_test cubecan_recovery_failure_test.c)
endif()
