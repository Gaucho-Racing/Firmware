#include <stdint.h>

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
/**
 * Enum representing the various states of the ECU state machine.
 * Each state corresponds to a specific operational mode of the ECU.
 * See the state diagram in the documentation for more details.
 */
typedef enum {
	/**
	 * The GLV is off and not operational (not possible if ECU is running)
	 */
	GR_GLV_OFF = 0,
	/**
	 * The ECU is on and ready for operation.
	 */
	GR_GLV_ON = 1,
	/**
	 * The HV precharging process has been initiated.
	 */
	GR_PRECHARGE_ENGAGED = 2,
	/**
	 * The HV precharging process has been completed successfully.
	 */
	GR_PRECHARGE_COMPLETE = 3,
	/**
	 * The HV system is fully operational and the drive system is active.
	 */
	GR_DRIVE_ACTIVE = 4,
	/**
	 * The HV system is in the process of discharging, TS Voltage >60V.
	 */
	GR_TS_DISCHARGE_OFF = 5,
} GR_ECU_State;
#endif
