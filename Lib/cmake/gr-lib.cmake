function(ternary boolean first second)
    if (boolean)
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
        message(FATAL_ERROR "You called add_GR_project with an unsupported number of inputs/arguments! Do better please :)")
    endif()

    set(Platform ${ARGV0})
    set(GR_PROJECT ${ARGV1})

    # equivalent to calling the funnction add_executable_${Platform}() but cmake won't let me do that
    cmake_language(CALL add_executable_${Platform} ${GR_PROJECT})

    if(DEFINED ${GR_PROJECT_PATH})
        add_subdirectory(${GR_PROJECT_PATH})
    else()
        add_subdirectory(${GR_PROJECT})
    endif()

    add_library(Combinator_${GR_PROJECT} INTERFACE)

    target_link_libraries(Combinator_${GR_PROJECT} INTERFACE
        ${Platform}_LIB
        ${GR_PROJECT}_USER_CODE # Blame Owen
    )

    target_link_libraries(${GR_PROJECT}
        Combinator_${GR_PROJECT}
    )
endfunction()