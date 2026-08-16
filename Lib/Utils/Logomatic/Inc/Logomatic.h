#include <stdio.h>
#include "main.h"

#ifndef LOGOMATIC_H
#define LOGOMATIC_H

/**
 * @brief Logomatic log levels
 *
 * These are the different log levels available in Logomatic
 *
 * @note To turn off logging, you must use CMAKE_LOGOMATIC_ENABLED
 * @warning Strictly ordered in increasing severity, changing this order will break filters
 */
typedef enum {
	/// @brief Verbose log level, used for detailed debugging information
	LogLevel_Verbose,
	/// @brief Debug log level, used for debugging information
	LogLevel_Debug,
	/// @brief Info log level, used for general information
	LogLevel_Info,
	/// @brief Warning log level, used for warnings
	LogLevel_Warning,
	/// @brief Error log level, used for errors
	LogLevel_Error,
	/// @brief Critical log level, used for critical errors
	LogLevel_Critical,
	/// @brief Disabled log level, used to turn off logging
	LogLevel_Off
} Logomatic_LogLevel;

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
extern Logomatic_LogLevel global_logomatic_level;

/**
 * @brief Logomatic driver function pointer
 *
 * This function pointer is used to point to the appropriate log driver function based on the selected log output method (e.g., ITM, UART, etc.)
 *
 * @note This variable should not be modified once setup by the user.
 */
extern Logomatic_Driver global_logomatic_driver;

/**
 * @brief Logomatic log level implementation macro
 *
 * This macro is used to implement the log level functionality to give fancy names
 *
 * @param level The log level of the message
 * @param ... The log message format and arguments
 */
#define LOGOMATIC_PRIVATE_LEVEL(level, ...)                                                                                                                                                            \
	do {                                                                                                                                                                                           \
		if (global_logomatic_level <= (level) && (level) != LogLevel_Off && global_logomatic_level != LogLevel_Off) {                                                                                 \
			_Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdouble-promotion\"") printf(__VA_ARGS__);                                                                   \
			_Pragma("GCC diagnostic pop")                                                                                                                                                  \
		}                                                                                                                                                                                      \
	} while (0)

#if defined(LOGOMATIC_ENABLED)

#define LOGOMATIC_CRITICAL(...) LOGOMATIC_PRIVATE_LEVEL(LogLevel_Critical, __VA_ARGS__)
#define LOGOMATIC_ERROR(...) LOGOMATIC_PRIVATE_LEVEL(LogLevel_Error, __VA_ARGS__)
#define LOGOMATIC_WARNING(...) LOGOMATIC_PRIVATE_LEVEL(LogLevel_Warning, __VA_ARGS__)
#define LOGOMATIC_INFO(...) LOGOMATIC_PRIVATE_LEVEL(LogLevel_Info, __VA_ARGS__)
#define LOGOMATIC_DEBUG(...) LOGOMATIC_PRIVATE_LEVEL(LogLevel_Debug, __VA_ARGS__)
#define LOGOMATIC_VERBOSE(...) LOGOMATIC_PRIVATE_LEVEL(LogLevel_Verbose, __VA_ARGS__)

#else

#define LOGOMATIC_OFF(...)                                                                                                                                                                             \
	do {                                                                                                                                                                                           \
		if (0) {                                                                                                                                                                               \
			printf(__VA_ARGS__);                                                                                                                                                           \
		}                                                                                                                                                                                      \
	} while (0)

#define LOGOMATIC_CRITICAL(...) LOGOMATIC_OFF(__VA_ARGS__)
#define LOGOMATIC_ERROR(...) LOGOMATIC_OFF(__VA_ARGS__)
#define LOGOMATIC_WARNING(...) LOGOMATIC_OFF(__VA_ARGS__)
#define LOGOMATIC_INFO(...) LOGOMATIC_OFF(__VA_ARGS__)
#define LOGOMATIC_DEBUG(...) LOGOMATIC_OFF(__VA_ARGS__)
#define LOGOMATIC_VERBOSE(...) LOGOMATIC_OFF(__VA_ARGS__)

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
