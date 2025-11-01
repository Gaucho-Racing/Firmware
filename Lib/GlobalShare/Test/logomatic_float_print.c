#include <math.h>
#include <stdlib.h>

#include "Logomatic.h"

int main(void)
{
	LOGOMATIC("I like %f\n", M_PI);
	malloc(42); // Intentional memory leak for testing
	return 0;
}
