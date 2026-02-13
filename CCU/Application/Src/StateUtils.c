#include "StateUtils.h"

#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"

void setSoftwareLatch(bool close)
{

	if (close && !(HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))) {
		HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
		LOGOMATIC("Software Latch: High");
	} else if (!(close) && HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) {
		HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
		LOGOMATIC("Software Latch: Low");
	}
}

bool CriticalError(CCU_StateData* state_data){

	if (state_data->ACU_S2_OVERCURR_ERROR){
		setSoftwareLatch(0);
		return true;

	} else if (state_data->ACU_S2_OVERTEMP_ERROR){
		setSoftwareLatch(0);
		return true;

	} else if (state_data->ACU_S2_OVERVOLT_ERROR){
		setSoftwareLatch(0);
		return true;

	} else if (state_data->ACU_S2_UNDERCURR_ERROR){
		setSoftwareLatch(0);
		return true;

	} else if (state_data->ACU_S2_UNDERVOLT_ERROR){
		setSoftwareLatch(0);
		return true;

	} else {
		return false;
		
	}


}
