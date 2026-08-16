#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ComparatorMacros.h"
#include "Logomatic.h"
#include "Stringification.h"

const char *expected_output_critical = "%sThis is a critical message.\n";
const char *expected_output_error = "%sThis is an error message.\n";
const char *expected_output_warning = "%sThis is a warning message.\n";
const char *expected_output_info = "%sThis is an info message.\n";
const char *expected_output_debug = "%sThis is a debug message.\n";
const char *expected_output_verbose = "%sThis is a verbose message.\n";

bool verification(void (*print_func)(void), const char *expected_content)
{
	char formatted[100];
	char *buffer = NULL;
	size_t size = 0;

	FILE *original_stdout = stdout;

	FILE *mem_stream = open_memstream(&buffer, &size);
	if (!mem_stream) {
		perror("Failed to create memory stream");
		return false;
	}

	stdout = mem_stream;

	print_func();

	fflush(stdout);
	fclose(mem_stream);

	stdout = original_stdout;

	switch (_logomatic_loglevel) {
		case LogLevel_Critical:
			snprintf(formatted, sizeof(formatted), expected_content, "[CRIT] ");
			break;
		case LogLevel_Error:
			snprintf(formatted, sizeof(formatted), expected_content, "[ERROR] ");
			break;
		case LogLevel_Warning:
			snprintf(formatted, sizeof(formatted), expected_content, "[WARN] ");
			break;
		case LogLevel_Info:
			snprintf(formatted, sizeof(formatted), expected_content, "[INFO] ");
			break;
		case LogLevel_Debug:
			snprintf(formatted, sizeof(formatted), expected_content, "[DBG] ");
			break;
		case LogLevel_Verbose:
			snprintf(formatted, sizeof(formatted), expected_content, "[V - " __FILE_NAME__ ":117] ");
			break;
		default:
			snprintf(formatted, sizeof(formatted), expected_content, "");
			break;
	}

	bool match = strncmp(buffer, formatted, MIN_WITH_TYPES(size, strlen(formatted))) == 0;

	if (!match) {
		printf("Mismatch detected.\n");
		printf("  Expected: \"%s\"\n", formatted);
		printf("  Captured: \"%s\"\n", buffer);
	} else {
		printf("Output matches expected content for level %d with %s", _logomatic_loglevel, formatted);
	}

	free(buffer);

	return match;
}

void print_all(void)
{

	LOGOMATIC_CRITICAL(expected_output_critical);
	LOGOMATIC_ERROR(expected_output_error);
	LOGOMATIC_WARNING(expected_output_warning);
	LOGOMATIC_INFO(expected_output_info);
	LOGOMATIC_DEBUG(expected_output_debug);
	LOGOMATIC_VERBOSE(expected_output_verbose);
}

void printf_critical(void)
{
	LOGOMATIC_CRITICAL(expected_output_critical, "");
}

void printf_error(void)
{
	LOGOMATIC_ERROR(expected_output_error, "");
}

void printf_warning(void)
{
	LOGOMATIC_WARNING(expected_output_warning, "");
}

void printf_info(void)
{
	LOGOMATIC_INFO(expected_output_info, "");
}

void printf_debug(void)
{
	LOGOMATIC_DEBUG(expected_output_debug, "");
}

void printf_verbose(void)
{
	LOGOMATIC_VERBOSE(expected_output_verbose, "");
}

int main(void)
{
	_logomatic_loglevel = LogLevel_Off;
	if (!verification(print_all, "")) {
		return 1;
	}
	_logomatic_loglevel = LogLevel_Critical;
	if (!verification(printf_critical, expected_output_critical)) {
		return 2;
	}
	_logomatic_loglevel = LogLevel_Error;
	if (!verification(printf_critical, "")) {
		return 3;
	}
	_logomatic_loglevel = LogLevel_Error;
	if (!verification(printf_error, expected_output_error)) {
		return 4;
	}
	_logomatic_loglevel = LogLevel_Warning;
	if (!verification(printf_warning, expected_output_warning)) {
		return 5;
	}
	_logomatic_loglevel = LogLevel_Info;
	if (!verification(printf_info, expected_output_info)) {
		return 6;
	}
	_logomatic_loglevel = LogLevel_Info;
	if (!verification(printf_debug, "")) {
		return 7;
	}
	_logomatic_loglevel = LogLevel_Debug;
	if (!verification(printf_debug, expected_output_debug)) {
		return 8;
	}
	_logomatic_loglevel = LogLevel_Verbose;
	if (!verification(printf_verbose, expected_output_verbose)) {
		return 9;
	}

	return 0;
}
