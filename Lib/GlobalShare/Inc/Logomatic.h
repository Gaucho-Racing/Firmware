#include <stdio.h>

#ifndef _LOGOMATIC_H_
	#define _LOGOMATIC_H_

	#ifdef LOGOMATIC_ENABLED
		#define LOGOMATIC(...) printf(__VA_ARGS__)
	#else
		#define LOGOMATIC(...)
	#endif

	#if defined(LOGOMATIC_ENABLED) && defined(ITM)
		__attribute__((weak)) int __io_putchar(int ch)
		{
			ITM_SendChar(ch);
			return ch;
		}
	#endif
#endif
