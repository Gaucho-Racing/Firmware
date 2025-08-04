cmake_minimum_required(VERSION 3.25)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})

# what, does in fact not get the filename of somthing but rather the name of the project from the path
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

add_library(${PROJECT_NAME}_USER_CODE INTERFACE)
target_sources(${PROJECT_NAME}_USER_CODE INTERFACE
    # Core/Src/...
    # Application/Src/...
)

target_link_libraries(${PROJECT_NAME}_USER_CODE INTERFACE

)

target_include_directories(${PROJECT_NAME}_USER_CODE INTERFACE
    Core/Inc
    Application/Inc
)
