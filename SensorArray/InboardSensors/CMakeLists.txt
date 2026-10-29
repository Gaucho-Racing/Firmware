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
		"${CMAKE_CURRENT_SOURCE_DIR}/STM32H503xx_FLASH.ld"
)

target_compile_definitions(${TARGET_NAME} PRIVATE STM32H503xx)

target_include_directories(
	${TARGET_NAME}
	PRIVATE
		Core/Inc
		Application/Inc
)

target_sources(
	${TARGET_NAME}
	PRIVATE
		Core/Src/fdcan.c
		Core/Src/gpio.c
		Core/Src/i2c.c
		Core/Src/icache.c
		Core/Src/main.c
		Core/Src/stm32h5xx_hal_msp.c
		Core/Src/stm32h5xx_it.c
		Core/Src/syscalls.c
		Core/Src/sysmem.c
		Core/Src/system_stm32h5xx.c
		startup_stm32h503xx.s
)

target_link_libraries(${TARGET_NAME} PRIVATE LOGOMATIC_LIB)
