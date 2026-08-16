#include "Logomatic.h"
#include "main.h"

__weak Logomatic_LogLevel _logomatic_loglevel = LogLevel_Debug;

#if defined(ITM)
__weak const Logomatic_Driver global_logomatic_driver = Logomatic_Driver_ITM;
#elif defined(LPUART1)
__weak const Logomatic_Driver global_logomatic_driver = Logomatic_Driver_LPUART1;
#else
__weak const Logomatic_Driver global_logomatic_driver = NULL;
#endif

const char *Logomatic_Private_LogLevelStrings[] = {
#define X(name, val, str) [val] = str,
    _LOGOMATIC_LOGLEVEL_X_LIST
#undef X
};

bool Logomatic_SetLogLevel(Logomatic_LogLevel level)
{
#define X(name, val, string)                                                                                                                                                                           \
	if (level == val) {                                                                                                                                                                            \
		_logomatic_loglevel = level;                                                                                                                                                           \
		return true;                                                                                                                                                                           \
	}
	_LOGOMATIC_LOGLEVEL_X_LIST
#undef X
	return false;
}
