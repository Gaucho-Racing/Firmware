#include <string.h>

#include "Stringification.h"
#include "Unused.h"

int main(void)
{
	const char phrase[11] = "fourty two";
	UNUSED(phrase);
	return strcmp(STRINGIFY(phrase), "phrase");
}
