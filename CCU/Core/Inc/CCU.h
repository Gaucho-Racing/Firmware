#include <stdbool.h>
#include <stdint.h>

// IDK if this is any good, if so, prolly move to move, just wanted to get
// something down

typedef enum { /*Basic states that I could come up with*/

	       CCU_STATE_IDLE = 0,
	       CCU_STATE_PRECHECK,
	       CCU_STATE_PRECAHRGE,
	       CCU_STATE_CHARGING,
	       CCU_STATE_COMPLETE,
	       CCU_STATE_FAULT

} ccu_state_t;

typedef enum { /*probably more fault states should be add*/

	       CCU_FAULT_NONE = 0,
	       CCU_FAULT_NO_BCU,
	       CCU_FAULT_BCU_DENY,
	       CCU_FAULT_OVERVOLT,
	       CCU_FAULT_OVERCURR,
	       CCU_FAULT_OVERTEMP,
	       CCU_FAULT_PRECHARGE_TIMEOUT

} ccu_fault_t;

typedef struct { /*probably more inputs, im just kinda guessing these just to
		    get a feel yk (sorry for bad comms) */

	bool start_button;
	float voltage_v;
	float charge_current_A;
	float temp_c;

} ccu_inputs_t;

typedef struct {	      // ouputs to power
	bool precharge_relay; // precharge path is enabled
	bool main_contactor;  // charge contactor  is enabled
	bool charger_enable;  // signal to charger/to charge
	bool status_led;      // indicator

} ccu_outputs_t;

typedef struct { // limits received by bcu
	bool bcu_alive;
	bool bcu_allows_charge;
	float max_charge_current_A;
	float max_pack_voltage_V;
	uint32_t bcu_fault_flags;
} ccu_bcu_t;

typedef struct {
	ccu_state_t state;
	ccu_fault_t fault;

	ccu_inputs_t in;
	ccu_outputs_t out;
	ccu_bcu_t bcu;

	// Timing cause I feel like thats need
	uint32_t ms_in_state;
	uint32_t last_bcu_message;

} ccu_t;

void ccu_init(ccu_t *ccu);
void ccu_set_inputs(ccu_t *ccu, const ccu_inputs_t *in);
void ccu_tick(ccu_t *ccu, uint32_t now_ms,
	      uint32_t delta_ms); // now_ms = curr time, delta_ms = time since last