#include "Logomatic.h"
#include "main.h"

#if defined(LOGOMATIC_HOOTLTEST)
Logomatic_LogLevel _logomatic_loglevel = LogLevel_Verbose;
#else
Logomatic_LogLevel _logomatic_loglevel = LogLevel_Debug;
#endif

#if defined(ITM)
__weak const Logomatic_Driver global_logomatic_driver = Logomatic_Driver_ITM;
#elif defined(LPUART1)
__weak const Logomatic_Driver global_logomatic_driver = Logomatic_Driver_LPUART1;
#else
__weak const Logomatic_Driver global_logomatic_driver = NULL;
#endif

const char *Logomatic_Private_LogLevelStrings[] = {
#define X(name, val, str) [val] = str,
    _LOGOMATIC_LOGLEVEL_PRIVATE_LIST
#undef X
};

bool Logomatic_SetLogLevel(Logomatic_LogLevel level)
{
#if defined(LOGOMATIC_HOOTLTEST)
	_logomatic_loglevel = LogLevel_Verbose;
	return true;
#endif

#define X(name, val, string)                                                                                                                                                                           \
	if (level == val) {                                                                                                                                                                            \
		_logomatic_loglevel = level;                                                                                                                                                           \
		return true;                                                                                                                                                                           \
	}
	_LOGOMATIC_LOGLEVEL_PRIVATE_LIST
#undef X
	return false;
}
