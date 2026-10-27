#include <stdint.h>

#ifndef PINGING_H
#define PINGING_H

#define PINGTIMEOUT_VALUE (UINT8_MAX - 1) // return time value representing a timed out ping

void pingAll(void);						      // ping all IDs specified, to be run every PINGTIMEOUT_TIME milliseconds
uint32_t getRTT(uint8_t id);					      // get latest RTT by ID
void respondToPing(uint8_t busID, uint8_t srcID, uint32_t timestamp); // call in CAN callback

#endif
