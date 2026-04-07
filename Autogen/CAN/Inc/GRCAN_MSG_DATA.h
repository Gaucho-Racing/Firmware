/* Auto-generated header file */
#ifndef GR_OLD_MESSAGES_H
#define GR_OLD_MESSAGES_H

#include <stdint.h>

/** Ping */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint32_t timestamp;
} GR_OLD_PING_MSG;

/** ECU Status 1 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t state_messages;
	/** Byte 1 (Byte 1) */
	uint8_t status_flags;
	/** Byte 2 (Byte 2) */
	uint8_t power_level_torque_map;
	/** Byte 3 (Byte 3) */
	uint8_t max_cell_temp;
	/** Byte 4 (Byte 4) */
	uint8_t accumulator_state_of_charge;
	/** Byte 5 (Byte 5) */
	uint8_t glv_state_of_charge;
	/** Byte 6 (Byte 6) */
	uint16_t tractive_system_voltage;
} GR_OLD_ECU_STATUS_1_MSG;

/** ECU Status 2 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t vehicle_speed;
	/** Byte 2 (Byte 2) */
	uint16_t fr_wheel_rpm;
	/** Byte 4 (Byte 4) */
	uint16_t fl_wheel_rpm;
	/** Byte 6 (Byte 6) */
	uint16_t rr_wheel_rpm;
} GR_OLD_ECU_STATUS_2_MSG;

/** ECU Status 3 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t rl_wheel_rpm;
} GR_OLD_ECU_STATUS_3_MSG;

/** ECU config */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t reserved;
} GR_OLD_ECU_CONFIG_MSG;

/** BCU Status 1 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t accumulator_voltage;
	/** Byte 2 (Byte 2) */
	uint16_t ts_voltage;
	/** Byte 4 (Byte 4) */
	uint16_t accumulator_current;
	/** Byte 6 (Byte 6) */
	uint8_t accumulator_soc;
	/** Byte 7 (Byte 7) */
	uint8_t glv_soc;
} GR_OLD_BCU_STATUS_1_MSG;

/** BCU Status 2 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t _20v_voltage;
	/** Byte 1 (Byte 1) */
	uint8_t _12v_voltage;
	/** Byte 2 (Byte 2) */
	uint8_t sdc_voltage;
	/** Byte 3 (Byte 3) */
	uint8_t min_cell_voltage;
	/** Byte 4 (Byte 4) */
	uint8_t max_cell_temp;
	/** Byte 5 (Byte 5) */
	uint8_t status_flags;
	/** Byte 6 (Byte 6) */
	uint8_t precharge_latch_flags;
} GR_OLD_BCU_STATUS_2_MSG;

/** BCU Status 3 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t hv_input_voltage;
	/** Byte 2 (Byte 2) */
	uint16_t hv_output_voltage;
	/** Byte 4 (Byte 4) */
	uint16_t hv_input_current;
	/** Byte 6 (Byte 6) */
	uint16_t hv_output_current;
} GR_OLD_BCU_STATUS_3_MSG;

/** BCU Precharge */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t set_ts_active;
} GR_OLD_BCU_PRECHARGE_MSG;

/** BCU Config Charge Parameters */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t charge_voltage;
	/** Byte 2 (Byte 2) */
	uint16_t charge_current;
} GR_OLD_BCU_CONFIG_CHARGE_PARAMETERS_MSG;

/** BCU Config Operational Parameters */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t minimium_cell_voltage;
	/** Byte 1 (Byte 1) */
	uint8_t max_cell_temperature;
} GR_OLD_BCU_CONFIG_OPERATIONAL_PARAMETERS_MSG;

/** BCU Cell Data 1 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_BCU_CELL_DATA_1_MSG;

/** BCU Cell Data 2 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_BCU_CELL_DATA_2_MSG;

/** BCU Cell Data 3 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_BCU_CELL_DATA_3_MSG;

/** BCU Cell Data 4 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_BCU_CELL_DATA_4_MSG;

/** BCU Cell Data 5 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_BCU_CELL_DATA_5_MSG;

/** DC-DC Status */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t input_voltage;
	/** Byte 2 (Byte 2) */
	uint16_t output_voltage;
	/** Byte 4 (Byte 4) */
	uint8_t input_current;
	/** Byte 5 (Byte 5) */
	uint8_t output_current;
	/** Byte 6 (Byte 6) */
	uint8_t dc_dc_temp;
} GR_OLD_DC_DC_STATUS_MSG;

/** Inverter Status 1 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t ac_current;
	/** Byte 2 (Byte 2) */
	uint16_t dc_current;
	/** Byte 4 (Byte 4) */
	uint16_t motor_rpm;
} GR_OLD_INVERTER_STATUS_1_MSG;

/** Inverter Status 2 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t u_mosfet_temperature;
	/** Byte 2 (Byte 2) */
	uint8_t v_mosfet_temperature;
	/** Byte 4 (Byte 4) */
	uint8_t w_mosfet_temperature;
} GR_OLD_INVERTER_STATUS_2_MSG;

/** Inverter Status 3 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t motor_temperature;
	uint8_t error_fault_violation_bits;
} GR_OLD_INVERTER_STATUS_3_MSG;

/** Inverter Config */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t max_ac_current;
	/** Byte 2 (Byte 2) */
	uint16_t max_dc_current;
	/** Byte 4 (Byte 4) */
	uint16_t absolute_max_rpm_limit;
	/** Byte 6 (Byte 6) */
	uint8_t motor_direction;
} GR_OLD_INVERTER_CONFIG_MSG;

/** Inverter Command */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t set_ac_current;
	/** Byte 2 (Byte 2) */
	uint16_t set_dc_current;
	/** Byte 4 (Byte 4) */
	uint16_t rpm_limit;
	/** Byte 6 (Byte 6) */
	uint8_t field_weakening;
	/** Byte 7 (Byte 7) */
	uint8_t drive_enable;
} GR_OLD_INVERTER_COMMAND_MSG;

/** Fan Status */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t fan_speed;
	/** Byte 2 (Byte 2) */
	uint8_t input_voltage;
	/** Byte 3 (Byte 3) */
	uint8_t input_current;
} GR_OLD_FAN_STATUS_MSG;

/** Fan Command */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t fan_command;
} GR_OLD_FAN_COMMAND_MSG;

/** Dash Status */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t button_flags;
	/** Byte 1 (Byte 1) */
	uint8_t led_bits;
} GR_OLD_DASH_STATUS_MSG;

/** Dash Config */
typedef struct {
	uint8_t ping_block;
} GR_OLD_DASH_CONFIG_MSG;

/** Steering Status */
typedef struct {
	/** Byte 0 (Byte 0) */
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
	/** Byte 0 (Byte 0) */
	uint8_t temp;
} GR_OLD_SAM_BRAKE_IR_MSG;

/** SAM Tire Temp */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t outside_temp;
	/** Byte 1 (Byte 1) */
	uint8_t outside_middle_temp;
	/** Byte 2 (Byte 2) */
	uint8_t inside_middle_temp;
	/** Byte 3 (Byte 3) */
	uint8_t inside_temp;
} GR_OLD_SAM_TIRE_TEMP_MSG;

/** SAM IMU */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t accel_x;
	/** Byte 2 (Byte 2) */
	uint16_t accel_y;
	/** Byte 4 (Byte 4) */
	uint16_t accel_z;
	/** Byte 6 (Byte 6) */
	uint16_t gyro_x;
	/** Byte 8 (Byte 8) */
	uint16_t gyro_y;
	/** Byte 10 (Byte 10) */
	uint16_t gyro_z;
} GR_OLD_SAM_IMU_MSG;

/** SAM GPS 1 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint32_t latitude;
	/** Byte 4 (Byte 4) */
	uint32_t longitude;
} GR_OLD_SAM_GPS_1_MSG;

/** SAM GPS 2 */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint32_t accuracy;
	/** Byte 4 (Byte 4) */
	uint32_t attitude;
} GR_OLD_SAM_GPS_2_MSG;

/** SAM GPS Time */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint32_t time;
	/** Byte 4 (Byte 4) */
	uint32_t time_of_week_ms;
} GR_OLD_SAM_GPS_TIME_MSG;

/** SAM GPS Heading */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint32_t heading_from_north;
} GR_OLD_SAM_GPS_HEADING_MSG;

/** SAM Sus Pots */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t suspension_angle;
} GR_OLD_SAM_SUS_POTS_MSG;

/** SAM TOF */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t height;
} GR_OLD_SAM_TOF_MSG;

/** SAM Rear Wheelspeed */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t speed;
} GR_OLD_SAM_REAR_WHEELSPEED_MSG;

/** SAM Pushrod Force */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t load_force;
} GR_OLD_SAM_PUSHROD_FORCE_MSG;

/** TCM Status */
typedef struct {
	uint8_t ping_block;
	/** Byte 1 (Byte 1) */
	uint16_t ping;
	/** Byte 3 (Byte 3) */
	uint32_t cache_size;
	/** Byte 7 (Byte 7) */
	uint8_t reserved;
} GR_OLD_TCM_STATUS_MSG;

/** TCM Resource Utilization */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t cpu_0_freq;
	/** Byte 2 (Byte 2) */
	uint8_t cpu_0_util;
	/** Byte 3 (Byte 3) */
	uint16_t cpu_1_freq;
	/** Byte 5 (Byte 5) */
	uint8_t cpu_1_util;
	/** Byte 6 (Byte 6) */
	uint16_t cpu_2_freq;
	/** Byte 8 (Byte 8) */
	uint8_t cpu_2_util;
	/** Byte 9 (Byte 9) */
	uint16_t cpu_3_freq;
	/** Byte 11 (Byte 11) */
	uint8_t cpu_3_util;
	/** Byte 12 (Byte 12) */
	uint16_t cpu_4_freq;
	/** Byte 14 (Byte 14) */
	uint8_t cpu_4_util;
	/** Byte 15 (Byte 15) */
	uint16_t cpu_5_freq;
	/** Byte 17 (Byte 17) */
	uint8_t cpu_5_util;
	/** Byte 18 (Byte 18) */
	uint8_t cpu_total_util;
	/** Byte 19 (Byte 19) */
	uint16_t ram_total;
	/** Byte 21 (Byte 21) */
	uint16_t ram_used;
	/** Byte 23 (Byte 23) */
	uint8_t ram_util;
	/** Byte 24 (Byte 24) */
	uint8_t gpu_util;
	/** Byte 25 (Byte 25) */
	uint16_t gpu_freq;
	/** Byte 27 (Byte 27) */
	uint32_t disk_total;
	/** Byte 31 (Byte 31) */
	uint32_t disk_used;
	/** Byte 35 (Byte 35) */
	uint8_t disk_util;
	/** Byte 36 (Byte 36) */
	uint8_t cpu_temp;
	/** Byte 37 (Byte 37) */
	uint8_t gpu_temp;
	/** Byte 38 (Byte 38) */
	uint16_t voltage_draw;
	/** Byte 40 (Byte 40) */
	uint16_t current_draw;
	/** Byte 42 (Byte 42) */
	uint16_t power_draw;
} GR_OLD_TCM_RESOURCE_UTILIZATION_MSG;

/** Dash Warning Flags */
typedef struct {
	uint8_t error_fault_violation_bits;
} GR_OLD_DASH_WARNING_FLAGS_MSG;

/** Specific Brake IR */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t wheel_identifier;
	/** Byte 1 (Byte 1) */
	uint8_t temp;
} GR_OLD_SPECIFIC_BRAKE_IR_MSG;

/** ECU Ping Information */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t online_pings;
} GR_OLD_ECU_PING_INFORMATION_MSG;

/** ECU Pedals Data */
typedef struct {
	/** 4-20 mA signal (Byte 0) */
	uint16_t bspd_signal;
	/** 4-20 mA signal (Byte 2) */
	uint16_t bse_signal;
	/** 4-20 mA signal (Byte 4) */
	uint16_t apps_1_signal;
	/** 4-20 mA signal (Byte 6) */
	uint16_t apps_2_signal;
	/** 4-20 mA signal (Byte 8) */
	uint16_t brakeline_f_signal;
	/** 4-20 mA signal (Byte 10) */
	uint16_t brakeline_r_signal;
	/** 4-20 mA signal (Byte 12) */
	uint16_t steering_angle_signal;
	/** 4-20 mA signal (Byte 14) */
	uint16_t aux_signal;
} GRCAN_ECU_ANALOG_DATA_MSG;

/** GPS LAT */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t lat;
} GR_OLD_GPS_LAT_MSG;

/** GPS LON */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t lon;
} GR_OLD_GPS_LON_MSG;

/** GPS ALT */
typedef struct {
	/** Byte 0 (Byte 0) */
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
	/** Byte 0 (Byte 0) */
	uint16_t dgps_u;
	/** Byte 2 (Byte 2) */
	uint16_t dgps_v;
	/** Byte 4 (Byte 4) */
	uint16_t dgps_w;
} GR_OLD_UVW_DGPS_MSG;

/** ECU Performance */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t elapsed_cycles;
} GR_OLD_ECU_PERFORMANCE_MSG;

/** ECU Performance */
typedef struct {
	/**
	 * Represents the total number of clock cycles elapsed for 10 iterations of the main loop
	 * data type: u32
	 * units: Clock Cycles (Byte 0) */
	uint32_t elapsed_cycles;
} GRCAN_ECU_PERFORMANCE_MSG;

#endif
