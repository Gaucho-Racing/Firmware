# --- 1. Define Paths ---
set(CAN_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(YAML_INPUT "${CAN_DIR}/Doc/format.CANdo")
set(INC_DIR "${CAN_DIR}/Inc")
set(SCRIPT_DIR "${CAN_DIR}/Src")

# error handling
function(run_can_parser script_name output_file)
	execute_process(
		COMMAND
			perl "${SCRIPT_DIR}/${script_name}" "${YAML_INPUT}"
			"${INC_DIR}/${output_file}"
		RESULT_VARIABLE result
		ERROR_VARIABLE error_output
	)

	if(NOT result EQUAL 0)
		message(
			FATAL_ERROR
			"\n[CANfigurator] FAILED to run ${script_name}\n"
			"Exit Code: ${result}\n"
			"Error Message: ${error_output}\n"
		)
	endif()
endfunction()

function(run_perl_files)
	run_can_parser("CANdlerparser.pl" "CANDler.h")
	run_can_parser("CANparser.pl"    "Custom_CAN_ID.h")
	run_can_parser("GRparser.pl"     "GR_IDS.h")
	run_can_parser("MSGparser.pl"    "can_msg_ids.h")
endfunction()

# --- 3. Execute Commands --
execute_process(
	COMMAND
		mkdir -p "${INC_DIR}"
)

message(STATUS "CANfigurator: Generating headers in ${INC_DIR}")

run_perl_files()

# --- 4. The Interface Library ---
add_library(CANfigurator INTERFACE)

target_include_directories(CANfigurator INTERFACE "${INC_DIR}")
