/*
 * To Do:
 * Check head and tail at every test
 * - Test Initialization
 * - Test Push/Pop
 *     - Pop empty buffer --> should do nothing
 *     - Push full buffer --> test data overwrite
 *     - Push buffer past size limite --> test data loopback
 * - Test Peek
 * - Test Memory stuff (Address Sanitizer + Maybe Valgrind)
 *     - Test free
 *     - Make sure memory overwrite free's the old data
**/
#include "circularBuffer.h"
#include <stdbool.h>
#include <stdlib.h>

int testInitialization()
{
	CircularBuffer *tmp;

	tmp = cbOps.create(0);
	if (tmp != NULL) {
		cbOps.free(tmp);
		return 12;
	}

	tmp = cbOps.create(5);
	if (tmp == NULL) {
		return 13;
	}
	cbOps.free(tmp);

	tmp = cbOps.create(100);
	if (tmp == NULL) {
		return 14;
	}
	cbOps.free(tmp);

	return 0;
}

int testPushPop(){

	CircularBuffer* buffer = cbOps.create(10);

	// pushing without overwriting
	for(int i = 0; i < 10; i++){
		int tmp = i;
		cbOps.push(buffer, &tmp, sizeof(tmp));
	}
	for(int i = 0; i < 10; i++){
		int* tmp;
		tmp = cbOps.pop(buffer);
		if(*tmp != i){
			cbOps.free(buffer);
			return 21;
		}
	}

	// poping empty buffer
	for(int i = 0; i < 1000; i++)
		cbOps.pop(buffer);
	for(int i = 0; i < 1000; i++){
		if(cbOps.pop(buffer) != NULL){ // Should do nothing
			cbOps.free(buffer);
			return 22;
		}
	}

	// pushing past size limite
	for(int i = 0; i < 1000; i++){
		int tmp = i;
		cbOps.push(buffer, &tmp, sizeof(tmp));
	}
	for(int i = 0; i < 10; i++){
		int tmp = i;
		cbOps.push(buffer, &tmp, sizeof(tmp));
	}
	for(int i = 0; i < 10; i++){
		int* tmp;
		tmp = cbOps.pop(buffer);
		if(*tmp != i){
			cbOps.free(buffer);
			return 23;
		}
	}

	cbOps.free(buffer);
	return 0;
}

int testPeek(){
	CircularBuffer* buffer = cbOps.create(10);

	// pushing without overwriting
	for(int i = 0; i < 10; i++){
		int tmp = i;
		cbOps.push(buffer, &tmp, sizeof(tmp));
	}
	for(int i = 0; i < 1000; i++){
		int* tmp;
		tmp = cbOps.peek(buffer);
		if(*tmp != 0){ // should not modify the buffer
			cbOps.free(buffer);
			return 21;
		}
	}

	cbOps.free(buffer);
	return 0;
}

int main()
{
	int code;
	if ((code = testInitialization()))
		return code;
	if ((code = testPushPop()))
		return code;
	if ((code = testPeek()))
		return code;
	
	return 0;
}
