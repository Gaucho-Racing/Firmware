set(CHIP "STM32L476xG") # REPLACE: with the appropriate platform name
set(CHIP_PATH "${CMAKE_SOURCE_DIR}/Lib/Platform/${CHIP}")
set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard") # REPLACE: with the appropriate linkerscript

function(add_executable_${CHIP} TARGET_NAME)

set(CHIP "STM32L476xG") # REPLACE: with the appropriate platform name
set(CHIP_PATH "${CMAKE_SOURCE_DIR}/Lib/Platform/${CHIP}")
set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard") # REPLACE: with the appropriate linkerscript
add_executable(${TARGET_NAME})
# the FLASH.ld script is a linker script that tells the linker how to arrange compiled code in the MCUs FLASH and RAM memory
set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-T\"${CHIP_PATH}/CompileDependencies/STM32L476XX_FLASH.ld\"") # REPLACE: with the appropriate linkerscript

# Cleanup (do not change, leave these 3 lines as is, trust)
set(CHIP "YOUHAVENOTCONFIGUREDCHIPRIGHT!")
set(CHIP_PATH "YOUHAVENOTCONFIGUREDCHIPPATHRIGHT!")
set(TARGET_FLAGS "YOUHAVENOTCONFIGUREDTARGETFLAGSRIGHT!")

endfunction()

add_library(${CHIP}_LIB INTERFACE)

# enable_language(C ASM)

# used to strip the last 2 characters of CHIP to make the compile def
string(SUBSTRING ${CHIP} 0 9 TEMP)  # FIXME Scary
target_compile_definitions(${CHIP}_LIB INTERFACE
	USE_FULL_LL_DRIVER
	USE_HAL_DRIVER
	${TEMP}xx # REPLACE: Check that this either works or needs to be overwritten w/ the new
    STM32L4   # REPLACE: If you get a TON of legacy warnings about things in stm32_hal_legacy.h then replace this with the 'STM32??' of the chip and uncomment
)


target_include_directories(${CHIP}_LIB INTERFACE
    ${CHIP_PATH}/Drivers/CMSIS/Include
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Inc
    ${CHIP_PATH}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
)

target_sources(${CHIP}_LIB INTERFACE
    # REPLACE: everything here with the files in the hal-driver & the startup_<platform>.s
    # you can just use 'ls -1' and use that but don't forget to comment out the templates
    ${CHIP_PATH}/CompileDependencies/startup_stm32l476xx.s
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_adc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_adc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_can.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_comp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_cortex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_crc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_crc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_cryp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_cryp_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dac.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dac_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dcmi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dfsdm.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dfsdm_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dma.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dma2d.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dma_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_dsi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_exti.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_firewall.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_flash.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_flash_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_flash_ramfunc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_gfxmmu.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_gpio.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_hash.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_hash_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_hcd.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_i2c.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_i2c_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_irda.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_iwdg.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_lcd.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_lptim.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_ltdc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_ltdc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_mmc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_mmc_ex.c
    # stm32l4xx_hal_msp_template.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_nand.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_nor.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_opamp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_opamp_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_ospi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_pcd.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_pcd_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_pka.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_pssi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_pwr.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_pwr_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_qspi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_rcc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_rcc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_rng.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_rng_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_rtc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_rtc_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_sai.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_sai_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_sd.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_sd_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_smartcard.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_smartcard_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_smbus.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_smbus_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_spi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_spi_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_sram.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_swpmi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_tim.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_tim_ex.c
    # stm32l4xx_hal_timebase_tim_template.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_tsc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_uart.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_uart_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_usart.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_usart_ex.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_hal_wwdg.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_adc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_comp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_crc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_crs.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_dac.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_dma.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_dma2d.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_exti.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_fmc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_gpio.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_i2c.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_lptim.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_lpuart.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_opamp.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_pka.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_pwr.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_rcc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_rng.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_rtc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_sdmmc.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_spi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_swpmi.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_tim.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_usart.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_usb.c
    ${CHIP_PATH}/Drivers/stm32-hal-driver/Src/stm32l4xx_ll_utils.c
)
