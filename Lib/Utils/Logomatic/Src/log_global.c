#include "Logomatic.h"
#include "main.h"

Logomatic_LogLevel global_logomatic_level = LogLevel_Info;

#if defined(ITM)
Logomatic_Driver global_logomatic_driver = Logomatic_Driver_ITM;
#elif defined(LPUART1)
Logomatic_Driver global_logomatic_driver = Logomatic_Driver_LPUART1;
#else
Logomatic_Driver global_logomatic_driver = NULL;
#endif
