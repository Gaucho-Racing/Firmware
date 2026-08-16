#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Logomatic.h"

int main()
{
	const char expected[] = "[CRITICAL] Critical log message\n"
				"[ERROR] Error log message\n"
				"[WARNING] Warning log message\n"
				"[INFO] Informational log message\n"
				"[DEBUG] Debug log message\n"
				"[VERBOSE - print.c:34] Verbose log message\n";

	int pipefd[2];
	char buffer[sizeof(expected) + 50] = {0};

	if (pipe(pipefd) == -1) {
		perror("Pipe failed");
		return 1;
	}

	int saved_stdout = dup(STDOUT_FILENO);
	dup2(pipefd[1], STDOUT_FILENO);

	LOGOMATIC_CRITICAL("Critical log message\n");
	LOGOMATIC_ERROR("Error log message\n");
	LOGOMATIC_WARNING("Warning log message\n");
	LOGOMATIC_INFO("Informational log message\n");
	LOGOMATIC_DEBUG("Debug log message\n");
	LOGOMATIC_VERBOSE("Verbose log message\n");

	fflush(stdout);
	close(pipefd[1]);

	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);

	ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
	if (bytes_read >= 0) {
		buffer[bytes_read] = '\0';
	}
	close(pipefd[0]);

	if (strcmp(buffer, expected) == 0) {
		printf("Test passed: Log output matches expected output.\n");
	} else {
		printf("Test failed: Log output does not match expected output.\n");
		printf("\nExpected:\n%s", expected);
		printf("\nGot:\n%s", buffer);
		return 1;
	}

	LOGOMATIC_CRITICAL("Critical log message again\n");
	LOGOMATIC_ERROR("Error log message again\n");
	LOGOMATIC_WARNING("Warning log message again\n");
	LOGOMATIC_INFO("Informational log message again\n");
	LOGOMATIC_DEBUG("Debug log message again\n");
	LOGOMATIC_VERBOSE("Verbose log message again\n");

	return 0;
}
