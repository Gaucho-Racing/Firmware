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

# TODO: make this cleaner
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Lib/Vendor/BCC ${CMAKE_CURRENT_SOURCE_DIR}/../Lib/Vendor/BCC/build)

# what, does in fact not get the filename of somthing but rather the name of the project from the path
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

add_library(${PROJECT_NAME}_USER_CODE INTERFACE)

target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unused-parameter -Wno-pedantic -Wtrigraphs -Wno-dev)

target_sources(${PROJECT_NAME}_USER_CODE INTERFACE
    # Core/Src/...
    Core/Src/adc.c
    Core/Src/dma.c
    Core/Src/fdcan.c
    Core/Src/gpio.c
    Core/Src/main.c
    Core/Src/spi.c
    Core/Src/stm32g4xx_hal_msp.c
    Core/Src/stm32g4xx_it.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Src/system_stm32g4xx.c
    Core/Src/tim.c
    Core/Src/usart.c
    Application/Src/acu.c
    Application/Src/battery.c
    Application/Src/debug.c
    Application/Src/mcu.c
    Application/Src/state.c
    # Application/Src/...
)

target_link_libraries(${PROJECT_NAME}_USER_CODE INTERFACE
    BCC
)

target_include_directories(${PROJECT_NAME}_USER_CODE INTERFACE
    Core/Inc
    Application/Inc
)
