#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "coredemo.h"

int main(int argc, char const *argv[])
{
	int c = 2;
	printf("Advanced Core Hello : %d\n", demoComputation(c));
	if (demoComputation(c) == c / 2) {
		return 0;
	}
	return -1;
}
