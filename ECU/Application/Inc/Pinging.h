#include <stdint.h>

#ifndef PINGING_H
#define PINGING_H

#define PINGTIMEOUT 250


void pingAll(void); // ping all IDs specified, to be run every PINGTIMEOUT milliseconds
uint32_t getRTT(uint8_t id); // get latest RTT by ID
void respondToPing(uint8_t srcID, uint32_t timestamp); // "callback"


#endif
