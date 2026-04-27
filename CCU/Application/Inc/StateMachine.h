#include <stdbool.h>
#include <stdint.h>

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef uint8_t CCU_STATE;

enum CCU_STATE { // State Machine
	/*
	 * The CCU is listening for BCU_STATUS_2 and 3 msgs, waiting to be told to start charging
	 */
	CCU_STATE_IDLE = 0, // Transition: Told to charge AND checks passed
	/*
	 * The CCU has been told to charge and checks have passed, starts charging
	 */
	CCU_STATE_CHARGING // Transition: Told to stop OR fault
};

typedef enum {
	PRECHARGE_STEP_WAIT_IR_MINUS = 0,
	PRECHARGE_STEP_WAIT_IR_PLUS,
	PRECHARGE_STEP_COMPLETE
} CCU_Precharge_Step;

#endif
