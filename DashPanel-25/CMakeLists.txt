cmake_minimum_required(VERSION 3.25)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME dashPanel25)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(LV_CONF_BUILD_DISABLE_DEMOS 1)
set(LV_CONF_BUILD_DISABLE_EXAMPLES 1)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(Middlewares/Third_Party/LVGL/lvgl)
add_subdirectory(cmake/stm32cubemx)

# The ordering is bad sorry
target_include_directories(lvgl PUBLIC
    Drivers/CMSIS/Device/ST/STM32U5xx/Include
    Drivers/CMSIS/Include
    Middlewares/Third_Party/LVGL
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/CMSIS/RTOS2/Include
    Core/Inc
    Drivers/STM32U5xx_HAL_Driver/Inc
    Drivers/STM32U5xx_HAL_Driver/Inc/Legacy
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/lvgl_port_display.c
    Core/Src/mx25lm51245g.c
    Application/Src/CANdler.c
    Application/Src/dash.c
    Application/Src/gui.c
    Application/Src/utils.c
    Application/Src/buttons.c

    Application/Src/Fonts/lv_font_gr_bold_16.c
    Application/Src/Fonts/lv_font_gr_bold_24.c
    Application/Src/Fonts/lv_font_gr_bold_32.c
    Application/Src/Fonts/lv_font_gr_bold_40.c
    Application/Src/Fonts/lv_font_gr_regular_16.c
    Application/Src/Fonts/lv_font_gr_regular_24.c
    Application/Src/Fonts/lv_font_gr_regular_32.c
    Application/Src/Fonts/lv_font_gr_regular_40.c
    Application/Src/Fonts/lv_font_gr_light_16.c
    Application/Src/Fonts/lv_font_gr_light_24.c
    Application/Src/Fonts/lv_font_gr_light_32.c
    Application/Src/Fonts/lv_font_gr_light_40.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Middlewares/Third_Party/LVGL
    Application/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    lvgl
    # lvgl_demos
)

# Setup debug to enable logomatic and disable background
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-variable")
    add_compile_definitions(LOGOMATIC_ENABLED)
#   add_compile_definitions(ADVANCED_LOGGING)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -u _printf_float -O1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float")
else()
    add_compile_definitions(USE_NICE_BACKGROUND)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE Application/Src/Images/dashbg.c)
endif()

# Add CSV-25 integration
add_dependencies(${CMAKE_PROJECT_NAME} executeCSV25)
add_custom_target(
    executeCSV25
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/CSV-25/executeCSV.sh
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/grCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/GR-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/msgCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/MSG-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/customIdCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/CUSTOM-ID.csv
    COMMENT "Updating GR ID and MSG ID header files from CSVs"
    USES_TERMINAL
)
