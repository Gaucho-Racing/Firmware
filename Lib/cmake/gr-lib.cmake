function(ternary boolean first second)
	if(boolean)
		return(first)
	else()
		return(second)
	endif()
endfunction()

function(add_GR_project)
	# overload add_GR_project
	if(${ARGC} EQUAL 2)
	elseif(${ARGC} EQUAL 3)
		set(GR_PROJECT_PATH ${ARGV2})
	else()
		message(
			FATAL_ERROR
			"You called add_GR_project with an unsupported number of inputs/arguments! Do better please :)"
		)
	endif()

	set(Platform ${ARGV0})
	set(GR_PROJECT ${ARGV1})

	if(CMAKE_BUILD_TYPE STREQUAL "Test")
		if(DEFINED GR_PROJECT_PATH)
			add_subdirectory("${GR_PROJECT}/${GR_PROJECT_PATH}")
		else()
			add_subdirectory(${GR_PROJECT})
		endif()
	else()
		if(DEFINED GR_PROJECT_PATH)
			set(TARGET_NAME "${GR_PROJECT_PATH}")
			add_subdirectory("${GR_PROJECT}/${GR_PROJECT_PATH}")
			set(COMBINATOR "${GR_PROJECT}_${GR_PROJECT_PATH}")
		else()
			set(TARGET_NAME "${GR_PROJECT}")
			add_subdirectory(${GR_PROJECT})
			set(COMBINATOR "${GR_PROJECT}")
		endif()

		cmake_language(
			CALL
				add_executable_${Platform}
				${TARGET_NAME}
		)

		# Create unique interface library to avoid conflicts
		set(INTERFACE_LIB "${COMBINATOR}_INTERFACE_LIB")

		if(NOT TARGET ${INTERFACE_LIB})
			add_library(${INTERFACE_LIB} INTERFACE)

			target_link_libraries(
				${INTERFACE_LIB}
				INTERFACE
					${Platform}_LIB
					${TARGET_NAME}_USER_CODE # Blame Owen
					GLOBALSHARE_LIB
			)
		endif()

		target_link_libraries(${TARGET_NAME} PRIVATE ${INTERFACE_LIB})
	endif()
endfunction()
