#include "Logomatic.h"

int main(void)
{
	int fourtytwo = 42;
	LOGOMATIC("Hello, Logomatic!\n");
	LOGOMATIC("Verified unused parameters work! %d\n", fourtytwo);
	return 0;
}
