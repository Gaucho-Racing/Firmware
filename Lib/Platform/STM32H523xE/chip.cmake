set(CHIP "STM32H523xE")

function(add_executable_${CHIP} TARGET_NAME)
	# Setup
	set(CHIP "STM32H523xE")
	set(
		TARGET_FLAGS
		-mcpu=cortex-m33
		-mfpu=fpv5-sp-d16
		-mfloat-abi=hard
		-mthumb
	)

	add_executable(${TARGET_NAME})

	target_compile_definitions(
		${TARGET_NAME}
		PRIVATE
			USE_FULL_LL_DRIVER
			USE_HAL_DRIVER
			STM32H523xx
			STM32H5
	)

	target_compile_options(${TARGET_NAME} PRIVATE ${TARGET_FLAGS})

	target_link_options(
		${TARGET_NAME}
		PRIVATE
			${TARGET_FLAGS}
			-T
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/STM32H523CEUx_FLASH.ld"
	)

	target_sources(
		${TARGET_NAME}
		PRIVATE
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/startup_stm32h523xx.s"
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/system_stm32h5xx.c"
	)

	target_link_libraries(
		${TARGET_NAME}
		PRIVATE
			STM32_HAL_LL_H5xx
			CMSIS_5_CORE
	)

	# Cleanup
	set(CHIP "YOUHAVENOTCONFIGUREDCHIPRIGHT!")
	set(TARGET_FLAGS "YOUHAVENOTCONFIGUREDTARGETFLAGSRIGHT!")
endfunction()
