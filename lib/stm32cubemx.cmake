# STM32CubeMX Shared Library Functions
# Provides common functionality for STM32CubeMX-generated targets

# Helper function to add source files only if they exist
function(add_sources_if_exist TARGET_NAME)
    foreach(SOURCE_FILE ${ARGN})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
            target_sources(${TARGET_NAME} INTERFACE ${SOURCE_FILE})
        endif()
    endforeach()
endfunction()

# Function to create STM32CubeMX library for STM32U5 targets
function(create_stm32u5_cubemx_target TARGET_NAME)
    # Create the interface library
    add_library(${TARGET_NAME} INTERFACE)
    
    # STM32U5 compile definitions
    target_compile_definitions(${TARGET_NAME} INTERFACE 
        USE_HAL_DRIVER 
        STM32U599xx
        $<$<CONFIG:Debug>:DEBUG>
    )
    
    # STM32U5 include directories (relative to board root)
    target_include_directories(${TARGET_NAME} INTERFACE
        Core/Inc
        Drivers/STM32U5xx_HAL_Driver/Inc
        Drivers/STM32U5xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32U5xx/Include
        Drivers/CMSIS/Include
        Middlewares/Third_Party/FreeRTOS/Source/include/
        Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/
        Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/
        Middlewares/Third_Party/CMSIS/RTOS2/Include/
    )
    
    # Core STM32U5 sources - only add if they exist
    add_sources_if_exist(${TARGET_NAME}
        Core/Src/main.c
        Core/Src/gpio.c
        Core/Src/app_freertos.c
        Core/Src/mdf.c
        Core/Src/cordic.c
        Core/Src/crc.c
        Core/Src/dcache.c
        Core/Src/dma2d.c
        Core/Src/fdcan.c
        Core/Src/flash.c
        Core/Src/gpu2d.c
        Core/Src/gtzc.c
        Core/Src/hash.c
        Core/Src/icache.c
        Core/Src/ltdc.c
        Core/Src/memorymap.c
        Core/Src/octospi.c
        Core/Src/rng.c
        Core/Src/rtc.c
        Core/Src/sdmmc.c
        Core/Src/spi.c
        Core/Src/tim.c
        Core/Src/stm32u5xx_it.c
        Core/Src/stm32u5xx_hal_msp.c
        Core/Src/stm32u5xx_hal_timebase_tim.c
        Core/Src/sysmem.c
        Core/Src/syscalls.c
        startup_stm32u599xx.s
    )
    
    # STM32U5 HAL/LL Driver sources - only add if they exist
    add_sources_if_exist(${TARGET_NAME}
        Core/Src/system_stm32u5xx.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_exti.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cortex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_usart.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_usart_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cordic.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_crc.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_crc_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dcache.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma2d.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_fdcan.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpu2d.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gtzc.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_hash.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_hash_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_icache.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ltdc.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ltdc_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_mdf.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ospi.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rng.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rng_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rtc.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rtc_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sd.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sd_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_sdmmc.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dlyb.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_adc.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_adc_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_adc.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c_ex.c
        Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dsi.c
    )
    
    # FreeRTOS sources - only add if they exist
    add_sources_if_exist(${TARGET_NAME}
        Middlewares/Third_Party/FreeRTOS/Source/croutine.c
        Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
        Middlewares/Third_Party/FreeRTOS/Source/list.c
        Middlewares/Third_Party/FreeRTOS/Source/queue.c
        Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
        Middlewares/Third_Party/FreeRTOS/Source/tasks.c
        Middlewares/Third_Party/FreeRTOS/Source/timers.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
        Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    )
endfunction()

# Function to create STM32CubeMX library for STM32G4 targets  
function(create_stm32g4_cubemx_target TARGET_NAME)
    # Create the interface library
    add_library(${TARGET_NAME} INTERFACE)
    
    # STM32G4 compile definitions
    target_compile_definitions(${TARGET_NAME} INTERFACE 
        USE_HAL_DRIVER 
        STM32G474xx
        $<$<CONFIG:Debug>:DEBUG>
    )
    
    # STM32G4 include directories (relative to board root)
    target_include_directories(${TARGET_NAME} INTERFACE
        Core/Inc
        Drivers/STM32G4xx_HAL_Driver/Inc
        Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32G4xx/Include
        Drivers/CMSIS/Include
    )
    
    # Core STM32G4 sources - only add if they exist
    add_sources_if_exist(${TARGET_NAME}
        Core/Src/main.c
        Core/Src/gpio.c
        Core/Src/adc.c
        Core/Src/dma.c
        Core/Src/fdcan.c
        Core/Src/usart.c
        Core/Src/stm32g4xx_it.c
        Core/Src/stm32g4xx_hal_msp.c
        Core/Src/sysmem.c
        Core/Src/syscalls.c
        startup_stm32g474xx.s
    )
    
    # STM32G4 HAL/LL Driver sources - only add if they exist
    add_sources_if_exist(${TARGET_NAME}
        Core/Src/system_stm32g4xx.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc_ex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_adc.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_usart.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_usart_ex.c
        Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_fdcan.c
    )
endfunction()

# Function to add board-specific sources to an existing STM32CubeMX target
function(add_board_specific_sources TARGET_NAME)
    # Parse additional arguments as source files and add only if they exist
    add_sources_if_exist(${TARGET_NAME} ${ARGN})
endfunction()
