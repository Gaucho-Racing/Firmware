#include <string.h>

#include "Unused.h"
#include "Stringification.h"

int main(void)
{
	const char phrase[11] = "fourty two";
	UNUSED(phrase);
	return strcmp(STRINGIFY(phrase), "phrase");
}
