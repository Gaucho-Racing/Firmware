// DO NOT CONSIDER THESE STABLE ... EXPECT URCA TO REPLACE THEM FULLY

#ifndef GR_OLD_BUS_ID_H
#define GR_OLD_BUS_ID_H

/** GR CAN Bus IDs */
typedef enum {
	/** Testing Bus */
	GR_OLD_BUS_TESTING = 0,
	/** Primary Bus */
	GR_OLD_BUS_PRIMARY = 1,
	/** Data Bus */
	GR_OLD_BUS_DATA = 2,
	/** Charging Cart and ACU Bus */
	GR_OLD_BUS_CHARGING = 3,
} GR_OLD_BUS_ID;

#endif
