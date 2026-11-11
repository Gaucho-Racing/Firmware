#[[
Required components (orderless)
- Link options for the GCC linker script
- Compile definitions for the chip subfamily
- Includes for the project
- Sources for the project
- Additional link libraries for the project
]]

target_link_options(
    ${TARGET_NAME}
    PRIVATE
    -T
    "${CMAKE_CURRENT_SOURCE_DIR}/STM32G431xx_FLASH.ld"
)

target_compile_definitions(${TARGET_NAME} PRIVATE STM32G431xx)

target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    Application/Inc
    Core/Inc
)

target_sources(
    ${TARGET_NAME}
    PRIVATE
    Core/Src/gpio.c
    Core/Src/main.c
    Core/Src/stm32g4xx_hal_msp.c
    Core/Src/stm32g4xx_it.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Src/system_stm32g4xx.c
    startup_stm32g431xx.s
)

target_link_libraries(
    ${TARGET_NAME}
    PRIVATE
    LOGOMATIC_LIB
    GLVPD_A_COMMON_LIB
)
