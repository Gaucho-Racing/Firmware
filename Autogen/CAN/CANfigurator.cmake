# ============================================================
# CANfigurator.cmake
# Ensures Perl + YAML::XS and generates CAN headers
# ============================================================

# --- 0. Define paths ---
set(CAN_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(YAML_INPUT "${CAN_DIR}/Doc/format.CANdo")
set(INC_DIR "${CAN_DIR}/Inc")
set(SCRIPT_DIR "${CAN_DIR}/Src")

# --- 1. Find Perl ---
find_package(Perl REQUIRED)

# --- 2. Define a local Perl install dir (portable & deterministic) ---
set(PERL_LOCAL_LIB "${CMAKE_BINARY_DIR}/perl5")
file(MAKE_DIRECTORY "${PERL_LOCAL_LIB}/lib/perl5")

# --- 3. Set PERL5LIB globally for all Perl calls ---
set(ENV{PERL5LIB} "${PERL_LOCAL_LIB}/lib/perl5:$ENV{PERL5LIB}")

# --- 4. Check for YAML::XS ---
execute_process(
	COMMAND
		${PERL_EXECUTABLE} -MYAML::XS -e "1"
	RESULT_VARIABLE YAML_XS_FOUND
)

# --- 5. Install YAML::XS via CPAN if missing ---
if(NOT YAML_XS_FOUND EQUAL 0)
	message(
		STATUS
		"Perl module YAML::XS not found. Installing locally via CPAN..."
	)

	execute_process(
		COMMAND
			${PERL_EXECUTABLE} -MCPAN -e
			"CPAN::HandleConfig->load; CPAN::Shell->notest('install','YAML::XS')"
		RESULT_VARIABLE CPAN_RESULT
		ENVIRONMENT
		PERL5LIB=${PERL_LOCAL_LIB}/lib/perl5
		PERL_MM_OPT=INSTALL_BASE=${PERL_LOCAL_LIB}
		PERL_MB_OPT=--install_base=${PERL_LOCAL_LIB}
	)

	if(NOT CPAN_RESULT EQUAL 0)
		message(
			FATAL_ERROR
			"Failed to install Perl module YAML::XS.\nTry manually:\n  perl -MCPAN -e 'install YAML::XS'"
		)
	endif()
endif()

# --- 6. Create output directory (portable) ---
file(MAKE_DIRECTORY "${INC_DIR}")
message(STATUS "CANfigurator: Generating headers in ${INC_DIR}")

# --- 7. Error-handled Perl runner ---
function(run_can_parser script_name output_file)
	execute_process(
		COMMAND
			${PERL_EXECUTABLE} "${SCRIPT_DIR}/${script_name}" "${YAML_INPUT}"
			"${INC_DIR}/${output_file}"
		RESULT_VARIABLE result
		ERROR_VARIABLE error_output
	)

	if(NOT result EQUAL 0)
		message(
			FATAL_ERROR
			"\n[CANfigurator] FAILED to run ${script_name}\n"
			"Exit Code: ${result}\n"
			"Error Message:\n${error_output}\n"
		)
	endif()
endfunction()

# --- 8. Run all generators ---
function(run_perl_files)
	run_can_parser("CANdlerparser.pl" "CANDler.h")
	run_can_parser("CANparser.pl"    "Custom_CAN_ID.h")
	run_can_parser("GRparser.pl"     "GR_IDS.h")
	run_can_parser("MSGparser.pl"    "can_msg_ids.h")
endfunction()

run_perl_files()

# --- 9. Interface library for include paths ---
add_library(CANfigurator INTERFACE)
target_include_directories(CANfigurator INTERFACE "${INC_DIR}")
