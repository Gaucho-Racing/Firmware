/* Auto-generated header file */
#ifndef GRCAN_MESSAGES_H
#define GRCAN_MESSAGES_H

#include <stdint.h>

/** Ping */
typedef struct {
	/** Time in millis (Byte 0) */
	uint32_t timestamp;
} GRCAN_PING_MSG;

/** ECU Status 1 */
typedef struct {
	/** [Byte 0 / Bits 0-1] GLV States
0: GLV Off State,
1: GLV On State.
See diagram in StateMachine.
[Byte 0 / Bits 2-3] Precharge States
2: Precharge Engaged State
3: Precharge Complete State
See diagram in StateMachine.h
[Byte 0 / Bits 4-5] ECU States
4: Drive Active ECU State
5: TS Discharge ECU State
6-7: Reserved
See diagram in StateMachine.h (Byte 0) */
	uint8_t state_messages;
	/** [Byte 1 / Bits 8-15]
8: BCU Node Status (1: OK, 0: Timeout)
9: GR Inverter Status (1: OK, 0: Timeout)
10: Fan Controller 1 Status (1: OK, 0: Timeout)
11: Fan Controller 2 Status (1: OK, 0: Timeout)
12: Fan Controller 3 Status (1: OK, 0: Timeout)
13: Dash Panel Status (1: OK, 0: Timeout)
14: TCM Node Status (1: OK, 0: Timeout)
15: Reserved (Byte 1) */
	uint8_t status_flags;
	/** Controls the AC current limits to each of the inverters
Discrete Mapping, actual values TBD (16 possible values) The torque map selected; torque map is the mapping of the throttle to the torque sent to each motor (Byte 2) */
	uint8_t power_level_torque_map;
	/** the temperature of the hottest cell of the accumulator (Byte 3) */
	uint8_t max_cell_temp;
	/** % charged of the Accumulator (Byte 4) */
	uint8_t accumulator_state_of_charge;
	/** % charged of the Low Voltage Bat (Byte 5) */
	uint8_t glv_state_of_charge;
	/** Output terminal voltage of accumulator (Byte 6) */
	uint16_t tractive_system_voltage;
} GRCAN_ECU_STATUS_1_MSG;

/** ECU Status 2 */
typedef struct {
	/** Absolute value of speed (Byte 0) */
	uint16_t vehicle_speed;
	/** Wheel RPM (Byte 2) */
	uint16_t fr_wheel_rpm;
	/** Wheel RPM (Byte 4) */
	uint16_t fl_wheel_rpm;
	/** Wheel RPM (Byte 6) */
	uint16_t rr_wheel_rpm;
} GRCAN_ECU_STATUS_2_MSG;

/** ECU Status 3 */
typedef struct {
	/** Wheel RPM (Byte 0) */
	uint16_t rl_wheel_rpm;
} GRCAN_ECU_STATUS_3_MSG;

/** ECU config */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t reserved;
} GRCAN_ECU_CONFIG_MSG;

/** BCU Status 1 */
typedef struct {
	/** All cell voltages added up (Byte 0) */
	uint16_t accumulator_voltage;
	/** Output terminal voltage of accumulator (Byte 2) */
	uint16_t ts_voltage;
	/** Current output of accumulator (Byte 4) */
	uint16_t accumulator_current;
	/** Accumulator state of charge (Based on lowest cell) (Byte 6) */
	uint8_t accumulator_soc;
	/** GLV state of charge (Byte 7) */
	uint8_t glv_soc;
} GRCAN_BCU_STATUS_1_MSG;

/** BCU Status 2 */
typedef struct {
	/** 20v GLV voltage
data type: u8
units: Volts
scaled min: 0
scaled max: 25.5
map equation: "0.1x" (Byte 0) */
	uint8_t _20v_voltage;
	/** 12v supply voltage
data type: u8
units: Volts
scaled min: 0
scaled max: 25.5
map equation: "0.1x" (Byte 1) */
	uint8_t _12v_voltage;
	/** Voltage before BCU Latch
data type: u8
units: Volts
scaled min: 0
scaled max: 25.5
map equation: "0.1x" (Byte 2) */
	uint8_t sdc_voltage;
	/** Lowest cell voltage in accumulator
data type: u8
units: Volts
scaled min: 2
scaled max: 4.55
map equation: "0.01x+2" (Byte 3) */
	uint8_t min_cell_voltage;
	/** Hottest cell in accumulator
data type: u8
units: Celsius
scaled min: 0
scaled max: 63.75
map equation: "0.25x" (Byte 4) */
	uint8_t max_cell_temp;
	/** [Byte 5 / Bits 40-47]
40: Over Temp (>60C)
41: Over Voltage (>4.2V/cell)
42: Under Volt (<2.5V/cell)
43: Over Current (Discharge)
44: Under Current (Charge)
45: 20V GLV Warning
46: 12V Supply Warning
47: SDC Warning (Byte 5) */
	uint8_t status_flags;
	/** [Byte 6 / Bits 48-55]
48: Precharge Timeout
49: IR- / Precharge State (0:Open, 1:Closed)
50: IR+ State (0:Open, 1:Closed)
51: Software Latch (0:Open, 1:Closed)
52-55: Reserved (Byte 6) */
	uint8_t precharge_latch_flags;
} GRCAN_BCU_STATUS_2_MSG;

/** BCU Status 3 */
typedef struct {
	/** 600v input voltage (Byte 0) */
	uint16_t hv_input_voltage;
	/** 20v output voltage (Byte 2) */
	uint16_t hv_output_voltage;
	/** 600v input current (Byte 4) */
	uint16_t hv_input_current;
	/** 20v output current (Byte 6) */
	uint16_t hv_output_current;
} GRCAN_BCU_STATUS_3_MSG;

/** BCU Precharge */
typedef struct {
	/** 0: shutdown, 1: go TS Active/Precharge (Byte 0) */
	uint8_t set_ts_active;
} GRCAN_BCU_PRECHARGE_MSG;

/** BCU Config Charge Parameters */
typedef struct {
	/** Sets the Target Charging voltage (Byte 0) */
	uint16_t charge_voltage;
	/** Sets the Target Charging Current (Byte 2) */
	uint16_t charge_current;
} GRCAN_BCU_CONFIG_CHARGE_PARAMETERS_MSG;

/** BCU Config Operational Parameters */
typedef struct {
	/** Sets the threshold for Minimum Cell Voltage before Shutdown (Byte 0) */
	uint8_t minimium_cell_voltage;
	/** Sets the threshold for Max Cell Temperature before Shutdown (Byte 1) */
	uint8_t max_cell_temperature;
} GRCAN_BCU_CONFIG_OPERATIONAL_PARAMETERS_MSG;

/** BCU Cell Data 1 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_BCU_CELL_DATA_1_MSG;

/** BCU Cell Data 2 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_BCU_CELL_DATA_2_MSG;

/** BCU Cell Data 3 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_BCU_CELL_DATA_3_MSG;

/** BCU Cell Data 4 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_BCU_CELL_DATA_4_MSG;

/** BCU Cell Data 5 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_BCU_CELL_DATA_5_MSG;

/** DC-DC Status */
typedef struct {
	/** ~20v for LV (LV only. Send 0 for HV) (Byte 0) */
	uint16_t input_voltage;
	/** ~12v for LV and ~20v for HV (Byte 2) */
	uint16_t output_voltage;
	/** Input current (LV only. Send 0 for HV) (Byte 4) */
	uint8_t input_current;
	/** Output current (Byte 5) */
	uint8_t output_current;
	/** Temp of DC-DC converter (Byte 6) */
	uint8_t dc_dc_temp;
} GRCAN_DC_DC_STATUS_MSG;

/** Inverter Status 1 */
typedef struct {
	/** 0.01 * current, int16_t (Byte 0) */
	uint16_t ac_current;
	/** 0.01 * current, int16_t (Byte 2) */
	uint16_t dc_current;
	/** RPM, int16_t (Byte 4) */
	uint16_t motor_rpm;
} GRCAN_INVERTER_STATUS_1_MSG;

/** Inverter Status 2 */
typedef struct {
	/** Celsius + 40, uint8_t (Byte 0) */
	uint8_t u_mosfet_temperature;
	/** Celsius + 40, uint8_t (Byte 2) */
	uint8_t v_mosfet_temperature;
	/** Celsius + 40, uint8_t (Byte 4) */
	uint8_t w_mosfet_temperature;
} GRCAN_INVERTER_STATUS_2_MSG;

/** Inverter Status 3 */
typedef struct {
	/** Celsius + 40, uint8_t (Byte 0) */
	uint8_t motor_temperature;
	/** TS above set max voltage TS below set min voltage Inverter over set max temp Motor over set max temp Mosfet or mosfet drive error Encoder communication or calc error CAN message error or
	 * timeout (Byte 2) */
	uint8_t over_voltage_faults_under_voltage_fault_inv_overtemp_fault_motor_overtemp_fault_transistor_fault_encoder_fault_can_fault_future_use;
} GRCAN_INVERTER_STATUS_3_MSG;

/** Inverter Config */
typedef struct {
	/** Max AC Current (Byte 0) */
	uint16_t max_ac_current;
	/** Max DC Current (Byte 2) */
	uint16_t max_dc_current;
	/** 0: No limit      n :limited at n RPM (Byte 4) */
	uint16_t absolute_max_rpm_limit;
	/** Write 1 inverts direction (Byte 6) */
	uint8_t motor_direction;
} GRCAN_INVERTER_CONFIG_MSG;

/** Inverter Command */
typedef struct {
	/** Commanded AC Current (Byte 0) */
	uint16_t set_ac_current;
	/** Commanded DC Current (Byte 2) */
	uint16_t set_dc_current;
	/** 0: No limit      n :limited at n RPM (Byte 4) */
	uint16_t rpm_limit;
	/** Field weakening strength (Byte 6) */
	uint8_t field_weakening;
	/** Write this to 1 every 100ms to enable inverter (Byte 7) */
	uint8_t drive_enable;
} GRCAN_INVERTER_COMMAND_MSG;

/** Fan Status */
typedef struct {
	/** Fan RPM (Byte 0) */
	uint16_t fan_speed;
	/** 0-22 (Byte 2) */
	uint8_t input_voltage;
	/** 0-10 (Byte 3) */
	uint8_t input_current;
} GRCAN_FAN_STATUS_MSG;

/** Fan Command */
typedef struct {
	/** 0-100 Percent (Byte 0) */
	uint8_t fan_command;
} GRCAN_FAN_COMMAND_MSG;

/** Dash Status */
typedef struct {
	/** [Byte 0 / Bits 0-7]
0: TS Active
1: RTD
2-7: Reserved (Byte 0) */
	uint8_t button_flags;
	/** [Byte 1 / Bits 8-15]
0: BMS
1: IMD
2: BSPD
3-7: Reserved (Byte 1) */
	uint8_t led_bits;
} GRCAN_DASH_STATUS_MSG;

/** Dash Config */
typedef struct {
	/** [Byte 0 / Bits 0-7]
0: BMS LED command
1: IMD LED command
2: BSPD LED command
3-7: Reserved (Byte 0) */
	uint8_t led_bits;
} GRCAN_DASH_CONFIG_MSG;

/** Steering Status */
typedef struct {
	/** Position of knob (1-16) Position of knob (1-16) (Byte 0) */
	uint8_t current_encoder_torque_map_encoder;
	/** Position of knob (1-16) Button State Button State Button State Button State (Byte 1) */
	uint8_t regen_button_1_button_2_button_3_button_4;
} GRCAN_STEERING_STATUS_MSG;

/** Steering Config */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint8_t reserved;
} GRCAN_STEERING_CONFIG_MSG;

/** SAM Brake IR */
typedef struct {
	/** IR Temp of Brakes (Byte 0) */
	uint8_t temp;
} GRCAN_SAM_BRAKE_IR_MSG;

/** SAM Tire Temp */
typedef struct {
	/** Furthest from chassis (Byte 0) */
	uint8_t outside_temp;
	/** Middle of tire (Byte 1) */
	uint8_t outside_middle_temp;
	/** Middle of tire (Byte 2) */
	uint8_t inside_middle_temp;
	/** Closest to chassis (Byte 3) */
	uint8_t inside_temp;
} GRCAN_SAM_TIRE_TEMP_MSG;

/** SAM IMU */
typedef struct {
	/** Acceleration in X-axis (Byte 0) */
	uint16_t accel_x;
	/** Acceleration in Y-axis (Byte 2) */
	uint16_t accel_y;
	/** Acceleration in Z-axis (Byte 4) */
	uint16_t accel_z;
	/** Angular velocity in X-axis (Byte 6) */
	uint16_t gyro_x;
	/** Angular velocity in Y-axis (Byte 8) */
	uint16_t gyro_y;
	/** Angular velocity in Z-axis (Byte 10) */
	uint16_t gyro_z;
} GRCAN_SAM_IMU_MSG;

/** SAM GPS 1 */
typedef struct {
	/** Latitude in decimal degrees (Byte 0) */
	uint32_t latitude;
	/** Longitude in decimal degrees (Byte 4) */
	uint32_t longitude;
} GRCAN_SAM_GPS_1_MSG;

/** SAM GPS 2 */
typedef struct {
	/** GPS position accuracy (Byte 0) */
	uint32_t accuracy;
	/** Vehicle attitude (Byte 4) */
	uint32_t attitude;
} GRCAN_SAM_GPS_2_MSG;

/** SAM GPS Time */
typedef struct {
	/** Time in seconds since GPS Epoch (Byte 0) */
	uint32_t time;
	/** Time of week in milliseconds (Byte 4) */
	uint32_t time_of_week_ms;
} GRCAN_SAM_GPS_TIME_MSG;

/** SAM GPS Heading */
typedef struct {
	/** Heading angle relative to true North (Byte 0) */
	uint32_t heading_from_north;
} GRCAN_SAM_GPS_HEADING_MSG;

/** SAM Sus Pots */
typedef struct {
	/** Pot Pos (Byte 0) */
	uint8_t suspension_angle;
} GRCAN_SAM_SUS_POTS_MSG;

/** SAM TOF */
typedef struct {
	/** Ride Height (Byte 0) */
	uint16_t height;
} GRCAN_SAM_TOF_MSG;

/** SAM Rear Wheelspeed */
typedef struct {
	/** Wheel RPM (Byte 0) */
	uint16_t speed;
} GRCAN_SAM_REAR_WHEELSPEED_MSG;

/** SAM Pushrod Force */
typedef struct {
	/** Pushrod Force (Byte 0) */
	uint16_t load_force;
} GRCAN_SAM_PUSHROD_FORCE_MSG;

/** TCM Status */
typedef struct {
	/** [Byte 0 / Bits 0-7]
0: Connection Status
1: MQTT Status
2: Epic Shelter Status
3: Camera Status
4-7: Reserved (Byte 0) */
	uint8_t status_bits;
	/** Mapache ping (upload) (Byte 1) */
	uint16_t ping;
	/** # of messages on cache (non-synced) (Byte 3) */
	uint32_t cache_size;
	/** Byte 7 (Byte 7) */
	uint8_t reserved;
} GRCAN_TCM_STATUS_MSG;

/** TCM Resource Utilization */
typedef struct {
	/** core 0 frequency in MHz (Byte 0) */
	uint16_t cpu_0_freq;
	/** core 0 utilization in % (Byte 2) */
	uint8_t cpu_0_util;
	/** core 1 frequency in MHz (Byte 3) */
	uint16_t cpu_1_freq;
	/** core 1 utilization in % (Byte 5) */
	uint8_t cpu_1_util;
	/** core 2 frequency in MHz (Byte 6) */
	uint16_t cpu_2_freq;
	/** core 2 utilization in % (Byte 8) */
	uint8_t cpu_2_util;
	/** core 3 frequency in MHz (Byte 9) */
	uint16_t cpu_3_freq;
	/** core 3 utilization in % (Byte 11) */
	uint8_t cpu_3_util;
	/** core 4 frequency in MHz (Byte 12) */
	uint16_t cpu_4_freq;
	/** core 4 utilization in % (Byte 14) */
	uint8_t cpu_4_util;
	/** core 5 frequency in MHz (Byte 15) */
	uint16_t cpu_5_freq;
	/** core 5 utilization in % (Byte 17) */
	uint8_t cpu_5_util;
	/** total cpu utilization in % (Byte 18) */
	uint8_t cpu_total_util;
	/** total memory in MB (Byte 19) */
	uint16_t ram_total;
	/** used memory in MB (Byte 21) */
	uint16_t ram_used;
	/** memory utilization in % (Byte 23) */
	uint8_t ram_util;
	/** gpu utilization in % (Byte 24) */
	uint8_t gpu_util;
	/** gpu frequency in MHz (Byte 25) */
	uint16_t gpu_freq;
	/** total disk space in MB (Byte 27) */
	uint32_t disk_total;
	/** used disk space in MB (Byte 31) */
	uint32_t disk_used;
	/** disk utilization in % (Byte 35) */
	uint8_t disk_util;
	/** cpu temp in ˚C (Byte 36) */
	uint8_t cpu_temp;
	/** gpu temp in ˚C (Byte 37) */
	uint8_t gpu_temp;
	/** voltage draw in mV (Byte 38) */
	uint16_t voltage_draw;
	/** current draw in mA (Byte 40) */
	uint16_t current_draw;
	/** power draw in mW (Byte 42) */
	uint16_t power_draw;
} GRCAN_TCM_RESOURCE_UTILIZATION_MSG;

/** ECU Analog Data */
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
	/** lattitude (Byte 0) */
	uint8_t lat;
} GRCAN_GPS_LAT_MSG;

/** GPS LON */
typedef struct {
	/** longitude (Byte 0) */
	uint8_t lon;
} GRCAN_GPS_LON_MSG;

/** GPS ALT */
typedef struct {
	/** altitude (Byte 0) */
	uint8_t alt;
} GRCAN_GPS_ALT_MSG;

/** GPS PX */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t theta;
	/** Byte 2 (Byte 2) */
	uint16_t acc;
	/** Byte 4 (Byte 4) */
	uint32_t status;
} GRCAN_GPS_PX_MSG;

/** GPS QY */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t theta;
	/** Byte 2 (Byte 2) */
	uint16_t acc;
	/** Byte 4 (Byte 4) */
	uint32_t status;
} GRCAN_GPS_QY_MSG;

/** GPS RZ */
typedef struct {
	/** Byte 0 (Byte 0) */
	uint16_t theta;
	/** Byte 2 (Byte 2) */
	uint16_t acc;
	/** Byte 4 (Byte 4) */
	uint32_t status;
} GRCAN_GPS_RZ_MSG;

/** UVW DGPS */
typedef struct {
	/** U (Byte 0) */
	uint16_t dgps_u;
	/** V (Byte 2) */
	uint16_t dgps_v;
	/** W (Byte 4) */
	uint16_t dgps_w;
} GRCAN_UVW_DGPS_MSG;

/** ECU Performance */
typedef struct {
	/** Represents the total number of clock cycles elapsed for 10 iterations of the main loop
data type: u32
units: Clock Cycles
data type: u8 (Byte 0) */
	uint8_t elapsed_cycles;
} GRCAN_ECU_PERFORMANCE_MSG;

#endif
