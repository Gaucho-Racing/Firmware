/* Auto-generated header file */
#ifndef GR_OLD_MESSAGES_H
#define GR_OLD_MESSAGES_H

#include <stdint.h>

/** Debug 2.0 */
typedef struct {
	/** Essentially a print statement up to 64 bytes long that whichever targeted can parse
 (Byte 0) */
	uint8_t debug;
} GR_OLD_DEBUG_2_0_MSG;

/** Debug FD */
typedef struct {
	/** Essentially a print statement up to 64 bytes long that whichever targeted can parse
 (Byte 0) */
	uint8_t debug;
} GR_OLD_DEBUG_FD_MSG;

/** Ping */
typedef struct {
	/** Time in millis
 (Byte 0) */
	uint32_t timestamp;
} GR_OLD_PING_MSG;

// Removed as being broken in `main` is not allowed

/** ECU Status 2 */
typedef struct {
	/** Output terminal voltage of accumulator
 (Byte 0) */
	uint16_t tractive_system_voltage;
	/** Absolute value of speed
 (Byte 2) */
	uint16_t vehicle_speed;
	/** Wheel RPM
 (Byte 4) */
	uint16_t fr_wheel_rpm;
	/** Wheel RPM
 (Byte 6) */
	uint16_t fl_wheel_rpm;
} GR_OLD_ECU_STATUS_2_MSG;

/** ECU Status 3 */
typedef struct {
	/** Wheel RPM
 (Byte 0) */
	uint16_t rr_wheel_rpm;
	/** Wheel RPM
 (Byte 2) */
	uint16_t rl_wheel_rpm;
} GR_OLD_ECU_STATUS_3_MSG;

/** ECU config */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t reserved;
} GR_OLD_ECU_CONFIG_MSG;

/** ACU Status 1 */
typedef struct {
	/** All cell voltages added up
 (Byte 0) */
	uint16_t accumulator_voltage;
	/** Output terminal voltage of accumulator
 (Byte 2) */
	uint16_t ts_voltage;
	/** Current output of accumulator
 (Byte 4) */
	uint16_t accumulator_current;
	/** Accumulator state of charge (Based on lowest cell)
 (Byte 6) */
	uint8_t accumulator_soc;
	/** GLV state of charge
 (Byte 7) */
	uint8_t glv_soc;
} GR_OLD_ACU_STATUS_1_MSG;

// Removed as being broken in `main` is not allowed

/** ACU Status 3 */
typedef struct {
	/** 600v input voltage
 (Byte 0) */
	uint16_t hv_input_voltage;
	/** 20v output voltage
 (Byte 2) */
	uint16_t hv_output_voltage;
	/** 600v input current
 (Byte 4) */
	uint16_t hv_input_current;
	/** 20v output current
 (Byte 6) */
	uint16_t hv_output_current;
} GR_OLD_ACU_STATUS_3_MSG;

/** ACU Precharge */
typedef struct {
	/** 0: shutdown, 1: go TS Active/Precharge
 (Byte 0) */
	uint8_t set_ts_active;
} GR_OLD_ACU_PRECHARGE_MSG;

/** ACU Config Charge Parameters */
typedef struct {
	/** Sets the Target Charging voltage
 (Byte 0) */
	uint16_t charge_voltage;
	/** Sets the Target Charging Current
 (Byte 2) */
	uint16_t charge_current;
} GR_OLD_ACU_CONFIG_CHARGE_PARAMETERS_MSG;

/** ACU Config Operational Parameters */
typedef struct {
	/** Sets the threshold for Minimum Cell Voltage before Shutdown
 (Byte 0) */
	uint8_t minimium_cell_voltage;
	/** Sets the threshold for Max Cell Temperature before Shutdown
 (Byte 1) */
	uint8_t max_cell_temperature;
} GR_OLD_ACU_CONFIG_OPERATIONAL_PARAMETERS_MSG;

/** ACU Cell Data 1 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_CELL_DATA_1_MSG;

/** ACU Cell Data 2 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_CELL_DATA_2_MSG;

/** ACU Cell Data 3 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_CELL_DATA_3_MSG;

/** ACU Cell Data 4 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_CELL_DATA_4_MSG;

/** ACU Cell Data 5 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_CELL_DATA_5_MSG;

/** DC-DC Status */
typedef struct {
	/** 0-22
 (Byte 0) */
	uint16_t input_voltage;
	/** ~12v for LV and ~20v for HV
 (Byte 2) */
	uint16_t output_voltage;
	/** 0-10
 (Byte 4) */
	uint8_t input_current;
	/** Output current
 (Byte 5) */
	uint8_t output_current;
	/** Temp of DC-DC converter
 (Byte 6) */
	uint8_t dc_dc_temp;
} GR_OLD_DC_DC_STATUS_MSG;

/** Inverter Status 1 */
typedef struct {
	/** 0.01 * current, int16_t
 (Byte 0) */
	uint16_t ac_current;
	/** 0.01 * current, int16_t
 (Byte 2) */
	uint16_t dc_current;
	/** RPM, int16_t
 (Byte 4) */
	uint16_t motor_rpm;
} GR_OLD_INVERTER_STATUS_1_MSG;

/** Inverter Status 2 */
typedef struct {
	/** Celsius + 40, uint8_t
 (Byte 0) */
	uint8_t u_mosfet_temperature;
	/** Celsius + 40, uint8_t
 (Byte 2) */
	uint8_t v_mosfet_temperature;
	/** Celsius + 40, uint8_t
 (Byte 4) */
	uint8_t w_mosfet_temperature;
} GR_OLD_INVERTER_STATUS_2_MSG;

/** Inverter Status 3 */
typedef struct {
	/** Celsius + 40, uint8_t
 (Byte 0) */
	uint8_t motor_temperature;
	uint8_t error_fault_violation_bits;
} GR_OLD_INVERTER_STATUS_3_MSG;

/** Inverter Config */
typedef struct {
	/** Max AC Current
 (Byte 0) */
	uint16_t max_ac_current;
	/** Max DC Current
 (Byte 2) */
	uint16_t max_dc_current;
	/** 0: No limit      n :limited at n RPM
 (Byte 4) */
	uint16_t absolute_max_rpm_limit;
	/** Write 1 inverts direction
 (Byte 6) */
	uint8_t motor_direction;
} GR_OLD_INVERTER_CONFIG_MSG;

/** Inverter Command */
typedef struct {
	/** Commanded AC Current
 (Byte 0) */
	uint16_t set_ac_current;
	/** Commanded DC Current
 (Byte 2) */
	uint16_t set_dc_current;
	/** 0: No limit      n :limited at n RPM
 (Byte 4) */
	uint16_t rpm_limit;
	/** Field weakening strength
 (Byte 6) */
	uint8_t field_weakening;
	/** Write this to 1 every 100ms to enable inverter
 (Byte 7) */
	uint8_t drive_enable;
} GR_OLD_INVERTER_COMMAND_MSG;

/** Fan Status */
typedef struct {
	/** Fan RPM
 (Byte 0) */
	uint16_t fan_speed;
	/** 0-22
 (Byte 2) */
	uint8_t input_voltage;
	/** 0-10
 (Byte 3) */
	uint8_t input_current;
} GR_OLD_FAN_STATUS_MSG;

/** Fan Command */
typedef struct {
	/** 0-100 Percent
 (Byte 0) */
	uint8_t fan_command;
} GR_OLD_FAN_COMMAND_MSG;

/** Dash Status */
typedef struct {
	uint8_t ping_block;
	/** MSB is state (1: pressed)
 Other 7 bits represent the time in 0.1s that it has been in that state
 (Byte 1) */
	uint8_t ts_button_data;
	/** MSB is state (1: pressed)
 Other 7 bits represent the time in 0.1s that it has been in that state
 (Byte 2) */
	uint8_t rtd_button_data;
} GR_OLD_DASH_STATUS_MSG;

/** Dash Config */
typedef struct {
	uint8_t ping_block;
} GR_OLD_DASH_CONFIG_MSG;

/** Steering Status */
typedef struct {
	/** Position of knob (1-16)
 Position of knob (1-16)
 (Byte 0) */
	uint8_t current_encoder_torque_map_encoder;
	uint8_t ping_block;
} GR_OLD_STEERING_STATUS_MSG;

/** Steering Config */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t reserved;
} GR_OLD_STEERING_CONFIG_MSG;

/** SAM Brake IR */
typedef struct {
	/** IR Temp of Brakes
 (Byte 0) */
	uint8_t temp;
} GR_OLD_SAM_BRAKE_IR_MSG;

/** SAM Tire Temp */
typedef struct {
	/** Furthest from chassis
 (Byte 0) */
	uint8_t outside_temp;
	/** Middle of tire
 (Byte 1) */
	uint8_t outside_middle_temp;
	/** Middle of tire
 (Byte 2) */
	uint8_t inside_middle_temp;
	/** Closest to chassis
 (Byte 3) */
	uint8_t inside_temp;
} GR_OLD_SAM_TIRE_TEMP_MSG;

/** SAM IMU */
typedef struct {
	/** Acceleration in X-axis
 (Byte 0) */
	uint16_t accel_x;
	/** Acceleration in Y-axis
 (Byte 2) */
	uint16_t accel_y;
	/** Acceleration in Z-axis
 (Byte 4) */
	uint16_t accel_z;
	/** Angular velocity in X-axis
 (Byte 6) */
	uint16_t gyro_x;
	/** Angular velocity in Y-axis
 (Byte 8) */
	uint16_t gyro_y;
	/** Angular velocity in Z-axis
 (Byte 10) */
	uint16_t gyro_z;
} GR_OLD_SAM_IMU_MSG;

/** SAM GPS 1 */
typedef struct {
	/** Latitude in decimal degrees
 (Byte 0) */
	uint32_t latitude;
	/** Longitude in decimal degrees
 (Byte 4) */
	uint32_t longitude;
} GR_OLD_SAM_GPS_1_MSG;

/** SAM GPS 2 */
typedef struct {
	/** GPS position accuracy
 (Byte 0) */
	uint32_t accuracy;
	/** Vehicle attitude
 (Byte 4) */
	uint32_t attitude;
} GR_OLD_SAM_GPS_2_MSG;

/** SAM GPS Time */
typedef struct {
	/** Time in seconds since GPS Epoch
 (Byte 0) */
	uint32_t time;
	/** Time of week in milliseconds
 (Byte 4) */
	uint32_t time_of_week_ms;
} GR_OLD_SAM_GPS_TIME_MSG;

/** SAM GPS Heading */
typedef struct {
	/** Heading angle relative to true North
 (Byte 0) */
	uint32_t heading_from_north;
} GR_OLD_SAM_GPS_HEADING_MSG;

/** SAM Sus Pots */
typedef struct {
	/** Pot Pos
 (Byte 0) */
	uint8_t suspension_angle;
} GR_OLD_SAM_SUS_POTS_MSG;

/** SAM TOF */
typedef struct {
	/** Ride Height
 (Byte 0) */
	uint16_t height;
} GR_OLD_SAM_TOF_MSG;

/** SAM Rear Wheelspeed */
typedef struct {
	/** Wheel RPM
 (Byte 0) */
	uint16_t speed;
} GR_OLD_SAM_REAR_WHEELSPEED_MSG;

/** SAM Pushrod Force */
typedef struct {
	/** Pushrod Force
 (Byte 0) */
	uint16_t load_force;
} GR_OLD_SAM_PUSHROD_FORCE_MSG;

/** TCM Status */
typedef struct {
	uint8_t ping_block;
	/** Mapache ping (upload)
 (Byte 1) */
	uint16_t ping;
	/** # of messages on cache (non-synced)
 (Byte 3) */
	uint32_t cache_size;
	/** Byte 7 (Byte 7) */
	uint8_t reserved;
} GR_OLD_TCM_STATUS_MSG;

/** TCM Resource Utilization */
typedef struct {
	/** core 0 frequency in MHz
 (Byte 0) */
	uint16_t cpu_0_freq;
	/** core 0 utilization in %
 (Byte 2) */
	uint8_t cpu_0_util;
	/** core 1 frequency in MHz
 (Byte 3) */
	uint16_t cpu_1_freq;
	/** core 1 utilization in %
 (Byte 5) */
	uint8_t cpu_1_util;
	/** core 2 frequency in MHz
 (Byte 6) */
	uint16_t cpu_2_freq;
	/** core 2 utilization in %
 (Byte 8) */
	uint8_t cpu_2_util;
	/** core 3 frequency in MHz
 (Byte 9) */
	uint16_t cpu_3_freq;
	/** core 3 utilization in %
 (Byte 11) */
	uint8_t cpu_3_util;
	/** core 4 frequency in MHz
 (Byte 12) */
	uint16_t cpu_4_freq;
	/** core 4 utilization in %
 (Byte 14) */
	uint8_t cpu_4_util;
	/** core 5 frequency in MHz
 (Byte 15) */
	uint16_t cpu_5_freq;
	/** core 5 utilization in %
 (Byte 17) */
	uint8_t cpu_5_util;
	/** total cpu utilization in %
 (Byte 18) */
	uint8_t cpu_total_util;
	/** total memory in MB
 (Byte 19) */
	uint16_t ram_total;
	/** used memory in MB
 (Byte 21) */
	uint16_t ram_used;
	/** memory utilization in %
 (Byte 23) */
	uint8_t ram_util;
	/** gpu utilization in %
 (Byte 24) */
	uint8_t gpu_util;
	/** gpu frequency in MHz
 (Byte 25) */
	uint16_t gpu_freq;
	/** total disk space in MB
 (Byte 27) */
	uint32_t disk_total;
	/** used disk space in MB
 (Byte 31) */
	uint32_t disk_used;
	/** disk utilization in %
 (Byte 35) */
	uint8_t disk_util;
	/** cpu temp in ˚C
 (Byte 36) */
	uint8_t cpu_temp;
	/** gpu temp in ˚C
 (Byte 37) */
	uint8_t gpu_temp;
	/** voltage draw in mV
 (Byte 38) */
	uint16_t voltage_draw;
	/** current draw in mA
 (Byte 40) */
	uint16_t current_draw;
	/** power draw in mW
 (Byte 42) */
	uint16_t power_draw;
} GR_OLD_TCM_RESOURCE_UTILIZATION_MSG;

/** Dash Warning Flags */
typedef struct {
	uint8_t error_fault_violation_bits;
} GR_OLD_DASH_WARNING_FLAGS_MSG;

/** Specific Brake IR */
typedef struct {
	/** Wheel identifier according to the wiki
 (Byte 0) */
	uint8_t wheel_identifier;
	/** IR Temp of Brakes
 (Byte 1) */
	uint8_t temp;
} GR_OLD_SPECIFIC_BRAKE_IR_MSG;

/** ECU Ping Information */
typedef struct {
	/** Literal copy of ECU Status's status bit map
 (Byte 0) */
	uint8_t online_pings;
} GR_OLD_ECU_PING_INFORMATION_MSG;

/** ECU Pedals Data */
typedef struct {
	/** APPS 1 Signal
 (Byte 0) */
	uint16_t apps1_signal;
	/** APPS 2 Signal
 (Byte 2) */
	uint16_t apps2_signal;
	/** Brake Force Signal
 (Byte 4) */
	uint16_t bse_signal;
	/** Brake Pressure Signal
 Brake Pressure Signal
 (Byte 6) */
	uint16_t brake_f_signal_brake_r_signal;
} GR_OLD_ECU_PEDALS_DATA_MSG;

/** GPS LAT */
typedef struct {
	/** lattitude
 (Byte 0) */
	uint8_t lat;
} GR_OLD_GPS_LAT_MSG;

/** GPS LON */
typedef struct {
	/** longitude
 (Byte 0) */
	uint8_t lon;
} GR_OLD_GPS_LON_MSG;

/** GPS ALT */
typedef struct {
	/** altitude
 (Byte 0) */
	uint8_t alt;
} GR_OLD_GPS_ALT_MSG;

/** GPS PX */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t theta;
	/** Byte 2 (Byte 2) */
	uint16_t acc;
	/** Byte 4 (Byte 4) */
	uint32_t status;
} GR_OLD_GPS_PX_MSG;

/** GPS QY */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t theta;
	/** Byte 2 (Byte 2) */
	uint16_t acc;
	/** Byte 4 (Byte 4) */
	uint32_t status;
} GR_OLD_GPS_QY_MSG;

/** GPS RZ */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t theta;
	/** Byte 2 (Byte 2) */
	uint16_t acc;
	/** Byte 4 (Byte 4) */
	uint32_t status;
} GR_OLD_GPS_RZ_MSG;

/** UVW DGPS */
typedef struct {
	/** U
 (Byte 0) */
	uint16_t dgps_u;
	/** V
 (Byte 2) */
	uint16_t dgps_v;
	/** W
 (Byte 4) */
	uint16_t dgps_w;
} GR_OLD_UVW_DGPS_MSG;

#endif
