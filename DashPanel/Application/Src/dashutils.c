#include "dashutils.h"
#include "main.h"

// from ECU
uint32_t MillisecondsSinceBoot(void)
{
	// For some reason, GetTickFreq returns period in millisecon instead of frequency
	// See https://community.st.com/t5/stm32-mcus-embedded-software/name-amp-description-of-hal-gettickfreq-misleading/td-p/242457
	return HAL_GetTick() * HAL_GetTickFreq();
}
