#include "Loop.h"

#include "Logomatic.h"

void Loop(void)
{
	LOGOMATIC_CRITICAL("HELLO, WORLD!!!\n");
	LOGOMATIC_ERROR("Hello, World!!!\n");
	LOGOMATIC_WARNING("Hello, World!!\n");
	LOGOMATIC_INFO("Hello, World!\n");
	LOGOMATIC_DEBUG("Hello, World.\n");
	LOGOMATIC_VERBOSE("Hello, World...\n");
}
