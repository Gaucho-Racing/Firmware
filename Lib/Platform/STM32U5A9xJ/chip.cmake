set(CHIP "STM32U5A9xJ")

function(add_executable_${CHIP} TARGET_NAME)
	# Setup
	set(CHIP "STM32U5A9xJ")
	set(
		TARGET_FLAGS
		-mcpu=cortex-m33
		-mfpu=fpv4-sp-d16
		-mfloat-abi=hard
		-mthumb
	)

	add_executable(${TARGET_NAME})

	target_compile_definitions(
		${TARGET_NAME}
		PRIVATE
			USE_FULL_LL_DRIVER
			USE_HAL_DRIVER
			STM32U5A9xx
			STM32U5
	)

	target_compile_options(
		${TARGET_NAME}
		PRIVATE
			-Wno-error # TODO Remove when dev container is setup due to unfiltered warnings (U5 hal doesn't support arm gcc > 12.3)
			${TARGET_FLAGS}
	)

	target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/startup_stm32u5a9xx.s")

	target_link_options(
		${TARGET_NAME}
		PRIVATE
			${TARGET_FLAGS}
			-T
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/STM32U5A9xx_FLASH.ld"
	)

	target_link_libraries(
		${TARGET_NAME}
		PRIVATE
			STM32_HAL_LL_U5xx
			CMSIS_5_CORE
	)

	# Cleanup
	set(CHIP "YOUHAVENOTCONFIGUREDCHIPRIGHT!")
	set(TARGET_FLAGS "YOUHAVENOTCONFIGUREDTARGETFLAGSRIGHT!")
endfunction()
