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
	/** [Byte 0 / Bits 0-7] ECU state machine data
0: GLV Off State
1: GLV On State
2: Precharge Engaged State
3: Precharge Complete State
4: Drive Active State
5: TS Discharge State
6-7: Reserved
See diagram in StateMachine.h (Byte 0) */
	uint8_t ecu_state;
	/** [Byte 1 / Bits 8-15] ECU ping targets
8: ACU (1: OK, 0: Timeout)
9: GR Inverter (1: OK, 0: Timeout)
10: Fan Controller 1 (1: OK, 0: Timeout)
11: Fan Controller 2 (1: OK, 0: Timeout)
12: Fan Controller 3 (1: OK, 0: Timeout)
13: Dash Panel (1: OK, 0: Timeout)
14: TCM (1: OK, 0: Timeout)
15: DGPS (1: OK, 0: Timeout) (Byte 1) */
	uint8_t ping_group_1;
	/** [Byte 2 / Bits 16-23] ECU ping targets
16: Suspension FL (1: OK, 0: Timeout)
17: Suspension FR (1: OK, 0: Timeout)
18: Suspension RL (1: OK, 0: Timeout)
19: Suspension RR (1: OK, 0: Timeout)
20: InboardFloor FL (1: OK, 0: Timeout)
21: InboardFloor FR (1: OK, 0: Timeout)
22: InboardFloor RL (1: OK, 0: Timeout)
23: InboardFloor RR (1: OK, 0: Timeout) (Byte 2) */
	uint8_t ping_group_2;
	/** [Byte 3 / Bits 24-31] ECU ping targets
24: TireTemp FL (1: OK, 0: Timeout)
25: TireTemp FR (1: OK, 0: Timeout)
26: TireTemp RL (1: OK, 0: Timeout)
27: TireTemp RR (1: OK, 0: Timeout)
28: BrakeTemp FL (1: OK, 0: Timeout)
29: BrakeTemp FR (1: OK, 0: Timeout)
30: BrakeTemp RL (1: OK, 0: Timeout)
31: BrakeTemp RR (1: OK, 0: Timeout) (Byte 3) */
	uint8_t ping_group_3;
	/** Controls the AC current limits to each of the inverters
Discrete Mapping, actual values TBD (16 possible values) The torque map selected; torque map is the mapping of the throttle to the torque sent to each motor (Byte 4) */
	uint8_t power_level_torque_map;
	/** the temperature of the hottest cell of the accumulator (Byte 5) */
	uint8_t max_cell_temp;
	/** % charged of the Accumulator (Byte 6) */
	uint8_t accumulator_state_of_charge;
	/** % charged of the Low Voltage Bat (Byte 7) */
	uint8_t glv_state_of_charge;
} GRCAN_ECU_STATUS_1_MSG;

/** ECU Status 2 */
typedef struct {
	/** Output terminal voltage of accumulator (Byte 0) */
	uint16_t tractive_system_voltage;
	/** Absolute value of speed (Byte 2) */
	uint16_t vehicle_speed;
	/** FL Wheel RPM (Byte 4) */
	uint16_t fl_wheel_rpm;
	/** FR Wheel RPM (Byte 6) */
	uint16_t fr_wheel_rpm;
} GRCAN_ECU_STATUS_2_MSG;

/** ECU Status 3 */
typedef struct {
	/** RL Wheel RPM (Byte 0) */
	uint16_t rl_wheel_rpm;
	/** RR Wheel RPM (Byte 2) */
	uint16_t rr_wheel_rpm;
	/** [Byte 4 / Bits 32-39]
0: BMS OK
1: IMD OK
2: BSPD OK
3: Software OK
4-7: Reserved (Byte 4) */
	uint8_t relay_states;
} GRCAN_ECU_STATUS_3_MSG;

/** ACU Status 1 */
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
} GRCAN_ACU_STATUS_1_MSG;

/** ACU Status 2 */
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
	/** Voltage before ACU Latch
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
55: Precharge Timeout
54: IR- / Precharge State (0:Open, 1:Closed)
53: IR+ State (0:Open, 1:Closed)
52: Software Latch (0:Open, 1:Closed)
48-51: Reserved (Byte 6) */
	uint8_t precharge_latch_flags;
} GRCAN_ACU_STATUS_2_MSG;

/** ACU Status 3 */
typedef struct {
	/** 600v input voltage (Byte 0) */
	uint16_t hv_input_voltage;
	/** 20v output voltage (Byte 2) */
	uint16_t hv_output_voltage;
	/** 600v input current (Byte 4) */
	uint16_t hv_input_current;
	/** 20v output current (Byte 6) */
	uint16_t hv_output_current;
} GRCAN_ACU_STATUS_3_MSG;

/** ACU Precharge */
typedef struct {
	/** 0: shutdown, 1: go TS Active/Precharge (Byte 0) */
	uint8_t set_ts_active;
} GRCAN_ACU_PRECHARGE_MSG;

/** ACU Config Charge Parameters */
typedef struct {
	/** Sets the Target Charging voltage (Byte 0) */
	uint16_t charge_voltage;
	/** Sets the Target Charging Current (Byte 2) */
	uint16_t charge_current;
} GRCAN_ACU_CONFIG_CHARGE_PARAMETERS_MSG;

/** ACU Config Operational Parameters */
typedef struct {
	/** Sets the threshold for Minimum Cell Voltage before Shutdown (Byte 0) */
	uint8_t minimium_cell_voltage;
	/** Sets the threshold for Max Cell Temperature before Shutdown (Byte 1) */
	uint8_t max_cell_temperature;
} GRCAN_ACU_CONFIG_OPERATIONAL_PARAMETERS_MSG;

/** ACU Cell Data 1 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_ACU_CELL_DATA_1_MSG;

/** ACU Cell Data 2 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_ACU_CELL_DATA_2_MSG;

/** ACU Cell Data 3 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_ACU_CELL_DATA_3_MSG;

/** ACU Cell Data 4 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_ACU_CELL_DATA_4_MSG;

/** ACU Cell Data 5 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GRCAN_ACU_CELL_DATA_5_MSG;

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
	uint16_t u_mosfet_temperature;
	/** Celsius + 40, uint8_t (Byte 2) */
	uint16_t v_mosfet_temperature;
	/** Celsius + 40, uint8_t (Byte 4) */
	uint16_t w_mosfet_temperature;
} GRCAN_INVERTER_STATUS_2_MSG;

/** Inverter Status 3 */
typedef struct {
	/** Celsius + 40, uint8_t (Byte 0) */
	uint8_t motor_temperature;
	/** TS above set max voltage, TS below set min voltage, Inverter over set max temp, Motor over set max temp, Mosfet or mosfet drive error, Encoder communication or calc error, CAN message
	 * error or timeout (Byte 1) */
	uint8_t fault_bits;
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
0: TS press
1: RTD press
2: TS hold
3: RTD hold
4: BSPD led
5: IMD led
6: BMS led
7: Reserved (Byte 0) */
	uint8_t button_led_flags;
} GRCAN_DASH_STATUS_MSG;

/** Dash Config */
typedef struct {
	/** [Byte 0 / Bits 0-7]
0: BSPD led
1: IMD led
2: BMS led
3: BSPD latch
4: IMD latch
5: BMS latch
6-7: Reserved (Byte 0) */
	uint8_t led_latch_flags;
} GRCAN_DASH_CONFIG_MSG;

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
	uint16_t mapache_ping;
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

/** ECU Pinging RTT */
typedef struct {
	/** Round trip time (Byte 0) */
	uint8_t acu_rtt;
	/** Round trip time (Byte 1) */
	uint8_t gr_inverter_rtt;
	/** Round trip time (Byte 2) */
	uint8_t fan_controller_1_rtt;
	/** Round trip time (Byte 3) */
	uint8_t fan_controller_2_rtt;
	/** Round trip time (Byte 4) */
	uint8_t fan_controller_3_rtt;
	/** Round trip time (Byte 5) */
	uint8_t dash_panel_rtt;
	/** Round trip time (Byte 6) */
	uint8_t tcm_rtt;
	/** Round trip time (Byte 7) */
	uint8_t tire_temp_fl_rtt;
	/** Round trip time (Byte 8) */
	uint8_t tire_temp_fr_rtt;
	/** Round trip time (Byte 9) */
	uint8_t tire_temp_rl_rtt;
	/** Round trip time (Byte 10) */
	uint8_t tire_temp_rr_rtt;
	/** Round trip time (Byte 11) */
	uint8_t suspension_node_fl_rtt;
	/** Round trip time (Byte 12) */
	uint8_t suspension_node_fr_rtt;
	/** Round trip time (Byte 13) */
	uint8_t suspension_node_rl_rtt;
	/** Round trip time (Byte 14) */
	uint8_t suspension_node_rr_rtt;
	/** Round trip time (Byte 15) */
	uint8_t inboard_floor_fl_rtt;
	/** Round trip time (Byte 16) */
	uint8_t inboard_floor_fr_rtt;
	/** Round trip time (Byte 17) */
	uint8_t inboard_floor_rl_rtt;
	/** Round trip time (Byte 18) */
	uint8_t inboard_floor_rr_rtt;
	/** Round trip time (Byte 19) */
	uint8_t brake_temp_fl_rtt;
	/** Round trip time (Byte 20) */
	uint8_t brake_temp_fr_rtt;
	/** Round trip time (Byte 21) */
	uint8_t brake_temp_rl_rtt;
	/** Round trip time (Byte 22) */
	uint8_t brake_temp_rr_rtt;
	/** Round trip time (Byte 23) */
	uint8_t dgps_rtt;
} GRCAN_ECU_PINGING_RTT_MSG;

/** Tire Temp Frame 0 */
typedef struct {
	/** Tire temperature frame 0 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 0 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 0 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 0 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 0 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 0 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 0 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 0 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 0 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 0 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 0 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 0 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 0 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 0 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 0 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 0 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 0 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 0 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 0 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 0 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 0 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 0 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 0 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 0 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 0 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 0 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 0 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 0 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 0 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 0 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 0 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 0 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_0_MSG;

/** Tire Temp Frame 1 */
typedef struct {
	/** Tire temperature frame 1 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 1 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 1 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 1 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 1 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 1 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 1 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 1 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 1 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 1 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 1 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 1 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 1 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 1 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 1 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 1 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 1 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 1 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 1 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 1 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 1 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 1 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 1 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 1 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 1 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 1 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 1 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 1 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 1 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 1 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 1 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 1 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_1_MSG;

/** Tire Temp Frame 2 */
typedef struct {
	/** Tire temperature frame 2 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 2 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 2 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 2 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 2 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 2 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 2 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 2 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 2 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 2 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 2 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 2 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 2 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 2 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 2 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 2 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 2 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 2 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 2 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 2 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 2 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 2 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 2 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 2 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 2 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 2 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 2 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 2 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 2 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 2 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 2 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 2 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_2_MSG;

/** Tire Temp Frame 3 */
typedef struct {
	/** Tire temperature frame 3 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 3 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 3 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 3 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 3 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 3 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 3 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 3 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 3 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 3 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 3 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 3 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 3 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 3 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 3 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 3 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 3 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 3 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 3 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 3 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 3 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 3 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 3 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 3 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 3 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 3 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 3 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 3 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 3 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 3 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 3 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 3 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_3_MSG;

/** Tire Temp Frame 4 */
typedef struct {
	/** Tire temperature frame 4 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 4 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 4 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 4 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 4 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 4 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 4 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 4 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 4 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 4 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 4 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 4 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 4 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 4 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 4 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 4 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 4 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 4 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 4 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 4 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 4 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 4 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 4 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 4 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 4 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 4 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 4 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 4 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 4 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 4 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 4 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 4 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_4_MSG;

/** Tire Temp Frame 5 */
typedef struct {
	/** Tire temperature frame 5 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 5 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 5 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 5 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 5 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 5 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 5 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 5 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 5 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 5 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 5 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 5 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 5 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 5 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 5 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 5 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 5 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 5 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 5 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 5 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 5 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 5 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 5 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 5 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 5 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 5 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 5 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 5 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 5 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 5 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 5 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 5 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_5_MSG;

/** Tire Temp Frame 6 */
typedef struct {
	/** Tire temperature frame 6 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 6 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 6 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 6 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 6 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 6 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 6 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 6 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 6 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 6 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 6 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 6 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 6 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 6 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 6 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 6 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 6 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 6 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 6 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 6 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 6 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 6 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 6 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 6 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 6 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 6 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 6 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 6 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 6 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 6 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 6 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 6 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_6_MSG;

/** Tire Temp Frame 7 */
typedef struct {
	/** Tire temperature frame 7 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 7 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 7 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 7 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 7 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 7 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 7 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 7 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 7 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 7 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 7 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 7 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 7 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 7 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 7 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 7 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 7 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 7 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 7 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 7 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 7 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 7 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 7 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 7 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 7 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 7 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 7 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 7 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 7 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 7 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 7 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 7 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_7_MSG;

/** Tire Temp Frame 8 */
typedef struct {
	/** Tire temperature frame 8 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 8 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 8 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 8 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 8 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 8 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 8 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 8 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 8 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 8 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 8 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 8 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 8 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 8 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 8 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 8 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 8 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 8 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 8 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 8 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 8 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 8 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 8 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 8 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 8 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 8 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 8 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 8 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 8 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 8 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 8 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 8 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_8_MSG;

/** Tire Temp Frame 9 */
typedef struct {
	/** Tire temperature frame 9 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 9 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 9 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 9 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 9 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 9 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 9 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 9 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 9 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 9 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 9 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 9 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 9 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 9 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 9 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 9 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 9 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 9 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 9 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 9 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 9 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 9 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 9 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 9 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 9 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 9 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 9 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 9 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 9 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 9 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 9 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 9 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_9_MSG;

/** Tire Temp Frame 10 */
typedef struct {
	/** Tire temperature frame 10 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 10 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 10 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 10 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 10 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 10 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 10 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 10 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 10 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 10 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 10 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 10 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 10 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 10 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 10 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 10 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 10 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 10 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 10 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 10 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 10 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 10 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 10 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 10 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 10 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 10 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 10 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 10 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 10 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 10 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 10 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 10 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_10_MSG;

/** Tire Temp Frame 11 */
typedef struct {
	/** Tire temperature frame 11 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 11 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 11 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 11 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 11 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 11 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 11 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 11 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 11 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 11 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 11 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 11 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 11 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 11 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 11 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 11 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 11 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 11 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 11 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 11 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 11 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 11 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 11 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 11 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 11 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 11 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 11 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 11 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 11 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 11 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 11 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 11 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_11_MSG;

/** Tire Temp Frame 12 */
typedef struct {
	/** Tire temperature frame 12 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 12 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 12 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 12 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 12 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 12 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 12 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 12 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 12 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 12 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 12 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 12 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 12 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 12 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 12 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 12 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 12 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 12 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 12 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 12 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 12 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 12 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 12 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 12 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 12 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 12 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 12 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 12 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 12 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 12 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 12 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 12 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_12_MSG;

/** Tire Temp Frame 13 */
typedef struct {
	/** Tire temperature frame 13 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 13 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 13 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 13 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 13 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 13 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 13 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 13 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 13 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 13 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 13 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 13 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 13 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 13 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 13 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 13 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 13 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 13 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 13 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 13 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 13 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 13 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 13 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 13 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 13 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 13 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 13 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 13 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 13 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 13 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 13 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 13 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_13_MSG;

/** Tire Temp Frame 14 */
typedef struct {
	/** Tire temperature frame 14 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 14 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 14 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 14 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 14 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 14 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 14 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 14 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 14 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 14 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 14 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 14 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 14 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 14 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 14 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 14 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 14 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 14 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 14 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 14 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 14 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 14 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 14 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 14 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 14 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 14 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 14 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 14 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 14 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 14 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 14 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 14 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_14_MSG;

/** Tire Temp Frame 15 */
typedef struct {
	/** Tire temperature frame 15 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 15 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 15 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 15 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 15 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 15 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 15 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 15 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 15 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 15 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 15 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 15 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 15 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 15 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 15 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 15 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 15 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 15 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 15 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 15 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 15 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 15 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 15 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 15 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 15 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 15 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 15 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 15 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 15 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 15 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 15 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 15 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_15_MSG;

/** Tire Temp Frame 16 */
typedef struct {
	/** Tire temperature frame 16 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 16 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 16 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 16 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 16 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 16 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 16 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 16 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 16 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 16 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 16 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 16 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 16 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 16 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 16 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 16 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 16 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 16 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 16 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 16 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 16 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 16 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 16 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 16 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 16 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 16 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 16 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 16 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 16 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 16 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 16 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 16 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_16_MSG;

/** Tire Temp Frame 17 */
typedef struct {
	/** Tire temperature frame 17 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 17 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 17 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 17 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 17 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 17 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 17 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 17 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 17 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 17 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 17 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 17 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 17 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 17 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 17 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 17 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 17 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 17 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 17 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 17 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 17 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 17 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 17 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 17 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 17 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 17 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 17 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 17 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 17 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 17 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 17 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 17 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_17_MSG;

/** Tire Temp Frame 18 */
typedef struct {
	/** Tire temperature frame 18 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 18 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 18 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 18 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 18 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 18 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 18 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 18 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 18 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 18 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 18 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 18 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 18 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 18 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 18 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 18 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 18 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 18 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 18 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 18 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 18 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 18 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 18 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 18 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 18 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 18 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 18 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 18 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 18 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 18 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 18 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 18 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_18_MSG;

/** Tire Temp Frame 19 */
typedef struct {
	/** Tire temperature frame 19 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 19 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 19 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 19 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 19 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 19 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 19 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 19 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 19 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 19 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 19 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 19 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 19 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 19 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 19 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 19 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 19 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 19 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 19 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 19 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 19 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 19 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 19 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 19 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 19 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 19 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 19 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 19 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 19 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 19 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 19 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 19 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_19_MSG;

/** Tire Temp Frame 20 */
typedef struct {
	/** Tire temperature frame 20 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 20 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 20 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 20 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 20 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 20 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 20 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 20 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 20 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 20 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 20 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 20 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 20 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 20 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 20 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 20 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 20 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 20 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 20 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 20 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 20 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 20 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 20 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 20 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 20 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 20 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 20 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 20 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 20 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 20 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 20 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 20 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_20_MSG;

/** Tire Temp Frame 21 */
typedef struct {
	/** Tire temperature frame 21 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 21 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 21 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 21 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 21 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 21 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 21 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 21 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 21 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 21 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 21 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 21 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 21 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 21 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 21 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 21 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 21 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 21 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 21 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 21 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 21 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 21 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 21 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 21 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 21 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 21 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 21 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 21 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 21 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 21 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 21 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 21 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_21_MSG;

/** Tire Temp Frame 22 */
typedef struct {
	/** Tire temperature frame 22 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 22 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 22 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 22 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 22 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 22 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 22 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 22 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 22 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 22 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 22 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 22 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 22 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 22 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 22 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 22 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 22 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 22 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 22 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 22 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 22 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 22 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 22 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 22 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 22 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 22 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 22 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 22 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 22 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 22 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 22 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 22 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_22_MSG;

/** Tire Temp Frame 23 */
typedef struct {
	/** Tire temperature frame 23 pixel 0 (Byte 0) */
	uint16_t pixel0;
	/** Tire temperature frame 23 pixel 1 (Byte 2) */
	uint16_t pixel1;
	/** Tire temperature frame 23 pixel 2 (Byte 4) */
	uint16_t pixel2;
	/** Tire temperature frame 23 pixel 3 (Byte 6) */
	uint16_t pixel3;
	/** Tire temperature frame 23 pixel 4 (Byte 8) */
	uint16_t pixel4;
	/** Tire temperature frame 23 pixel 5 (Byte 10) */
	uint16_t pixel5;
	/** Tire temperature frame 23 pixel 6 (Byte 12) */
	uint16_t pixel6;
	/** Tire temperature frame 23 pixel 7 (Byte 14) */
	uint16_t pixel7;
	/** Tire temperature frame 23 pixel 8 (Byte 16) */
	uint16_t pixel8;
	/** Tire temperature frame 23 pixel 9 (Byte 18) */
	uint16_t pixel9;
	/** Tire temperature frame 23 pixel 10 (Byte 20) */
	uint16_t pixel10;
	/** Tire temperature frame 23 pixel 11 (Byte 22) */
	uint16_t pixel11;
	/** Tire temperature frame 23 pixel 12 (Byte 24) */
	uint16_t pixel12;
	/** Tire temperature frame 23 pixel 13 (Byte 26) */
	uint16_t pixel13;
	/** Tire temperature frame 23 pixel 14 (Byte 28) */
	uint16_t pixel14;
	/** Tire temperature frame 23 pixel 15 (Byte 30) */
	uint16_t pixel15;
	/** Tire temperature frame 23 pixel 16 (Byte 32) */
	uint16_t pixel16;
	/** Tire temperature frame 23 pixel 17 (Byte 34) */
	uint16_t pixel17;
	/** Tire temperature frame 23 pixel 18 (Byte 36) */
	uint16_t pixel18;
	/** Tire temperature frame 23 pixel 19 (Byte 38) */
	uint16_t pixel19;
	/** Tire temperature frame 23 pixel 20 (Byte 40) */
	uint16_t pixel20;
	/** Tire temperature frame 23 pixel 21 (Byte 42) */
	uint16_t pixel21;
	/** Tire temperature frame 23 pixel 22 (Byte 44) */
	uint16_t pixel22;
	/** Tire temperature frame 23 pixel 23 (Byte 46) */
	uint16_t pixel23;
	/** Tire temperature frame 23 pixel 24 (Byte 48) */
	uint16_t pixel24;
	/** Tire temperature frame 23 pixel 25 (Byte 50) */
	uint16_t pixel25;
	/** Tire temperature frame 23 pixel 26 (Byte 52) */
	uint16_t pixel26;
	/** Tire temperature frame 23 pixel 27 (Byte 54) */
	uint16_t pixel27;
	/** Tire temperature frame 23 pixel 28 (Byte 56) */
	uint16_t pixel28;
	/** Tire temperature frame 23 pixel 29 (Byte 58) */
	uint16_t pixel29;
	/** Tire temperature frame 23 pixel 30 (Byte 60) */
	uint16_t pixel30;
	/** Tire temperature frame 23 pixel 31 (Byte 62) */
	uint16_t pixel31;
} GRCAN_TIRE_TEMP_FRAME_23_MSG;

/** Brake Temp */
typedef struct {
	/** Brake rotor temperature (Byte 0) */
	uint16_t temp;
} GRCAN_BRAKE_TEMP_MSG;

/** Wheel Speed */
typedef struct {
	/** Wheel speed rpm (Byte 0) */
	uint16_t speed;
} GRCAN_WHEEL_SPEED_MSG;

#endif
