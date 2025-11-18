set(CHIP "STM32G474xE")	# FIXME Replace with real chip name

function(add_executable_${CHIP} TARGET_NAME)
	# Setup
	set(CHIP "STM32G474xE")	# FIXME Replace with real chip name
	set(
		TARGET_FLAGS
		# Replace with real target flags
	)

	add_executable(${TARGET_NAME})

	target_compile_definitions(
		${TARGET_NAME}
		PRIVATE
			USE_FULL_LL_DRIVER
			USE_HAL_DRIVER
			# STM32G474xx	# FIXME Replace with real chip series designation
			# STM32G4		# FIXME Replace with real series designation
	)

	target_compile_options(${TARGET_NAME} PRIVATE ${TARGET_FLAGS})

	target_link_options(
		${TARGET_NAME}
		PRIVATE
			${TARGET_FLAGS}
			-T
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/FLASH_LINKER_SCRIPT.ld"	# FIXME Replace with real linker script
	)

	target_sources(
		${TARGET_NAME}
		PRIVATE
			"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/STARTUP_ASSEMBLY_SCRIPT.s" # FIXME Replace with real startup assembly script
	)

	target_link_libraries(${TARGET_NAME} PRIVATE STM32_HAL_LL_G4xx CMSIS_5_CORE)	# FIXME Replace with real HAL/LL library

	# Cleanup
	set(CHIP "YOUHAVENOTCONFIGUREDCHIPRIGHT!")
	set(TARGET_FLAGS "YOUHAVENOTCONFIGUREDTARGETFLAGSRIGHT!")
endfunction()
