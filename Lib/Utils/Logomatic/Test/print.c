#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>

#define sys_pipe(fd) _pipe(fd, 4096, _O_BINARY)
#define sys_dup _dup
#define sys_dup2 _dup2
#define sys_read _read
#define sys_close _close

#ifndef STDOUT_FILENO
#define STDOUT_FILENO _fileno(stdout)
#endif

typedef ptrdiff_t ssize_t;
#else
#include <unistd.h>

#define sys_pipe(fd) pipe(fd)
#define sys_dup dup
#define sys_dup2 dup2
#define sys_read read
#define sys_close close
#endif

#include "Logomatic.h"

int main(void)
{
	const char expected[] = "[CRITICAL] Critical log message\n"
				"[ERROR] Error log message\n"
				"[WARNING] Warning log message\n"
				"[INFO] Informational log message\n"
				"[DEBUG] Debug log message\n"
				"[VERBOSE - print.c:60] Verbose log message\n";

	int pipefd[2];
	char buffer[sizeof(expected) + 50] = {0};

	if (sys_pipe(pipefd) == -1) {
		perror("Pipe failed");
		return 1;
	}

	int saved_stdout = sys_dup(STDOUT_FILENO);

#ifdef _WIN32
	_setmode(STDOUT_FILENO, _O_BINARY);
#endif

	sys_dup2(pipefd[1], STDOUT_FILENO);

	LOGOMATIC_CRITICAL("Critical log message\n");
	LOGOMATIC_ERROR("Error log message\n");
	LOGOMATIC_WARNING("Warning log message\n");
	LOGOMATIC_INFO("Informational log message\n");
	LOGOMATIC_DEBUG("Debug log message\n");
	LOGOMATIC_VERBOSE("Verbose log message\n");

	fflush(stdout);
	sys_close(pipefd[1]);

	sys_dup2(saved_stdout, STDOUT_FILENO);
	sys_close(saved_stdout);

#ifdef _WIN32
	_setmode(STDOUT_FILENO, _O_TEXT);
#endif

	ssize_t bytes_read = sys_read(pipefd[0], buffer, sizeof(buffer) - 1);
	if (bytes_read >= 0) {
		buffer[bytes_read] = '\0';
	}
	sys_close(pipefd[0]);

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
