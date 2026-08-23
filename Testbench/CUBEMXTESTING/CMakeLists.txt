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
    -T "${CMAKE_CURRENT_SOURCE_DIR}/STM32G474xx_FLASH.ld"
)

target_compile_definitions(
    ${TARGET_NAME}
    PRIVATE
    STM32G474xx
)

target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    Core/Inc
    Application/Inc
)

target_sources(
    ${TARGET_NAME}
    PRIVATE
    Application/Src/CANdler.c
    Application/Src/Loop.c
    Core/Src/fdcan.c
    Core/Src/gpio.c
    Core/Src/main.c
    Core/Src/stm32g4xx_it.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Src/system_stm32g4xx.c
    Core/Src/tim.c
    Core/Src/usart.c
    startup_stm32g474xx.s
)

target_link_libraries(
    ${TARGET_NAME}
    PRIVATE
    CUBE_CAN_LIB
    CUBE_VCP_LIB
    LOGOMATIC_LIB
)
