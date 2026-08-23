cmake_minimum_required(VERSION 3.25)
include(FetchContent)

#[[
@brief Downloads a git repository and checks out a specific tag, using sparse checkout to only fetch specific folders
@param NAME The name of the git repository (used for FetchContent)
@param GIT_REPOSITORY The URL of the git repository to download
@param GIT_TAG The git tag to checkout
@param OUTPUT_PATH_VAR The variable to set with the path to the downloaded repository
@param TARGET_FOLDER The folder(s) to fetch from the repository (sparse checkout)
@note This function uses FetchContent to download the repository and sparse checkout to only fetch the specified folder(s)
@remark Prefer calling add_project() instead of this function directly
]]
function(download_git)
	if(NOT ${ARGC} EQUAL 10)
		message(
			FATAL_ERROR
			"You called download_git with an unsupported number of inputs/arguments! Do better please :)"
		)
	endif()

	set(
		one_value_args
		NAME
		GIT_REPOSITORY
		GIT_TAG
		OUTPUT_PATH_VAR
	)
	set(multi_value_args TARGET_FOLDER)
	unset(DOWNLOAD_GIT_ARG_NAME)
	unset(DOWNLOAD_GIT_ARG_GIT_REPOSITORY)
	unset(DOWNLOAD_GIT_ARG_GIT_TAG)
	unset(DOWNLOAD_GIT_ARG_OUTPUT_PATH_VAR)
	unset(DOWNLOAD_GIT_ARG_TARGET_FOLDER)
	cmake_parse_arguments(
		"DOWNLOAD_GIT_ARG"
		""
		"${one_value_args}"
		"${multi_value_args}"
		${ARGN}
	)

	message(
		STATUS
		"This may take a moment, fetching content for ${DOWNLOAD_GIT_ARG_NAME} from ${DOWNLOAD_GIT_ARG_GIT_REPOSITORY} at tag ${DOWNLOAD_GIT_ARG_GIT_TAG}"
	)

	FetchContent_Declare(
		"${DOWNLOAD_GIT_ARG_NAME}"
		GIT_REPOSITORY "${DOWNLOAD_GIT_ARG_GIT_REPOSITORY}"
		GIT_TAG "${DOWNLOAD_GIT_ARG_GIT_TAG}"
		GIT_SHALLOW ON
		GIT_SUBMODULES
			""
		UPDATE_DISCONNECTED TRUE
		GIT_CONFIG
			"core.sparseCheckout=true"
	)

	string(TOLOWER "${DOWNLOAD_GIT_ARG_NAME}" NAME_LOWER)

	FetchContent_GetProperties(${NAME_LOWER})

	if(NOT ${NAME_LOWER}_POPULATED)
		FetchContent_Populate(${NAME_LOWER})
		separate_arguments(
			TARGET_FOLDER_LIST
			NATIVE_COMMAND
			"${DOWNLOAD_GIT_ARG_TARGET_FOLDER}"
		)
		execute_process(
			COMMAND
				git sparse-checkout set ${TARGET_FOLDER_LIST}
			WORKING_DIRECTORY "${${NAME_LOWER}_SOURCE_DIR}"
		)
		execute_process(
			COMMAND
				git checkout
			WORKING_DIRECTORY "${${NAME_LOWER}_SOURCE_DIR}"
		)
	endif()

	message(
		STATUS
		"Successfully fetched content for ${DOWNLOAD_GIT_ARG_NAME} from ${DOWNLOAD_GIT_ARG_GIT_REPOSITORY} at tag ${DOWNLOAD_GIT_ARG_GIT_TAG} to ${${DOWNLOAD_GIT_ARG_NAME}_SOURCE_DIR}"
	)

	set(
		"${DOWNLOAD_GIT_ARG_OUTPUT_PATH_VAR}"
		"${${DOWNLOAD_GIT_ARG_NAME}_SOURCE_DIR}"
	)
	return(PROPAGATE "${DOWNLOAD_GIT_ARG_OUTPUT_PATH_VAR}")
endfunction()

#[[
@brief Adds the ARM CMSIS core to the build system
@param MAJOR The major version of the CMSIS core to use (e.g., "5")
@param GIT_TAG The git tag for the CMSIS core to use
@note Link your user code against "CMSIS_${MAJOR}_Core" to ensure proper linking with the CMSIS core libraries
@remark Prefer calling add_project() instead of this function directly
]]
function(add_arm_cmsis_core)
	if(NOT ${ARGC} EQUAL 4)
		message(
			FATAL_ERROR
			"You called add_arm_cmsis_core with an unsupported number of inputs/arguments! Do better please :)"
		)
	endif()

	set(
		one_value_args
		MAJOR
		GIT_TAG
	)
	unset(ARM_CMSIS_CORE_ARG_MAJOR)
	unset(ARM_CMSIS_CORE_ARG_GIT_TAG)
	cmake_parse_arguments("ARM_CMSIS_CORE_ARG" "" "${one_value_args}" "" ${ARGN})

	if(NOT ARM_CMSIS_CORE_ARG_MAJOR MATCHES "^[0-9]$")
		message(
			FATAL_ERROR
			"MAJOR '${ARM_CMSIS_CORE_ARG_MAJOR}' is not a valid version number (ie '5')"
		)
	endif()

	if(TARGET CMSIS_${ARM_CMSIS_CORE_ARG_MAJOR}_Core)
		return()
	endif()

	download_git(
	        NAME "arm_cmsis_${ARM_CMSIS_CORE_ARG_MAJOR}"
	        GIT_REPOSITORY "https://github.com/arm-software/cmsis_${ARM_CMSIS_CORE_ARG_MAJOR}.git"
	        GIT_TAG "${ARM_CMSIS_CORE_ARG_GIT_TAG}"
	        OUTPUT_PATH_VAR ARM_CMSIS_CORE_PATH
	        TARGET_FOLDER "CMSIS/Core/Include"
	)
	message(
		STATUS
		"Successfully fetched content for ARM CMSIS core version ${ARM_CMSIS_CORE_ARG_MAJOR} from ${ARM_CMSIS_CORE_ARG_GIT_TAG} to ${ARM_CMSIS_CORE_PATH}"
	)
	add_library(CMSIS_${ARM_CMSIS_CORE_ARG_MAJOR}_Core INTERFACE)
	target_include_directories(
		CMSIS_${ARM_CMSIS_CORE_ARG_MAJOR}_Core
		INTERFACE
			$<BUILD_INTERFACE:${ARM_CMSIS_CORE_PATH}/CMSIS/Core/Include>
	)
endfunction()

#[[
@brief Adds the ARM CMSIS device interface to the build system
@param SERIES The STM32 series (e.g., "g4")
@param SPECIFIER The STM32 specifier (e.g., "g474re")
@param GIT_TAG The git tag for the CMSIS device to use
@note Link your user code against "CMSIS_Device_${SPECIFIER}" to ensure proper linking with the CMSIS device libraries
@remark Prefer calling add_project() instead of this function directly
]]
function(add_arm_cmsis_interface)
	if(NOT ${ARGC} EQUAL 6)
		message(
			FATAL_ERROR
			"You called add_arm_cmsis_interface with an unsupported number of inputs/arguments! Do better please :)"
		)
	endif()

	set(
		one_value_args
		SERIES
		SPECIFIER
		GIT_TAG
	)
	unset(ARM_CMSIS_INTERFACE_ARG_SERIES)
	unset(ARM_CMSIS_INTERFACE_ARG_SPECIFIER)
	unset(ARM_CMSIS_INTERFACE_ARG_GIT_TAG)
	cmake_parse_arguments(
		"ARM_CMSIS_INTERFACE_ARG"
		""
		"${one_value_args}"
		""
		${ARGN}
	)

	string(LENGTH "${ARM_CMSIS_INTERFACE_ARG_SERIES}" SERIES_LENGTH)
	if(NOT SERIES_LENGTH EQUAL 2)
		message(
			FATAL_ERROR
			"SERIES '${ARM_CMSIS_INTERFACE_ARG_SERIES}' must be 2 characters long (ie 'g4')"
		)
	endif()
	string(TOLOWER "${ARM_CMSIS_INTERFACE_ARG_SERIES}" SERIES_LOWER)

	string(LENGTH "${ARM_CMSIS_INTERFACE_ARG_SPECIFIER}" SPECIFIER_LENGTH)
	if(SPECIFIER_LENGTH LESS 6)
		message(
			FATAL_ERROR
			"SPECIFIER '${ARM_CMSIS_INTERFACE_ARG_SPECIFIER}' must be at least 6 characters long (ie 'g474re' or 'g474ret6')"
		)
	endif()
	string(TOLOWER "${ARM_CMSIS_INTERFACE_ARG_SPECIFIER}" SPECIFIER_LOWER)
	string(TOUPPER "${ARM_CMSIS_INTERFACE_ARG_SPECIFIER}" SPECIFIER_UPPER)

	string(
		FIND "${ARM_CMSIS_INTERFACE_ARG_SPECIFIER}"
		"${ARM_CMSIS_INTERFACE_ARG_SERIES}"
		SERIES_POSITION_IN_SPECIFIER
	)
	if(NOT SERIES_POSITION_IN_SPECIFIER EQUAL 0)
		message(
			FATAL_ERROR
			"SPECIFIER '${ARM_CMSIS_INTERFACE_ARG_SPECIFIER}' must start with SERIES '${ARM_CMSIS_INTERFACE_ARG_SERIES}' at the beginning (eg 'g4' and 'g474ret6')"
		)
	endif()

	if(TARGET CMSIS_Device_${SPECIFIER_UPPER})
		return()
	endif()

	download_git(
	        NAME "cmsis_device_${SERIES_LOWER}"
	        GIT_REPOSITORY "https://github.com/STMicroelectronics/cmsis-device-${SERIES_LOWER}.git"
	        GIT_TAG ${ARM_CMSIS_INTERFACE_ARG_GIT_TAG}
	        TARGET_FOLDER "Include"
	        OUTPUT_PATH_VAR CMSIS_DEVICE_PATH
	)
	message(
		STATUS
		"Successfully fetched content for ARM CMSIS device ${SPECIFIER_UPPER} from ${ARM_CMSIS_INTERFACE_ARG_GIT_TAG} to ${CMSIS_DEVICE_PATH}"
	)
	add_library("CMSIS_Device_${SPECIFIER_UPPER}" INTERFACE)
	target_include_directories(
		"CMSIS_Device_${SPECIFIER_UPPER}"
		INTERFACE
			$<BUILD_INTERFACE:${CMSIS_DEVICE_PATH}/Include>
	)
	target_compile_definitions(
		"CMSIS_Device_${SPECIFIER_UPPER}"
		INTERFACE
			"STM32${SERIES_UPPER}xx"
			"USE_HAL_DRIVER"
	)
endfunction()

#[[
@brief Adds the STM32 HAL driver interface to the build system
@param SERIES The STM32 series (e.g., "g4")
@param GIT_TAG The git tag for the STM32 HAL driver to use
@note Link your user code against "STM32HAL_${SERIES}" to ensure proper linking with the STM32 HAL libraries
@remark Prefer calling add_project() instead of this function directly
]]
function(add_stm32_hal_interface)
	if(NOT ${ARGC} EQUAL 4)
		message(
			FATAL_ERROR
			"You called add_stm32_hal_interface with an unsupported number of inputs/arguments! Do better please :)"
		)
	endif()

	set(
		one_value_args
		SERIES
		GIT_TAG
	)
	cmake_parse_arguments("STM32_HAL_ARG" "" "${one_value_args}" "" ${ARGN})

	message(
		VERBOSE
		"Adding STM32 HAL driver interface for SERIES '${STM32_HAL_ARG_SERIES}' with GIT_TAG '${STM32_HAL_ARG_GIT_TAG}'"
	)

	string(LENGTH "${STM32_HAL_ARG_SERIES}" SERIES_LENGTH)
	if(NOT SERIES_LENGTH EQUAL 2)
		message(
			FATAL_ERROR
			"SERIES '${STM32_HAL_ARG_SERIES}' must be 2 characters long (ie 'g4')"
		)
	endif()

	string(TOLOWER "${STM32_HAL_ARG_SERIES}" SERIES_LOWER)
	string(TOUPPER "${STM32_HAL_ARG_SERIES}" SERIES_UPPER)

	if(TARGET STM32HAL_${SERIES_UPPER})
		return()
	endif()

	download_git(
	        NAME "stm32_hal_driver_${SERIES_LOWER}"
	        GIT_REPOSITORY "https://github.com/STMicroelectronics/stm32${SERIES_LOWER}xx-hal-driver.git"
	        GIT_TAG ${STM32_HAL_ARG_GIT_TAG}
	        TARGET_FOLDER "Inc Src"
	        OUTPUT_PATH_VAR STM32_HAL_DRIVER_PATH
	)
	message(
		STATUS
		"Successfully fetched content for STM32 HAL driver ${SERIES_UPPER} from ${STM32_HAL_ARG_GIT_TAG} to ${STM32_HAL_DRIVER_PATH}"
	)
	add_library("STM32HAL_${SERIES_UPPER}" INTERFACE)

	target_include_directories(
		"STM32HAL_${SERIES_UPPER}"
		INTERFACE
			$<BUILD_INTERFACE:${STM32_HAL_DRIVER_PATH}/Inc>
			$<BUILD_INTERFACE:${STM32_HAL_DRIVER_PATH}/Inc/Legacy>
	)

	file(
		GLOB_RECURSE HAL_SOURCES
		CONFIGURE_DEPENDS
		"${STM32_HAL_DRIVER_PATH}/Src/*.c"
	)
	list(FILTER HAL_SOURCES EXCLUDE REGEX ".*_template\.c$")
	target_sources("STM32HAL_${SERIES_UPPER}" INTERFACE ${HAL_SOURCES})
endfunction()

#[[
@brief Adds a project to the build system
@param NAME The name of the project
@param PATH The location of the project relative to the CMakeLists.txt file
@param SERIES The STM32 series (e.g., "g4")
@param SPECIFIER The STM32 specifier (e.g., "g474re")
@param HAL_GIT_TAG The git tag for the STM32 HAL driver to use
@param CMSIS_GIT_TAG The git tag for the CMSIS device to use
@param CMSIS_MAJOR The major version of the CMSIS device to use
@note Link your user code against "${NAME}_USER_CODE" to ensure proper linking with the STM32 HAL and CMSIS libraries
]]
function(add_project)
	if(NOT ${ARGC} EQUAL 14)
		message(
			FATAL_ERROR
			"You called add_project with an unsupported number of inputs/arguments! Do better please :)"
		)
	endif()

	set(
		one_value_args
		NAME
		PATH
		SERIES
		SPECIFIER
		HAL_GIT_TAG
		CMSIS_GIT_TAG
		CMSIS_MAJOR
	)
	cmake_parse_arguments("ARG" "" "${one_value_args}" "" ${ARGN})
	string(TOUPPER "${ARG_SERIES}" SERIES_UPPER)
	string(TOLOWER "${ARG_SERIES}" SERIES_LOWER)
	string(TOUPPER "${ARG_SPECIFIER}" SPECIFIER_UPPER)

	message(
		DEBUG
		"Adding project '${ARG_NAME}' at path '${ARG_PATH}' with SERIES '${ARG_SERIES}', SPECIFIER '${ARG_SPECIFIER}', HAL_GIT_TAG '${ARG_HAL_GIT_TAG}', CMSIS_GIT_TAG '${ARG_CMSIS_GIT_TAG}', and CMSIS_MAJOR '${ARG_CMSIS_MAJOR}'"
	)

	add_arm_cmsis_interface(
	        SERIES ${ARG_SERIES}
	        SPECIFIER ${ARG_SPECIFIER}
	        GIT_TAG ${ARG_CMSIS_GIT_TAG}
	)

	add_stm32_hal_interface(
	        SERIES ${ARG_SERIES}
	        GIT_TAG ${ARG_HAL_GIT_TAG}
	)

	if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
		# TODO Later, out of scope for now
	else()
		set(TARGET_NAME "${ARG_NAME}")
		message(STATUS "Adding project '${TARGET_NAME}'")
		add_executable("${TARGET_NAME}")

		include(
			"${PROJECT_SOURCE_DIR}/Lib/Vendor/TargetFlags/stm32${SERIES_LOWER}xx.cmake"
		)

		target_link_libraries(
			"${TARGET_NAME}"
			PRIVATE
				"CMSIS_Device_${SPECIFIER_UPPER}"
				"CMSIS_${ARG_CMSIS_MAJOR}_Core"
				"STM32HAL_${SERIES_UPPER}"
				GLOBALSHARE_LIB
				m
		)

		target_link_options(
			"${TARGET_NAME}"
			PRIVATE
				"LINKER:-Map=$<TARGET_FILE_DIR:${TARGET_NAME}>/$<TARGET_FILE_BASE_NAME:${TARGET_NAME}>.map"
		)

		target_compile_definitions(
			"${TARGET_NAME}"
			PRIVATE
				USE_HAL_DRIVER
				USE_FULL_LL_DRIVER
				STM32${SERIES_UPPER}xx
		)
	endif()

	if(DEFINED NODE)
		target_compile_definitions("${ARG_NAME}" PRIVATE NODE=${NODE})
	endif()

	if(IS_ABSOLUTE "${ARG_PATH}")
		add_subdirectory("${ARG_PATH}")
	else()
		add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${ARG_PATH}")
	endif()
endfunction()
