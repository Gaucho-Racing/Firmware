#include "Logomatic.h"
#include "StateTicks.h"

int main(void)
{
	// Simulate state ticks, if creates its own infinite loop then something
	// is wrong
	for (int i = 0; i < 5; i++) {
		ECU_State_Tick();
	}

	return 0;
}
