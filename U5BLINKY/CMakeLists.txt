cmake_minimum_required(VERSION 3.25)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# what, does in fact not get the filename of somthing but rather the name of the project from the path
get_filename_component(GR_PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if(CMAKE_BUILD_TYPE STREQUAL "Test")
    include_directories(
        Application/Inc
        Core/Inc
    )
    
    add_executable(${GR_PROJECT_NAME}_basic_application_hootl_test
        Application/Test/basic_application_hootl_test.c
    )
    add_test(${GR_PROJECT_NAME}_basic_app ${GR_PROJECT_NAME}_basic_application_hootl_test)

    add_executable(${GR_PROJECT_NAME}_advanced_application_hootl_test
        Application/Test/advanced_application_hootl_test.c
        Application/Src/appdemo.c
    )
    add_test(${GR_PROJECT_NAME}_advanced_app ${GR_PROJECT_NAME}_advanced_application_hootl_test)

    add_executable(${GR_PROJECT_NAME}_basic_core_test
        Core/Test/basic_core_test.c
    )
    add_test(${GR_PROJECT_NAME}_basic_core ${GR_PROJECT_NAME}_basic_core_test)

    add_executable(${GR_PROJECT_NAME}_advanced_core_test
        Core/Test/advanced_core_test.c
        Core/Src/coredemo.c
    )
    add_test(${GR_PROJECT_NAME}_advanced_core ${GR_PROJECT_NAME}_advanced_core_test)
endif()

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})

add_library(${GR_PROJECT_NAME}_USER_CODE INTERFACE)
target_sources(${GR_PROJECT_NAME}_USER_CODE INTERFACE
    Core/Src/main.c
    Core/Src/stm32u5xx_it.c
    Core/Src/stm32u5xx_hal_msp.c
    Core/Src/system_stm32u5xx.c
    Core/Src/sysmem.c
    Core/Src/syscalls.c
)

target_link_libraries(${GR_PROJECT_NAME}_USER_CODE INTERFACE

)

target_include_directories(${GR_PROJECT_NAME}_USER_CODE INTERFACE
    Application/Inc
    Core/Inc
)
