set(CHIP "STM32U5A9xE")
set(CHIP_PATH "${CMAKE_SOURCE_DIR}/Lib/Platform/${CHIP}")

set(TARGET_FLAGS "-mcpu=cortex-m33 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

add_library(${CHIP}_LIB INTERFACE)

# enable_language(C ASM)

string(SUBSTRING ${CHIP} 0 9 TEMP)  # Scary
target_compile_definitions(${CHIP}_LIB INTERFACE 
	USE_FULL_LL_DRIVER
	USE_HAL_DRIVER
	${TEMP}xx
)

target_include_directories(${CHIP}_LIB INTERFACE
    ${CHIP_PATH}/Drivers/CMSIS/Include
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Inc
    ${CHIP_PATH}/Drivers/CMSIS/Device/ST/STM32U5xx/Include
)

target_sources(${CHIP}_LIB INTERFACE
    ${CHIP_PATH}/CompileDependencies/startup_stm32g474xx.s
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_adc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_adc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_comp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_cordic.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_cortex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_crc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_crc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_cryp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_cryp_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_dac.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_dac_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_dma.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_dma_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_exti.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_fdcan.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_flash.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_flash_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_flash_ramfunc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_fmac.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_gpio.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_hrtim.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_i2c.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_i2c_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_i2s.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_irda.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_iwdg.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_lptim.c
    # ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_msp_template.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_nand.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_nor.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_opamp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_opamp_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_pcd.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_pcd_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_pwr.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_pwr_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_qspi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_rcc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_rcc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_rng.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_rtc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_rtc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_sai.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_sai_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_smartcard.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_smartcard_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_smbus.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_smbus_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_spi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_spi_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_sram.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_tim.c
    # ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_timebase_tim_template.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_tim_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_uart.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_uart_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_usart.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_usart_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_hal_wwdg.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_adc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_comp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_cordic.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_crc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_crs.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_dac.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_dma.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_exti.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_fmac.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_fmc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_gpio.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_hrtim.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_i2c.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_lptim.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_lpuart.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_opamp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_pwr.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_rcc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_rng.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_rtc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_spi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_tim.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_ucpd.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_usart.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_usb.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32g4xx_ll_utils.c
)

function(add_executable_${CHIP} TARGET_NAME)

set(TARGET_FLAGS "-mcpu=cortex-m33 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

add_executable(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-T${CHIP_PATH}/CompileDependencies/STM32U599xx_FLASH_FLASH.ld")

endfunction()
