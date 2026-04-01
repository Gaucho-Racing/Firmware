#include "StateUtils.h"

#include "CANDler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"

void setSoftwareLatch(bool close, CCU_StateData *state_data)
{

	if (close && !LL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) {
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
		state_data->BCU_S2_SOFTWARE_LATCH = 1;
		LOGOMATIC("Software Latch: High\n");
	} else if (!close && LL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) {
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
		state_data->BCU_S2_SOFTWARE_LATCH = 0;
		LOGOMATIC("Software Latch: Low\n");
	}
}

bool BCU_Warnings(const CCU_StateData *state_data)
{
	if (state_data->BCU_S2_UNDER20v_WARNING) {
		LOGOMATIC("Under 20v Warning\n");
		return true;
	} else if (state_data->BCU_S2_UNDER12v_WARNING) {
		LOGOMATIC("Under 12v Warning\n");
		return true;
	} else if (state_data->BCU_S2_UNDERVOLTSDC_WARNING) {
		LOGOMATIC("Undervolt TSDC Wanring\n");
		return true;
	} else {
		return false;
	}
}

bool CriticalError(const CCU_StateData *state_data)
{

	if (state_data->BCU_S2_OVERCURR_ERROR) {
		LOGOMATIC("OVERCURR\n");
		return true;

	} else if (state_data->BCU_S2_OVERTEMP_ERROR) {
		LOGOMATIC("OVERTEMP\n");
		return true;

	} else if (state_data->BCU_S2_OVERVOLT_ERROR) {
		LOGOMATIC("OVERVOLT\n");
		return true;

	} else if (state_data->BCU_S2_UNDERCURR_ERROR) {
		LOGOMATIC("UNDECURR\n");
		return true;

	} else if (state_data->BCU_S2_UNDERVOLT_ERROR) {
		LOGOMATIC("UNDEVOLT\n");
		return true;

	} else {
		return false;
	}
}

bool SecondaryChecks(const CCU_StateData *state_data)
{

	// TODO: Implement secondary checks based of rules (This may be a different file entirely)
	// FIXME: Double check what that actually looks like and means

	/** Check 1:
	 * TS Voltage Discharge
	 *
	 * TS voltage must decrease below 60VDC in 5 sec.
	 * Allow team to set pace of tests to prevent overheating precharge/discharge resistors.
	 */

	/** Check 2:
	 * Maximum Power/Voltage Violation
	 *
	 *EV.3.4.1 A Violation occurs when one or two of these exist:
		a. Use of more than the specified maximum power EV.3.3.1
		b. Exceed the maximum voltage EV.3.3.2
		for one or the two conditions:
		• Continuously for 100 ms or more
		• After a moving average over 500 ms is applied
	 */
}
