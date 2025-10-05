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
         * The HV precharging process is underway.
         */
	    GR_PRECHARGING = 3,
        /**
         * The HV precharging process has been completed successfully.
         */
	    GR_PRECHARGE_COMPLETE = 4,
        /**
         * The vehicle is in drive standby mode, ready to transition to active states.
         */
	    GR_DRIVE_STANDBY = 5,
        /**
         * The vehicle is in active idle mode, maintaining readiness without significant power draw.
         */
	    GR_DRIVE_ACTIVE_IDLE = 6,
        /**
         * The vehicle is in active power mode, delivering power to the drivetrain.
         */
	    GR_DRIVE_ACTIVE_POWER = 7,
        /**
         * The vehicle is in active regenerative braking mode, recovering energy.
         */
	    GR_DRIVE_ACTIVE_REGEN = 8,
        /**
         * The HV system is in the process of discharging, TS Voltage >60V.
         */
	    GR_TS_DISCHARGE_OFF = 9,
        /**
         * An error has occurred, will handle discharge if necessary.
         */
	    GR_ERROR = 10,
    } GR_ECU_State;
#endif
