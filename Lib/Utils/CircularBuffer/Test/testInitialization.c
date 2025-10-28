#include "circularBuffer.h"
#include <stdlib.h>

int main()
{
	CircularBuffer *tmp;

	tmp =
	    GR_CircularBuffer_Create(0, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	if (tmp != NULL) {
		GR_CircularBuffer_Free(tmp);
		return 1;
	}

	tmp =
	    GR_CircularBuffer_Create(0, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	if (tmp != NULL) {
		GR_CircularBuffer_Free(tmp);
		return 2;
	}

	tmp =
	    GR_CircularBuffer_Create(5, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	if (tmp == NULL) {
		return 3;
	}
	GR_CircularBuffer_Free(tmp);

	tmp =
	    GR_CircularBuffer_Create(5, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	if (tmp == NULL) {
		return 4;
	}
	GR_CircularBuffer_Free(tmp);

	return 0;
}
