#include "Logomatic.h"
#include <stdlib.h>

int main(void)
{
	LOGOMATIC("Hello, Logomatic!\n");
	malloc(42);	// Intentional memory leak for testing
	return 0;
}
