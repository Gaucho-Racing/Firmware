#include <stdbool.h>
#include <stdio.h>

#include "main.h"

#ifndef LOGOMATIC_H
#define LOGOMATIC_H

/// @brief Logomatic log level type, used to define the severity of log messages
#define _LOGOMATIC_LOGLEVEL_PRIVATE_LIST                                                                                                                                                               \
	X(LogLevel_Off, 0, "OFF")                                                                                                                                                                      \
	X(LogLevel_Critical, 1, "CRITICAL")                                                                                                                                                            \
	X(LogLevel_Error, 2, "ERROR")                                                                                                                                                                  \
	X(LogLevel_Warning, 3, "WARNING")                                                                                                                                                              \
	X(LogLevel_Info, 4, "INFO")                                                                                                                                                                    \
	X(LogLevel_Debug, 5, "DEBUG")                                                                                                                                                                  \
	X(LogLevel_Verbose, 6, "VERBOSE")

typedef enum {
#define X(name, val, str) name = val,
	_LOGOMATIC_LOGLEVEL_PRIVATE_LIST
#undef X
} Logomatic_LogLevel;

/**
 * @brief Set the global log level
 *
 * Only logs with a higher or equal severity than the global log level will be printed.
 *
 * @param level The log level to set as the global log level. This should be one of the values from the Logomatic_LogLevel enum.
 * @return true if the log level was successfully set, false if the provided log level is invalid.
 */
bool Logomatic_SetLogLevel(Logomatic_LogLevel level);

/**
 * @brief Logomatic log level strings
 *
 * This array is used to map the log level enum values to their corresponding string representations for printing log messages.
 *
 * @note The order of the strings in this array must match the order of the enum values in Logomatic_LogLevel.
 */
extern const char *Logomatic_Private_LogLevelStrings[];

/**
 * @brief Logomatic driver function pointer type
 *
 * This is used to define the log driver function that will be used to output log messages
 *
 * @note Default log drivers are provided for you but you can implement your own log driver by defining a function that matches this signature and assigning it to the global_logomatic_driver variable
 */
typedef int (*Logomatic_Driver)(int);

/**
 * @brief Global log level variable
 *
 * Store the global log level in this variable. It is used to determine which log messages should be printed based on their severity.
 *
 * @warning This variable should not be modified once setup by the user.
 */
extern Logomatic_LogLevel _logomatic_loglevel;

/**
 * @brief Logomatic driver function pointer
 *
 * This function pointer is used to point to the appropriate log driver function based on the selected log output method (e.g., ITM, UART, etc.)
 *
 * @note This variable should not be modified once setup by the user.
 */
#ifdef LOGOMATIC_HOOTLTEST
extern const Logomatic_Driver global_logomatic_driver;
#else
extern const __weak Logomatic_Driver global_logomatic_driver;
#endif

#ifdef LOGOMATIC_HOOTLTEST

#define LOGOMATIC_CRITICAL(...)                                                                                                                                                                        \
	do {                                                                                                                                                                                           \
		printf("[CRITICAL] ");                                                                                                                                                                 \
		printf(__VA_ARGS__);                                                                                                                                                                   \
	} while (0)
#define LOGOMATIC_ERROR(...)                                                                                                                                                                           \
	do {                                                                                                                                                                                           \
		printf("[ERROR] ");                                                                                                                                                                    \
		printf(__VA_ARGS__);                                                                                                                                                                   \
	} while (0)
#define LOGOMATIC_WARNING(...)                                                                                                                                                                         \
	do {                                                                                                                                                                                           \
		printf("[WARNING] ");                                                                                                                                                                  \
		printf(__VA_ARGS__);                                                                                                                                                                   \
	} while (0)
#define LOGOMATIC_INFO(...)                                                                                                                                                                            \
	do {                                                                                                                                                                                           \
		printf("[INFO] ");                                                                                                                                                                     \
		printf(__VA_ARGS__);                                                                                                                                                                   \
	} while (0)
#define LOGOMATIC_DEBUG(...)                                                                                                                                                                           \
	do {                                                                                                                                                                                           \
		printf("[DEBUG] ");                                                                                                                                                                    \
		printf(__VA_ARGS__);                                                                                                                                                                   \
	} while (0)
#define LOGOMATIC_VERBOSE(...)                                                                                                                                                                         \
	do {                                                                                                                                                                                           \
		printf("[VERBOSE - %s:%d] ", __FILE_NAME__, __LINE__);                                                                                                                                 \
		printf(__VA_ARGS__);                                                                                                                                                                   \
	} while (0)

#else

#if defined(LOGOMATIC_ENABLED)
#define _LOGOMATIC_PRIVATE_LOG(level, ...)                                                                                                                                                             \
	do {                                                                                                                                                                                           \
		if (_logomatic_loglevel >= (level) && (level) != LogLevel_Off && _logomatic_loglevel != LogLevel_Off) {                                                                                \
			if (level == LogLevel_Verbose) {                                                                                                                                               \
				printf("[%s - %s:%d] ", Logomatic_Private_LogLevelStrings[(level)], __FILE_NAME__, __LINE__);                                                                          \
			} else {                                                                                                                                                                       \
				printf("[%s] ", Logomatic_Private_LogLevelStrings[(level)]);                                                                                                           \
			}                                                                                                                                                                              \
			_Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdouble-promotion\"") printf(__VA_ARGS__);                                                                   \
			_Pragma("GCC diagnostic pop")                                                                                                                                                  \
		}                                                                                                                                                                                      \
	} while (0)
#define LOGOMATIC_CRITICAL(...) _LOGOMATIC_PRIVATE_LOG(LogLevel_Critical, __VA_ARGS__)
#define LOGOMATIC_ERROR(...) _LOGOMATIC_PRIVATE_LOG(LogLevel_Error, __VA_ARGS__)
#define LOGOMATIC_WARNING(...) _LOGOMATIC_PRIVATE_LOG(LogLevel_Warning, __VA_ARGS__)
#define LOGOMATIC_INFO(...) _LOGOMATIC_PRIVATE_LOG(LogLevel_Info, __VA_ARGS__)
#define LOGOMATIC_DEBUG(...) _LOGOMATIC_PRIVATE_LOG(LogLevel_Debug, __VA_ARGS__)
#define LOGOMATIC_VERBOSE(...) _LOGOMATIC_PRIVATE_LOG(LogLevel_Verbose, __VA_ARGS__)
#else
#define _LOGOMATIC_PRIVATE_LOG(...)                                                                                                                                                                    \
	do {                                                                                                                                                                                           \
		if (0) {                                                                                                                                                                               \
			printf(__VA_ARGS__);                                                                                                                                                           \
		}                                                                                                                                                                                      \
	} while (0)
#define LOGOMATIC_CRITICAL(...) _LOGOMATIC_PRIVATE_LOG(__VA_ARGS__)
#define LOGOMATIC_ERROR(...) _LOGOMATIC_PRIVATE_LOG(__VA_ARGS__)
#define LOGOMATIC_WARNING(...) _LOGOMATIC_PRIVATE_LOG(__VA_ARGS__)
#define LOGOMATIC_INFO(...) _LOGOMATIC_PRIVATE_LOG(__VA_ARGS__)
#define LOGOMATIC_DEBUG(...) _LOGOMATIC_PRIVATE_LOG(__VA_ARGS__)
#define LOGOMATIC_VERBOSE(...) _LOGOMATIC_PRIVATE_LOG(__VA_ARGS__)
#endif

#endif

#if defined(ITM)
int Logomatic_Driver_ITM(int ch);
#endif

#if defined(LPUART1)
int Logomatic_Driver_LPUART1(int ch);
#endif

#if defined(LPUART2)
int Logomatic_Driver_LPUART2(int ch);
#endif

#if defined(LPUART3)
int Logomatic_Driver_LPUART3(int ch);
#endif

#if defined(USART1)
int Logomatic_Driver_USART1(int ch);
#endif

#if defined(USART2)
int Logomatic_Driver_USART2(int ch);
#endif

#if defined(USART3)
int Logomatic_Driver_USART3(int ch);
#endif

#if defined(USART4)
int Logomatic_Driver_USART4(int ch);
#endif

#if defined(USART5)
int Logomatic_Driver_USART5(int ch);
#endif

#if defined(USART6)
int Logomatic_Driver_USART6(int ch);
#endif

#if defined(USART7)
int Logomatic_Driver_USART7(int ch);
#endif

#if defined(USART8)
int Logomatic_Driver_USART8(int ch);
#endif

#if defined(UART1)
int Logomatic_Driver_UART1(int ch);
#endif

#if defined(UART2)
int Logomatic_Driver_UART2(int ch);
#endif

#if defined(UART3)
int Logomatic_Driver_UART3(int ch);
#endif

#if defined(UART4)
int Logomatic_Driver_UART4(int ch);
#endif

#if defined(UART5)
int Logomatic_Driver_UART5(int ch);
#endif

#if defined(UART6)
int Logomatic_Driver_UART6(int ch);
#endif

#if defined(UART7)
int Logomatic_Driver_UART7(int ch);
#endif

#if defined(UART8)
int Logomatic_Driver_UART8(int ch);
#endif

#endif
