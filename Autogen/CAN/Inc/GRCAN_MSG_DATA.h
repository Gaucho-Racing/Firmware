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
	/** [Bytes 0-1 / Bits 0-15]
0: BCU Node Status (1: OK, 0: Timeout)
1: GR Inverter Status (1: OK, 0: Timeout)
2: Fan Controller 1 Status (1: OK, 0: Timeout)
3: Fan Controller 2 Status (1: OK, 0: Timeout)
4: Fan Controller 3 Status (1: OK, 0: Timeout)
5: Dash Panel Status (1: OK, 0: Timeout)
6: TCM Node Status (1: OK, 0: Timeout)
7: SAMM_Mag_1 Status (1: OK, 0: Timeout)
8: SAMM_Mag_2 Status (1: OK, 0: Timeout)
9: SAMM_ToF_1 Status (1: OK, 0: Timeout)
10: SAMM_ToF_2 Status (1: OK, 0: Timeout)
11: TireTemp_1 Status (1: OK, 0: Timeout)
12: TireTemp_2 Status (1: OK, 0: Timeout)
13: TireTemp_3 Status (1: OK, 0: Timeout)
14: TireTemp_4 Status (1: OK, 0: Timeout)
15: Reserved (Byte 0) */
	uint16_t status_flags;
	/** [Byte 2 / Bits 16-17] GLV States
0: GLV Off State,
1: GLV On State.
See diagram in StateMachine.
[Byte 2 / Bits 18-19] Precharge States
2: Precharge Engaged State
3: Precharge Complete State
See diagram in StateMachine.h
[Byte 2 / Bits 20-21] ECU States
4: Drive Active ECU State
5: TS Discharge ECU State
6-7: Reserved
See diagram in StateMachine.h (Byte 2) */
	uint8_t ecu_state;
	/** Controls the AC current limits to each of the inverters
Discrete Mapping, actual values TBD (16 possible values) The torque map selected; torque map is the mapping of the throttle to the torque sent to each motor (Byte 3) */
	uint8_t power_level_torque_map;
	/** the temperature of the hottest cell of the accumulator (Byte 4) */
	uint8_t max_cell_temp;
	/** % charged of the Accumulator (Byte 5) */
	uint8_t accumulator_state_of_charge;
	/** % charged of the Low Voltage Bat (Byte 6) */
	uint8_t glv_state_of_charge;
} GRCAN_ECU_STATUS_1_MSG;

/** ECU Status 2 */
typedef struct {
	/** Absolute value of speed (Byte 0) */
	uint16_t vehicle_speed;
	/** Output terminal voltage of accumulator (Byte 2) */
	uint16_t tractive_system_voltage;
	/** Wheel RPM (Byte 4) */
	uint16_t fr_wheel_rpm;
	/** Wheel RPM (Byte 6) */
	uint16_t fl_wheel_rpm;
} GRCAN_ECU_STATUS_2_MSG;

/** ECU Status 3 */
typedef struct {
	/** Wheel RPM (Byte 0) */
	uint16_t rr_wheel_rpm;
	/** Wheel RPM (Byte 2) */
	uint16_t rl_wheel_rpm;
} GRCAN_ECU_STATUS_3_MSG;

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
	/** TS above set max voltage, TS below set min voltage, Inverter over set max temp, Motor over set max temp, Mosfet or mosfet drive error, Encoder communication or calc error, CAN message
	 * error or timeout (Byte 2) */
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
	/** Represents the total number of clock cycles elapsed for 10 iterations of the main loop (Byte 0) */
	uint32_t elapsed_cycles;
} GRCAN_ECU_PERFORMANCE_MSG;

#endif
