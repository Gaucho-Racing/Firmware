#include <stdint.h>

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
/**
 * @brief Represents the various states of the ECU state machine.
 *
 * Each state corresponds to a specific operational mode of the ECU.
 * See the state diagram in the documentation for more details.
 *
 * Maps to the enum `GR_ECU_State` with the same name, used for `uint8_t`
 * backing.
 */
typedef uint8_t GR_ECU_State;

enum GR_ECU_State {
	/**
	 * The GLV is off and not operational (not possible if ECU is running)
	 */
	GR_GLV_OFF = __extension__ 0b00000001,
	/**
	 * The ECU is on and ready for operation.
	 */
	GR_GLV_ON = __extension__ 0b00000010,
	/**
	 * The HV precharging process has been initiated.
	 */
	GR_PRECHARGE_ENGAGED = __extension__ 0b00000100,
	/**
	 * The HV precharging process has been completed successfully.
	 */
	GR_PRECHARGE_COMPLETE = __extension__ 0b00001000,
	/**
	 * The HV system is fully operational and the drive system is active.
	 */
	GR_DRIVE_ACTIVE = __extension__ 0b00010000,
	/**
	 * The HV system is in the process of discharging, TS Voltage >60V.
	 */
	GR_TS_DISCHARGE = __extension__ 0b00100000,
};
#endif
