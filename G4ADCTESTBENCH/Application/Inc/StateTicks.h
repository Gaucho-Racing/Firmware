#include "StateData.h"
#include "StateMachine.h"
#include "adc.h"
#include "can.h"

#ifndef _STATE_TICKS_H_
#define _STATE_TICKS_H_

extern CANHandle *primary_can;
extern CANHandle *data_can;

/**
 * @brief Tick function for the ECU state machine.
 *
 * Calls the appropriate state handler based on the current state.
 *
 * @return void
 */
void ECU_State_Tick(void);

/**
 * @brief State handler for the GLV Off state.
 *
 * Handles actions and transitions specific to the GLV Off state.
 *
 * @param stateData Pointer to the ECU state data structure.
 *
 * @return void
 */
void ECU_GLV_Off(ECU_StateData *stateData);

/**
 * @brief State handler for the GLV On state.
 *
 * Handles actions and transitions specific to the GLV On state.
 *
 * @param stateData Pointer to the ECU state data structure.
 *
 * @return void
 */
void ECU_GLV_On(ECU_StateData *stateData);

/**
 * @brief Handles the transition from GLV On to Precharge Engaged state.
 *
 * Initiates the precharge process by switching to the Precharge Engaged state.
 *
 * @param stateData Pointer to the ECU state data structure.
 *
 * @return void
 */
void ECU_Precharge_Start(ECU_StateData *stateData);

/**
 * @brief State handler for the Precharge Engaged state.
 *
 * Handles actions and transitions specific to the Precharge Engaged state.
 *
 * @param stateData Pointer to the ECU state data structure.
 *
 * @return void
 */
void ECU_Precharge_Engaged(ECU_StateData *stateData);

/**
 * @brief State handler for the Precharge Complete state.
 *
 * Handles actions and transitions specific to the Precharge Complete state.
 *
 * @param stateData Pointer to the ECU state data structure.
 *
 * @return void
 */
void ECU_Precharge_Complete(ECU_StateData *stateData);

/**
 * @brief State handler for the Precharge Fault state.
 *
 * Handles actions and transitions specific to the Precharge Fault state.
 *
 * @param stateData Pointer to the ECU state data structure.
 *
 * @return void
 */
void ECU_Drive_Active(ECU_StateData *stateData);

/**
 * @brief Init function for ECU_Tractive_System_Discharge_Start.
 *
 * Resets Tractive System discharge timer and switches on the Tractive System
 * Discharge state.
 *
 * @param stateData Pointer to the ECU state data structure.
 *
 * @return void
 */
void ECU_Tractive_System_Discharge_Start(ECU_StateData *stateData);

/**
 * @brief State handler for the Tractive System Discharge state.
 *
 * Handles actions and transitions specific to the Tractive System Discharge
 * state.
 *
 * @param stateData Pointer to the ECU state data structure.
 *
 * @return void
 */
void ECU_Tractive_System_Discharge(ECU_StateData *stateData);

#endif
