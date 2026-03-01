// ccu_state_test.c
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint8_t CCU_STATE;

enum {
	CCU_STATE_IDLE = 0,
	CCU_STATE_CHARGING = 1
};

typedef struct {
	bool Button_Status;

	// Make this large enough for real bit positions.
	uint64_t ACU_S2_ERROR_BITS;

	bool ACU_S2_OVERTEMP_ERROR;
	bool ACU_S2_OVERVOLT_ERROR;
	bool ACU_S2_UNDERVOLT_ERROR;
	bool ACU_S2_OVERCURR_ERROR;
	bool ACU_S2_UNDERCURR_ERROR;
} CCU_StateData;

// bit index
#define CHECK_BIT(value, bit) (((value) >> (bit)) & 1ULL)

// ---- state functions ----
static void update_error_flags(CCU_StateData *sd)
{
	sd->ACU_S2_OVERTEMP_ERROR = CHECK_BIT(sd->ACU_S2_ERROR_BITS, 40);
	sd->ACU_S2_OVERVOLT_ERROR = CHECK_BIT(sd->ACU_S2_ERROR_BITS, 41);
	sd->ACU_S2_UNDERVOLT_ERROR = CHECK_BIT(sd->ACU_S2_ERROR_BITS, 42);
	sd->ACU_S2_OVERCURR_ERROR = CHECK_BIT(sd->ACU_S2_ERROR_BITS, 43);
	sd->ACU_S2_UNDERCURR_ERROR = CHECK_BIT(sd->ACU_S2_ERROR_BITS, 44);
}

static bool any_errors(const CCU_StateData *sd)
{
	return sd->ACU_S2_OVERTEMP_ERROR || sd->ACU_S2_OVERVOLT_ERROR || sd->ACU_S2_UNDERVOLT_ERROR || sd->ACU_S2_OVERCURR_ERROR || sd->ACU_S2_UNDERCURR_ERROR;
}

static CCU_STATE STATE_IDLE(CCU_StateData *sd, CCU_STATE state)
{
	update_error_flags(sd);

	if (!any_errors(sd) && sd->recv_charge_cmd) {
		return CCU_STATE_CHARGING;
	}
	return state;
}

static CCU_STATE STATE_CHARGING(CCU_StateData *sd, CCU_STATE state)
{
	update_error_flags(sd);

	// stop OR fault
	if (!sd->recv_charge_cmd || any_errors(sd)) {
		return CCU_STATE_IDLE;
	}
	return state;
}

static CCU_STATE CCU_State_Tick(CCU_StateData *sd, CCU_STATE state)
{
	switch (state) {
		case CCU_STATE_IDLE:
			return STATE_IDLE(sd, state);
		case CCU_STATE_CHARGING:
			return STATE_CHARGING(sd, state);
		default:
			return CCU_STATE_IDLE;
	}
}

static const char *state_name(CCU_STATE s)
{
	return (s == CCU_STATE_CHARGING) ? "CHARGING" : "IDLE";
}

// ---- test scenarios ----
int main(void)
{
	CCU_StateData sd;
	memset(&sd, 0, sizeof(sd));

	CCU_STATE state = CCU_STATE_IDLE;

	printf("Initial state: %s\n", state_name(state));

	// 1) Press button, no errors -> should enter CHARGING
	sd.recv_charge_cmd = true;
	sd.ACU_S2_ERROR_BITS = 0;
	state = CCU_State_Tick(&sd, state);
	printf("After button ON, no errors: %s\n", state_name(state));

	// 2) Inject an error bit -> should go back to IDLE
	sd.ACU_S2_ERROR_BITS = (1ULL << 42); // undervolt
	state = CCU_State_Tick(&sd, state);
	printf("After undervolt fault: %s\n", state_name(state));

	// 3) Clear error, press button again -> should go CHARGING
	sd.ACU_S2_ERROR_BITS = 0;
	sd.recv_charge_cmd = true;
	state = CCU_State_Tick(&sd, state);
	printf("After clear fault + button ON: %s\n", state_name(state));

	// 4) Turn button off -> should go IDLE
	sd.recv_charge_cmd = false;
	state = CCU_State_Tick(&sd, state);
	printf("After button OFF: %s\n", state_name(state));

	return 0;
}
