#include "circularBuffer.h"
#include <stdlib.h>

int main()
{
	CircularBuffer *tmp;

	tmp = GR_CircularBuffer_Create(0);
	if (tmp != NULL) {
		GR_CircularBuffer_Free(tmp);
		return 1;
	}

	tmp = GR_CircularBuffer_Create(5);
	if (tmp == NULL) {
		return 2;
	}
	GR_CircularBuffer_Free(tmp);

	return 0;
}
