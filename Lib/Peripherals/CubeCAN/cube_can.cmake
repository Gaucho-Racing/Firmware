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
    add_executable(CAN_File_test)
    target_sources(
        CAN_File_test
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Test/file.c
    )
	target_include_directories(CAN_File_test PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Test)
    target_link_libraries(CAN_File_test PRIVATE CUBE_CAN_LIB LOGOMATIC_LIB)
    add_test(CAN_File_test CAN_File_test)
endif()
