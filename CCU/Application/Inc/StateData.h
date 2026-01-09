#include <stdbool.h>
#include <stdint.h>

// IDK if this is any good, if so, prolly move to move, just wanted to get
// something down

typedef enum { /*Basic states that I could come up with*/

	       CCU_STATE_IDLE =
		   0, // Transition: Told to charge AND checks passed
	       CCU_STATE_CHARGING, // Transition: Told to stop OR fault

} ccu_state_t;

typedef struct { /*probably more fault states should be add*/

	// changed from enum to struct
	// FIXME: This needs to bis not bytes
	bool CCU_FAULT_HARDWARE_FAILURE;
	bool CCU_FAULT_WRONG_VOLT;
	bool CCU_FAULT_CONNECTION_ERROR;
	bool CCU_FAULT_OVERTEMP;
	bool CCU_FAULT_COMMUNICATION_TIMEOUT;

} ccu_fault_t;

typedef struct { // Stores Charger Control CAN (RX, input)

	uint16_t CCU_REQUESTED_VOLTAGE;
	uint16_t CCU_REQUESTED_CURR;
	uint8_t CCU_CONTROL; // should be a bit

} ccu_charger_control_t;

typedef struct { // Stores Charger DATA CAN (TX, output)

	uint8_t CCU_OUTPUT_VOLTAGE;
	uint8_t CCU_OUTPUT_CURR;
	ccu_fault_t fault; // probably need a better representation of this
			   // since it needs  0000, 0101, etc.

} ccu_charger_data_t;

typedef struct {

	float EM_MEASUREMENT_CURRENT;
	float EM_MEASUREMENT_VOLTAGE;
	bool EM_STATUS_VIOLATION;
	bool EM_STATUS_LOGGING;
	float EM_STATUS_ENERGY;
	uint8_t EM_TEMP_MUX_SIGNAL;
	uint8_t EM_TEMP_NUM_SENSORS;
	uint8_t EM_TEMP_MIN_TEMP;
	uint8_t EM_TEMP_MAX_TEMP;
	uint8_t EM_TEMP_5N;
	uint8_t EM_TEMP_5N_PLUS_1;
	uint8_t EM_TEMP_5N_PLUS_2;
	uint8_t EM_TEMP_5N_PLUS_3;
	uint8_t EM_TEMP_5N_PLUS_4;
	// Team Data
	int32_t EM_TEAMDATA_TEAMSIGNAL1;
	int32_t EM_TEANDATA_TEANSIGNAL2;

} CCU_StateData;

// Liable to change V
typedef struct {
	ccu_state_t state;
	ccu_fault_t fault;

	ccu_charger_control_t in;
	ccu_charger_data_t out;

	// Timing cause I feel like thats need
	uint32_t ms_in_state;
	// uint32_t last_bcu_message; (original intent unneeded but heard
	// something similar might be implemented, some ping thing)

} ccu_t;

void ccu_init(ccu_t *ccu);
void ccu_set_inputs(ccu_t *ccu, const ccu_charger_control_t *in);
void ccu_tick(
    ccu_t *ccu, uint32_t now_ms,
    uint32_t delta_ms); // now_ms = curr time, delta_ms = time since last