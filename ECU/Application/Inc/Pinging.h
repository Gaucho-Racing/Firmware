#include <stdint.h>

#ifndef PINGING_H
#define PINGING_H

#define PINGTIMEOUT_TIME 250U		  // time in ms before a ping is considered timed out
#define PINGTIMEOUT_VALUE (UINT8_MAX - 1) // return time value representing a timed out ping

void pingAll(void);				       // ping all IDs specified, to be run every PINGTIMEOUT_TIME milliseconds
uint32_t getRTT(uint8_t id);			       // get latest RTT by ID
void respondToPing(uint8_t srcID, uint32_t timestamp); // "callback"

#endif
