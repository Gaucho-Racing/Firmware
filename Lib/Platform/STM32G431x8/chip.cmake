set(CHIP "STM32G431x8")

function(add_executable_${CHIP} TARGET_NAME)
	# Setup
	set(CHIP "STM32G431x8")
	set(
		TARGET_FLAGS
		-mcpu=cortex-m4
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
			STM32G431xx
			STM32G4
	)

	target_compile_options(${TARGET_NAME} PRIVATE ${TARGET_FLAGS})

	target_sources(
		${TARGET_NAME}
		PRIVATE
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/startup_stm32g431xx.s"
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/system_stm32g4xx.c"
	)

	target_link_options(
		${TARGET_NAME}
		PRIVATE
			${TARGET_FLAGS}
			-T
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/STM32G431C8Ux_FLASH.ld"
	)

	target_link_libraries(
		${TARGET_NAME}
		PRIVATE
			STM32_HAL_LL_G4xx
			CMSIS_5_CORE
	)

	# Cleanup
	set(CHIP "YOUHAVENOTCONFIGUREDCHIPRIGHT!")
	set(TARGET_FLAGS "YOUHAVENOTCONFIGUREDTARGETFLAGSRIGHT!")
endfunction()
