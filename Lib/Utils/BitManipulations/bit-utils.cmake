add_library(BitManipulations_Lib INTERFACE)

target_include_directories(
    BitManipulations_Lib
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Inc
)

target_sources(
    BitManipulations_Lib
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Src/getBits.c
        ${CMAKE_CURRENT_LIST_DIR}/Src/setBits.c
)

if (CMAKE_BUILD_TYPE STREQUAL "Test")
    add_executable(BitManipulations_VerifyFullySet_test
        ${CMAKE_CURRENT_LIST_DIR}/Test/verifyFullySet.c
    )
    target_link_libraries(BitManipulations_VerifyFullySet_test BitManipulations_Lib)
    add_test(BitManipulations_VerifyFullySet BitManipulations_VerifyFullySet_test)

    add_executable(BitManipulations_VerifySingleBit_test
        ${CMAKE_CURRENT_LIST_DIR}/Test/verifySingleBit.c
    )
    target_link_libraries(BitManipulations_VerifySingleBit_test BitManipulations_Lib)
    add_test(BitManipulations_VerifySingleBit BitManipulations_VerifySingleBit_test)

    add_executable(BitManipulations_VerifyBitPatterns_test
        ${CMAKE_CURRENT_LIST_DIR}/Test/verifyBitPatterns.c
    )
    target_link_libraries(BitManipulations_VerifyBitPatterns_test BitManipulations_Lib)
    add_test(BitManipulations_VerifyBitPatterns BitManipulations_VerifyBitPatterns_test)

    add_executable(BitManipulations_VerifySetBit_test
        ${CMAKE_CURRENT_LIST_DIR}/Test/verifySetBit.c
    )
    target_link_libraries(BitManipulations_VerifySetBit_test BitManipulations_Lib)
    add_test(BitManipulations_VerifySetBit BitManipulations_VerifySetBit_test)

    add_executable(BitManipulations_VerifySetBitInArray_test
        ${CMAKE_CURRENT_LIST_DIR}/Test/verifySetBitInArray.c
    )
    target_link_libraries(BitManipulations_VerifySetBitInArray_test BitManipulations_Lib)
    add_test(BitManipulations_VerifySetBitInArray BitManipulations_VerifySetBitInArray_test)

    add_executable(BitManipulations_VerifySetBits_test
        ${CMAKE_CURRENT_LIST_DIR}/Test/verifySetBits.c
    )
    target_link_libraries(BitManipulations_VerifySetBits_test BitManipulations_Lib)
    add_test(BitManipulations_VerifySetBits BitManipulations_VerifySetBits_test)
endif()
