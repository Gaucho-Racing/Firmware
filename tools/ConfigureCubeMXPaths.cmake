# ConfigureCubeMXPaths.cmake
#
# Cross-platform STM32CubeMX template path configuration.
#
# Expected repository layout:
#
# Firmware/
# ├── tools/
# │   └── ConfigureCubeMXPaths.cmake
# ├── CUBEMXTESTING/
# │   └── CUBEMXTESTING.ioc
# └── Lib/
#     └── CubeMXTemplates/
#         └── Autogen/
#             └── Inc/
#                 └── can_cfg_h.ftl
#
# Run manually from the repository root:
#
#   cmake -P tools/ConfigureCubeMXPaths.cmake
#
# Optional overrides:
#
#   cmake
#     -DIOC_FILE=/path/to/project.ioc
#     -DTEMPLATE_ROOT=/path/to/CubeMXTemplates
#     -DPROJECT_OUTPUT_DIR=/path/to/project
#     -P tools/ConfigureCubeMXPaths.cmake
#
# This script writes absolute paths because STM32CubeMX does not reliably
# resolve relative custom-template paths.

cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------------
# Locate repository and default paths
# ---------------------------------------------------------------------------

get_filename_component(SCRIPT_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
get_filename_component(REPOSITORY_ROOT "${SCRIPT_DIR}/.." ABSOLUTE)

if(NOT DEFINED IOC_FILE)
    set(IOC_FILE
        "${REPOSITORY_ROOT}/CUBEMXTESTING/CUBEMXTESTING.ioc"
    )
endif()

if(NOT DEFINED PROJECT_OUTPUT_DIR)
    set(PROJECT_OUTPUT_DIR
        "${REPOSITORY_ROOT}/CUBEMXTESTING"
    )
endif()

if(NOT DEFINED TEMPLATE_ROOT)
    set(TEMPLATE_ROOT
        "${REPOSITORY_ROOT}/Lib/CubeMXTemplates"
    )
endif()

set(TEMPLATE_RELATIVE_PATH
    "Autogen/Inc/can_cfg_h.ftl"
)

get_filename_component(IOC_FILE "${IOC_FILE}" ABSOLUTE)
get_filename_component(PROJECT_OUTPUT_DIR "${PROJECT_OUTPUT_DIR}" ABSOLUTE)
get_filename_component(TEMPLATE_ROOT "${TEMPLATE_ROOT}" ABSOLUTE)

set(TEMPLATE_FILE
    "${TEMPLATE_ROOT}/${TEMPLATE_RELATIVE_PATH}"
)

# ---------------------------------------------------------------------------
# Validate inputs
# ---------------------------------------------------------------------------

if(NOT EXISTS "${IOC_FILE}")
    message(FATAL_ERROR
        "CubeMX .ioc file was not found:\n"
        "  ${IOC_FILE}"
    )
endif()

if(NOT IS_DIRECTORY "${PROJECT_OUTPUT_DIR}")
    message(FATAL_ERROR
        "CubeMX project output directory was not found:\n"
        "  ${PROJECT_OUTPUT_DIR}"
    )
endif()

if(NOT IS_DIRECTORY "${TEMPLATE_ROOT}")
    message(FATAL_ERROR
        "CubeMX template root was not found:\n"
        "  ${TEMPLATE_ROOT}"
    )
endif()

if(NOT EXISTS "${TEMPLATE_FILE}")
    message(FATAL_ERROR
        "CubeMX FreeMarker template was not found:\n"
        "  ${TEMPLATE_FILE}"
    )
endif()

# ---------------------------------------------------------------------------
# Convert paths to the format expected by CubeMX's .ioc property file
# ---------------------------------------------------------------------------

function(convert_to_cubemx_path INPUT_PATH OUTPUT_VARIABLE)
    if(WIN32)
        # Convert:
        #   C:/Users/name/Firmware
        # to:
        #   C\:\\Users\\name\\Firmware
        file(TO_NATIVE_PATH "${INPUT_PATH}" NATIVE_PATH)
        string(REPLACE "\\" "\\\\" ESCAPED_PATH "${NATIVE_PATH}")
        string(REPLACE ":" "\\:" ESCAPED_PATH "${ESCAPED_PATH}")
    else()
        # macOS/Linux paths use forward slashes and require no drive-letter
        # escaping.
        file(TO_CMAKE_PATH "${INPUT_PATH}" ESCAPED_PATH)
    endif()

    set("${OUTPUT_VARIABLE}" "${ESCAPED_PATH}" PARENT_SCOPE)
endfunction()

convert_to_cubemx_path(
    "${PROJECT_OUTPUT_DIR}"
    CUBEMX_PROJECT_OUTPUT_DIR
)

convert_to_cubemx_path(
    "${TEMPLATE_ROOT}"
    CUBEMX_TEMPLATE_ROOT
)

if(WIN32)
    set(CUBEMX_TEMPLATE_LIST
        "\\\\Autogen\\\\Inc\\\\can_cfg_h.ftl,"
    )
else()
    set(CUBEMX_TEMPLATE_LIST
        "/Autogen/Inc/can_cfg_h.ftl,"
    )
endif()

set(DESTINATION_LINE
    "ProjectManager.TemplateDestinationPath=${CUBEMX_PROJECT_OUTPUT_DIR}"
)

set(SOURCE_LINE
    "ProjectManager.TemplateSourcePath=${CUBEMX_TEMPLATE_ROOT}"
)

set(TEMPLATES_LINE
    "ProjectManager.TemplatesList=${CUBEMX_TEMPLATE_LIST}"
)

# ---------------------------------------------------------------------------
# Rewrite or add the three CubeMX settings
# ---------------------------------------------------------------------------

file(STRINGS "${IOC_FILE}" IOC_LINES ENCODING UTF-8)

set(UPDATED_CONTENT "")
set(FOUND_DESTINATION FALSE)
set(FOUND_SOURCE FALSE)
set(FOUND_TEMPLATES FALSE)

foreach(LINE IN LISTS IOC_LINES)
    if(LINE MATCHES "^ProjectManager\\.TemplateDestinationPath=")
        string(APPEND UPDATED_CONTENT "${DESTINATION_LINE}\n")
        set(FOUND_DESTINATION TRUE)

    elseif(LINE MATCHES "^ProjectManager\\.TemplateSourcePath=")
        string(APPEND UPDATED_CONTENT "${SOURCE_LINE}\n")
        set(FOUND_SOURCE TRUE)

    elseif(LINE MATCHES "^ProjectManager\\.TemplatesList=")
        string(APPEND UPDATED_CONTENT "${TEMPLATES_LINE}\n")
        set(FOUND_TEMPLATES TRUE)

    else()
        string(APPEND UPDATED_CONTENT "${LINE}\n")
    endif()
endforeach()

if(NOT FOUND_DESTINATION)
    string(APPEND UPDATED_CONTENT "${DESTINATION_LINE}\n")
endif()

if(NOT FOUND_SOURCE)
    string(APPEND UPDATED_CONTENT "${SOURCE_LINE}\n")
endif()

if(NOT FOUND_TEMPLATES)
    string(APPEND UPDATED_CONTENT "${TEMPLATES_LINE}\n")
endif()

# Keep a backup of the current .ioc file before modifying it.
configure_file(
    "${IOC_FILE}"
    "${IOC_FILE}.bak"
    COPYONLY
)

file(WRITE "${IOC_FILE}" "${UPDATED_CONTENT}")

# ---------------------------------------------------------------------------
# Report result
# ---------------------------------------------------------------------------

message(STATUS "")
message(STATUS "CubeMX template configuration updated successfully.")
message(STATUS "")
message(STATUS "${DESTINATION_LINE}")
message(STATUS "${SOURCE_LINE}")
message(STATUS "${TEMPLATES_LINE}")
message(STATUS "")
message(STATUS "Backup created at:")
message(STATUS "  ${IOC_FILE}.bak")
