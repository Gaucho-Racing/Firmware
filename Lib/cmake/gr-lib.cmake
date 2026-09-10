function(add_gr_project)
	# overload add_gr_project
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

	if(CMAKE_PRESET_NAME STREQUAL "HOOTLTest")
		if(DEFINED GR_PROJECT_PATH)
			add_subdirectory("${GR_PROJECT}/${GR_PROJECT_PATH}")
		else()
			add_subdirectory(${GR_PROJECT})
		endif()
	else()
		if(DEFINED GR_PROJECT_PATH)
			set(TARGET_NAME "${GR_PROJECT_PATH}")
			add_subdirectory("${GR_PROJECT}/${GR_PROJECT_PATH}")
		else()
			set(TARGET_NAME "${GR_PROJECT}")
			add_subdirectory(${GR_PROJECT})
		endif()

		cmake_language(CALL add_executable_${Platform} ${TARGET_NAME})

		target_link_libraries(
			${TARGET_NAME}
			PRIVATE
				${TARGET_NAME}_USER_CODE # Blame Owen
				GLOBALSHARE_LIB
				m
		)

		target_link_options(
			${TARGET_NAME}
			PRIVATE
				"LINKER:-Map=$<TARGET_FILE_DIR:${TARGET_NAME}>/$<TARGET_FILE_BASE_NAME:${TARGET_NAME}>.map"
		)

		if(DEFINED NODE)
			target_compile_definitions(${TARGET_NAME} PRIVATE NODE=${NODE})
		endif()
	endif()
endfunction()
