cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(C ASM)
# STM32CubeMX generated symbols (macros)
set(MX_Defines_Syms 
	USE_HAL_DRIVER 
	STM32U599xx
    $<$<CONFIG:Debug>:DEBUG>
)

# STM32CubeMX generated include paths
set(MX_Include_Dirs
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32U5xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/CMSIS/RTOS2/Include/
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_SOURCE_DIR}/Core/Src/main.c
    ${CMAKE_SOURCE_DIR}/Core/Src/gpio.c
    ${CMAKE_SOURCE_DIR}/Core/Src/app_freertos.c
    ${CMAKE_SOURCE_DIR}/Core/Src/adc.c
    ${CMAKE_SOURCE_DIR}/Core/Src/mdf.c
    ${CMAKE_SOURCE_DIR}/Core/Src/cordic.c
    ${CMAKE_SOURCE_DIR}/Core/Src/crc.c
    ${CMAKE_SOURCE_DIR}/Core/Src/dcache.c
    ${CMAKE_SOURCE_DIR}/Core/Src/dma2d.c
    ${CMAKE_SOURCE_DIR}/Core/Src/fdcan.c
    ${CMAKE_SOURCE_DIR}/Core/Src/flash.c
    ${CMAKE_SOURCE_DIR}/Core/Src/gpu2d.c
    ${CMAKE_SOURCE_DIR}/Core/Src/gtzc.c
    ${CMAKE_SOURCE_DIR}/Core/Src/hash.c
    ${CMAKE_SOURCE_DIR}/Core/Src/i2c.c
    ${CMAKE_SOURCE_DIR}/Core/Src/icache.c
    ${CMAKE_SOURCE_DIR}/Core/Src/ltdc.c
    ${CMAKE_SOURCE_DIR}/Core/Src/memorymap.c
    ${CMAKE_SOURCE_DIR}/Core/Src/octospi.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rng.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtc.c
    ${CMAKE_SOURCE_DIR}/Core/Src/sdmmc.c
    ${CMAKE_SOURCE_DIR}/Core/Src/spi.c
    ${CMAKE_SOURCE_DIR}/Core/Src/tim.c
    ${CMAKE_SOURCE_DIR}/Core/Src/usart.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stm32u5xx_it.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stm32u5xx_hal_msp.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stm32u5xx_hal_timebase_tim.c
    ${CMAKE_SOURCE_DIR}/Core/Src/sysmem.c
    ${CMAKE_SOURCE_DIR}/Core/Src/syscalls.c
    ${CMAKE_SOURCE_DIR}/startup_stm32u599xx.s
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32u5xx.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_adc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_adc_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cortex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_exti.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gtzc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_icache.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_mdf.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cordic.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_crc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_crc_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dcache.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma2d.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_fdcan.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpu2d.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_hash.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_hash_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ltdc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ltdc_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dsi.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ospi.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dlyb.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rng.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rng_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rtc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rtc_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_sdmmc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sd.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sd_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_mmc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_mmc_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sdio.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart_ex.c
)

# Drivers Midllewares


set(RTOS2_Src
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/list.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
)

# Link directories setup
set(MX_LINK_DIRS

)
# Project static libraries
set(MX_LINK_LIBS 
    STM32_Drivers
    RTOS2	
)
# Interface library for includes and symbols
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)


# Create RTOS2 static library
add_library(RTOS2 OBJECT)
target_sources(RTOS2 PRIVATE ${RTOS2_Src})
target_link_libraries(RTOS2 PUBLIC stm32cubemx)

# Add STM32CubeMX generated application sources to the project
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
