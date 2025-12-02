#include <stdint.h>

// DO NOT CONSIDER THESE STABLE ... EXPECT URCA TO REPLACE THEM FULLY

// ADD MESSAGES AS NEEDED AND AT MODIFICATION, TALK TO URCA FOLKS THOUGH

// ALSO... AI DID THE COMMENTS FROM THE DOCUMENT... DO NOT TRUST AT ALL

#ifndef GR_OLD_MSG_DAT_H
#define GR_OLD_MSG_DAT_H

/** Debug 2.0 - Print statement up to 8 bytes */
typedef struct {
	uint8_t message[8]; /** Debug msg parseable by target */
} GR_OLD_DEBUG_2_MSG;

/** Debug FD - Print statement up to 64 bytes */
typedef struct {
	uint8_t message[64]; /** Debug msg parseable by target */
} GR_OLD_DEBUG_FD_MSG;

/** Ping message */
typedef struct {
	uint32_t timestamp; /** Time in ms */
} GR_OLD_PING_MSG;

/** ECU Status 1 */
typedef struct {
	uint8_t ecu_status; /** ECU state (see diagram) */
	uint8_t
	    ping_block[3]; /** Node timeout status bits (1=OK, 0=Timeout) */
	uint8_t powerlevel_torquemap; /** Power lvl (4b) & torque map (4b) */
	uint8_t max_cell_temp;	      /** Hottest cell temp, 0.25°C/bit */
	uint8_t tractivebattery_soc;  /** Accumulator SoC, 20x/51=% */
	uint8_t glv_soc;	      /** GLV SoC, 20x/51=% */
} GR_OLD_ECU_STATUS_1_MSG;

/** ECU Status 2 */
typedef struct {
	uint16_t ts_voltage;	/** TS output voltage, 0.01V */
	uint16_t vehicle_speed; /** Vehicle speed, 0.01 MPH */
	uint16_t fr_wheel_rpm;	/** FR wheel, 0.1x-3276.8 RPM */
	uint16_t fl_wheel_rpm;	/** FL wheel, 0.1x-3276.8 RPM */
} GR_OLD_ECU_STATUS_2_MSG;

/** ECU Status 3 */
typedef struct {
	uint16_t rr_wheel_rpm; /** RR wheel, 0.1x-3276.8 RPM */
	uint16_t rl_wheel_rpm; /** RL wheel, 0.1x-3276.8 RPM */
} GR_OLD_ECU_STATUS_3_MSG;

/** ACU Status 1 */
typedef struct {
	uint16_t tractivebattery_voltage; /** All cells sum, 0.01V */
	uint16_t ts_voltage;		  /** TS output voltage, 0.01V */
	uint16_t tractivebattery_current; /** Current out (i16), 0.01A */
	uint8_t tractivebattery_soc; /** Accum SoC (lowest cell), 20x/51=% */
	uint8_t glv_soc;	     /** GLV SoC, 20x/51=% */
} GR_OLD_BCU_STATUS_1_MSG;

/** ACU Status 2 */
typedef struct {
	uint8_t voltage_20v;	  /** 20V GLV, 0.1V */
	uint8_t voltage_12v;	  /** 12V supply, 0.1V */
	uint8_t voltage_sdc;	  /** SDC voltage before latch, 0.1V */
	uint8_t voltage_min_cell; /** Min cell V, 0.01x+2 V */
	uint8_t max_cell_temp;	  /** Hottest cell, 0.25°C */
	uint8_t error_bits;	  /** Error flags (temp/V/I) */
	uint8_t precharge_bits;	  /** Precharge & relay states */
} GR_OLD_BCU_STATUS_2_MSG;

/** ACU Status 3 */
typedef struct {
	uint16_t hv_input_voltage;  /** 600V input, 0.01V */
	uint16_t hv_output_voltage; /** 20V output, 0.01V */
	uint16_t hv_input_current;  /** 600V input, 0.001A */
	uint16_t hv_output_current; /** 20V output, 0.001A */
} GR_OLD_BCU_STATUS_3_MSG;

/** ACU Precharge cmd */
typedef struct {
	uint8_t precharge; /** Set TS active (0=shutdown, 1=precharge) */
} GR_OLD_BCU_PRECHARGE_MSG;

/** ACU Config Charge Params */
typedef struct {
	uint16_t charge_voltage; /** Target charge V, 0.1V */
	uint16_t charge_current; /** Target charge I, 0.1A */
} GR_OLD_BCU_CONFIG_CHARGE_PARAMS_MSG;

/** ACU Config Operational Params */
typedef struct {
	uint8_t min_cell_voltage; /** Min cell V thresh, 0.01x+2 V */
	uint8_t max_cell_voltage; /** Max cell temp thresh, 0.25°C */
} GR_OLD_BCU_CONFIG_OPS_PARAMS_MSG;

/** ACU Cell Data 1 - Cells 0-31 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	};
} GR_OLD_BCU_CELL_DATA_1_MSG;

/** ACU Cell Data 2 - Cells 32-63 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	};
} GR_OLD_BCU_CELL_DATA_2_MSG;

/** ACU Cell Data 3 - Cells 64-95 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	};
} GR_OLD_BCU_CELL_DATA_3_MSG;

/** ACU Cell Data 4 - Cells 96-127 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	};
} GR_OLD_BCU_CELL_DATA_4_MSG;

/** ACU Cell Data 5 - Cells 128-159 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	};
} GR_OLD_BCU_CELL_DATA_5_MSG;

/** DC-DC Converter Status */
typedef struct {
	uint16_t input_voltage;	 /** ~20V (LV) or 600V (HV), x/1000 V */
	uint16_t output_voltage; /** ~12V (LV) or ~20V (HV), x/1000 V */
	uint8_t input_current;	 /** Input I (LV only), 0.1A */
	uint8_t output_current;	 /** Output I, 0.1A */
	uint8_t temperature;	 /** DC-DC temp, °C */
} GR_OLD_DCDC_STATUS_MSG;

/** Inverter Status 1 */
typedef struct {
	uint16_t ac_current; /** AC I (i16), 0.01x-327.68 A */
	uint16_t dc_current; /** DC I, 0.01A */
	uint16_t motor_rpm;  /** Motor RPM (i16), x-32768 */
} GR_OLD_INVERTER_STATUS_1_MSG;

/** Inverter Status 2 */
typedef struct {
	uint16_t u_mosfet_temp; /** U phase temp, x-40 °C */
	uint16_t v_mosfet_temp; /** V phase temp, x-40 °C */
	uint16_t w_mosfet_temp; /** W phase temp, x-40 °C */
} GR_OLD_INVERTER_STATUS_2_MSG;

/** Inverter Status 3 */
typedef struct {
	uint8_t motor_temp; /** Motor temp, x-40 °C */
	uint8_t fault_bits; /** Fault flags (V/temp/transistor/encoder/CAN) */
} GR_OLD_INVERTER_STATUS_3_MSG;

/** Inverter Config */
typedef struct {
	uint16_t max_ac_current;    /** Max AC I, 0.01x-327.68 A */
	uint16_t max_dc_current;    /** Max DC I, 0.01x-327.68 A */
	uint16_t abs_max_motor_rpm; /** Abs max RPM limit, x-32768 */
	uint8_t motor_direction;    /** Motor dir (1=invert) */
} GR_OLD_INVERTER_CONFIG_MSG;

/** Inverter Command */
typedef struct {
	uint16_t ac_current;	 /** Cmd AC I, 0.01x-327.68 A */
	uint16_t dc_current;	 /** Cmd DC I, 0.01x-327.68 A */
	uint16_t rpm_limit;	 /** RPM limit (0=none), x-32768 */
	uint8_t field_weakening; /** Field weak strength, 0.1A */
	uint8_t drive_enable;	 /** Enable inv (send 1 every 100ms) */
} GR_OLD_INVERTER_COMMAND_MSG;

/** Fan Status */
typedef struct {
	uint16_t fan_speed;    /** Fan RPM */
	uint8_t input_voltage; /** Input V (0-22), 0.1V */
	uint8_t input_current; /** Input I (0-10), 0.1A */
} GR_OLD_FAN_STATUS_MSG;

/** Fan Command */
typedef struct {
	uint8_t fan_command; /** Fan cmd, 0-100% */
} GR_OLD_FAN_COMMAND_MSG;

/** Dashboard Status */
typedef struct {
	uint8_t led_bits;  /** BMS/IMD/BSPD LED states */
	uint8_t ts_button; /** TS button state & time (MSB=pressed, 7b=0.1s) */
	uint8_t
	    rtd_button; /** RTD button state & time (MSB=pressed, 7b=0.1s) */
} GR_OLD_DASH_STATUS_MSG;

/** Dashboard Config */
typedef struct {
	uint8_t led_bits;	/** BMS/IMD/BSPD LED cmds (0=off, 1=on) */
	uint8_t button_led_1_r; /** Button 1 red (set by ECU state) */
	uint8_t button_led_1_g; /** Button 1 green (set by ECU state) */
	uint8_t button_led_1_b; /** Button 1 blue (set by ECU state) */
	uint8_t button_led_2_r; /** Button 2 red (set by ECU state) */
	uint8_t button_led_2_g; /** Button 2 green (set by ECU state) */
	uint8_t button_led_2_b; /** Button 2 blue (set by ECU state) */
} GR_OLD_DASH_CONFIG_MSG;

/** Steering Wheel Status */
typedef struct {
	uint8_t
	    encoder_bits; /** Current encoder pos (1-16) & torque map (1-16) */
	uint8_t
	    regen_and_button_bits; /** Regen pos (1-16) & 4 button states */
} GR_OLD_STEERING_STATUS_MSG;

/** Steering Wheel Config */
typedef struct {
	uint8_t RESERVED; /** Updates based on ECU status */
} GR_OLD_STEERING_CONFIG_MSG;

/** SAM Brake IR Temp */
typedef struct {
	uint8_t temperature; /** IR brake temp, °C */
} GR_OLD_SAM_BRAK_IR_MSG;

/** SAM Tire Temps */
typedef struct {
	uint8_t outside_temp;	     /** Furthest from chassis, °C */
	uint8_t outside_middle_temp; /** Mid-tire outer, °C */
	uint8_t inside_middle_temp;  /** Mid-tire inner, °C */
	uint8_t inside_temp;	     /** Closest to chassis, °C */
} GR_OLD_SAM_TIRE_TEMP_MSG;

/** SAM IMU Data */
typedef struct {
	uint16_t acceleration_x; /** X accel, 0.01x-327.68 m/s² */
	uint16_t acceleration_y; /** Y accel, 0.01x-327.68 m/s² */
	uint16_t acceleration_z; /** Z accel, 0.01x-327.68 m/s² */
	uint16_t gyro_x;	 /** X ang vel, 0.001x-32.768 rad/s */
	uint16_t gyro_y;	 /** Y ang vel, 0.001x-32.768 rad/s */
	uint16_t gyro_z;	 /** Z ang vel, 0.001x-32.768 rad/s */
} GR_OLD_SAM_IMU_MSG;

/** SAM GPS Position */
typedef struct {
	uint32_t latitude;  /** Lat, decimal degrees */
	uint32_t longitude; /** Lon, decimal degrees */
} GR_OLD_SAM_GPS_1_MSG;

/** SAM GPS Accuracy & Altitude */
typedef struct {
	uint32_t accuracy; /** GPS position accuracy */
	uint16_t altitude; /** Vehicle altitude */
} GR_OLD_SAM_GPS_2_MSG;

/** SAM GPS Time */
typedef struct {
	uint32_t time;		  /** Time since GPS epoch, s */
	uint32_t time_of_week_ms; /** Time of week, ms */
} GR_OLD_SAM_GPS_TIME_MSG;

/** SAM GPS Heading */
typedef struct {
	uint32_t heading; /** Heading angle rel to true North */
} GR_OLD_SAM_GPS_HEADING_MSG;

/** SAM Suspension Pots */
typedef struct {
	uint8_t suspension_angle; /** Pot position, degrees */
} GR_OLD_SUS_POTS_MSG;

/** SAM Time-of-Flight */
typedef struct {
	uint16_t height; /** Ride height, x/256 mm */
} GR_OLD_SAM_TOF_MSG;

/** SAM Rear Wheelspeed */
typedef struct {
	uint16_t wheel_speed; /** Wheel RPM, 0.1x-3276.8 */
} GR_OLD_SAM_REAR_WHEELSPEED_MSG;

/** SAM Pushrod Force */
typedef struct {
	uint16_t load_force; /** Pushrod force, 0.1x-3276.8 N */
} GR_OLD_SAM_PUSHROD_FORCE_MSG;

/** TCM Status */
typedef struct {
	uint8_t status_bits; /** Conn/MQTT/shelter/camera status */
	uint16_t ping;	     /** Mapache ping (upload), ms */
	uint16_t cache_size; /** Non-synced msg count */
	uint8_t reserved;    /** Reserved */
} GR_OLD_TCM_STATUS_MSG;

/** TCM Resource Utilization */
typedef struct {
	uint16_t cpu_0_freq;	/** Core 0 freq, MHz */
	uint8_t cpu_0_util;	/** Core 0 util, % */
	uint16_t cpu_1_freq;	/** Core 1 freq, MHz */
	uint8_t cpu_1_util;	/** Core 1 util, % */
	uint16_t cpu_2_freq;	/** Core 2 freq, MHz */
	uint8_t cpu_2_util;	/** Core 2 util, % */
	uint16_t cpu_3_freq;	/** Core 3 freq, MHz */
	uint8_t cpu_3_util;	/** Core 3 util, % */
	uint16_t cpu_4_freq;	/** Core 4 freq, MHz */
	uint8_t cpu_4_util;	/** Core 4 util, % */
	uint16_t cpu_5_freq;	/** Core 5 freq, MHz */
	uint8_t cpu_5_util;	/** Core 5 util, % */
	uint8_t cpu_total_util; /** Total CPU util, % */
	uint16_t ram_total;	/** Total RAM, MB */
	uint16_t ram_used;	/** Used RAM, MB */
	uint8_t ram_util;	/** RAM util, % */
	uint8_t gpu_util;	/** GPU util, % */
	uint16_t gpu_freq;	/** GPU freq, MHz */
	uint32_t disk_total;	/** Total disk, MB */
	uint32_t disk_used;	/** Used disk, MB */
	uint8_t disk_util;	/** Disk util, % */
	uint8_t cpu_temp;	/** CPU temp, °C */
	uint8_t gpu_temp;	/** GPU temp, °C */
	uint16_t voltage_draw;	/** Voltage, mV */
	uint16_t current_draw;	/** Current, mA */
	uint16_t power_draw;	/** Power, mW */
} GR_OLD_TCM_RESOURCE_UTILIZATION_MSG;

/** Dashboard Warning Flags */
typedef struct {
	uint8_t flag_bits; /** Warning flags (BSE APPS violation, etc.) */
} GR_OLD_DASH_WARNING_FLAGS_MSG;

/** Specific Brake IR Temp */
typedef struct {
	uint8_t wheel_identifier; /** Wheel ID per wiki */
	uint8_t temp;		  /** IR brake temp, °C */
} GR_OLD_SPECIFIC_BRAKE_IR_MSG;

/** ECU Ping Information */
typedef struct {
	uint16_t online_ping_bits; /** Copy of ECU Status ping bitmap */
} GR_OLD_ECU_PING_INFORMATION_MSG;

/** ECU Pedals Data */
typedef struct {
	uint16_t apps1_signal;	 /** APPS1 signal, x/655.35 % */
	uint16_t apps2_signal;	 /** APPS2 signal, x/655.35 % */
	uint16_t bse_signal;	 /** Brake force signal, x/655.35 % */
	uint16_t brake_f_signal; /** Front brake pressure, x/655.35 % */
	uint16_t brake_r_signal; /** Rear brake pressure, x/655.35 % */
} GR_OLD_ECU_PEDALS_DATA_MSG;

/** GPS Latitude */
typedef struct {
	double lat; /** Latitude */
} GR_OLD_GPS_LAT_MSG;

/** GPS Longitude */
typedef struct {
	double lon; /** Longitude */
} GR_OLD_GPS_LON_MSG;

/** GPS Altitude */
typedef struct {
	double alt; /** Altitude */
} GR_OLD_GPS_ALT_MSG;

/** GPS PX Data */
typedef struct {
	int16_t theta;	 /** Theta, x/1000 */
	int16_t acc;	 /** Acceleration, x/100 */
	uint32_t status; /** Status */
} GR_OLD_GPS_PX_MSG;

/** GPS QY Data */
typedef struct {
	int16_t theta;	 /** Theta, x/1000 */
	int16_t acc;	 /** Acceleration, x/100 */
	uint32_t status; /** Status */
} GR_OLD_GPS_QY_MSG;

/** GPS RZ Data */
typedef struct {
	int16_t theta;	 /** Theta, x/1000 */
	int16_t acc;	 /** Acceleration, x/100 */
	uint32_t status; /** Status */
} GR_OLD_GPS_RZ_MSG;

/** UVW DGPS Data */
typedef struct {
	int16_t dgps_u; /** U coord, x/100 */
	int16_t dgps_v; /** V coord, x/100 */
	int16_t dgps_w; /** W coord, x/100 */
} GR_OLD_UVW_DGPS_MSG;

#endif

/*
MSG ID,Message Name,Length,Byte,Bit Start,Data Name,Description,Data
Type,Min,Max,Scaled Min,Scaled Max,Map Equation,Units,For Indexing
Purposes,Status 0x000,Debug 2.0,8,,,,,,,,,,,,0,
,,,0-7,0,Debug,Essentially a print statement up to 8 bytes long that whichever
targeted can parse,s,~,~,~,~,~,,0, 0x001,Debug FD,64,,,,,,,,,,,,1,
,,,0-63,0,Debug,Essentially a print statement up to 64 bytes long that whichever
targeted can parse,s,~,~,~,~,~,,1, 0x002,Ping,4,,,,,,,,,,,,2,
,,,0-3,0,Timestamp,Time in
millis,u32,0,"4,294,967,296",0,"4,294,967,296",1x,ms,2, 0x003,ECU Status
1,8,,,,,,,,,,,,3,
,,,0,0,ECU State,See diagram,u8,~,~,~,~,~,Enum,3,
,,,1-3,8,ACU Node Status ,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,9,GR Inv 1 Status,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,10,GR Inv 2 Status,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,11,GR Inv 3 Status,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,12,GR Inv 4 Status,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,13,Fan Controller 1,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,14,Fan Controller 2,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,15,Fan Controller 3,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,16,Fan Controller 4,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,17,Fan Controller 5,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,18,Fan Controller 6,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,19,Fan Controller 7,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,20,Fan Controller 8,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,21,Dash,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,22,Steering,"1: OK, 0: Timeout",b,0,1,~,~,~,Bool,3,
,,,,23-31,Reserved for future use,Reserved,~,~,~,~,~,~,~,3,
,,,4,32,Power Level,"Controls the AC current limits to each of the inverters
Discrete Mapping, actual values TBD (16 possible values)",u4,0,15,~,,~,~,3,
,,,,36,Torque Map,The torque map selected; torque map is the mapping of the
throttle to the torque sent to each motor,u4,0,16,~,~,~,~,3,
,,,5,40,Max Cell Temp,the temperature of the hottest cell of the
accumulator,u8,0,255,0,63.75,0.25x,Celsius,3,
,,,6,48,Accumulator State of Charge,% charged of the
Accumulator,u8,0,255,0,100,20x/51,%,3,
,,,7,56,GLV State of Charge,% charged of the Low Voltage
Bat,u8,0,255,0,100,20x/51,%,3, 0x004,ECU Status 2,8,,,,,,,,,,,,4,
,,,0-1,0,Tractive System Voltage,Output terminal voltage of
accumulator,u16,0,65535,0,655.35,0.01x,Volts,4,
,,,2-3,16,Vehicle Speed,Absolute value of
speed,u16,0,65535,0,655.35,0.01x,MPH,4,
,,,4-5,32,FR Wheel RPM,Wheel RPM,u16,0,65535,-3276.8,3276.7,0.1x-3276.8,RPM,4,
,,,6-7,48,FL Wheel RPM,Wheel RPM,u16,0,65535,-3276.8,3276.7,0.1x-3276.8,RPM,4,
0x005,ECU Status 3,4,,,,,,,,,,,,5,
,,,0-1,0,RR Wheel RPM,Wheel RPM,u16,0,65535,-3276.8,3276.7,0.1x-3276.8,RPM,5,
,,,2-3,16,RL Wheel RPM,Wheel RPM,u16,0,65535,-3276.8,3276.7,0.1x-3276.8,RPM,5,
0x006,ECU config,0,,,,,,,,,,,,6,
,,,0,0,Reserved,"Reserved, Currently all selection will be done in response to
Dash & Steering status messages",~,~,~,~,~,~,~,6, 0x007,ACU Status
1,8,,,,,,,,,,,,7,
,,,0-1,0,Accumulator Voltage,All cell voltages added
up,u16,0,65535,0,655.35,0.01x,Volts,7,Done
,,,2-3,16,TS Voltage,Output terminal voltage of
accumulator,u16,0,65535,0,655.35,0.01x,Volts,7,Done
,,,4-5,32,Accumulator Current,Current output of
accumulator,i16,-32768,32767,-327.68,327.67,0.01x,Amps,7,Done
,,,6,48,Accumulator SOC,Accumulator state of charge (Based on lowest
cell),u8,0,255,0,100,20x/51,%,7,Done
,,,7,56,GLV SOC,GLV state of charge,u8,0,255,0,100,20x/51,%,7,Done
0x008,ACU Status 2,7,,,,,,,,,,,,8,
,,,0,0,20v Voltage,20v GLV voltage,u8,0,255,0,25.5,0.1x,Volts,8,Done
,,,1,8,12v Voltage,12v supply voltage,u8,0,255,0,25.5,0.1x,Volts,8,Done
,,,2,16,SDC Voltage,Voltage before ACU Latch,u8,0,255,0,25.5,0.1x,Volts,8,Done
,,,3,24,Min Cell Voltage,Lowest cell voltage in
accumulator,u8,0,255,2,4.55,0.01x+2,Volts,8,Done
,,,4,32,Max Cell Temp,Hottest cell in
accumulator,u8,0,255,0,63.75,0.25x,Celsius,8,Done
,,,5,40,Over Temp Error,Over temp (>60c),b,0,1,~,~,~,Bool,8,Done
,,,,41,Over Voltage Error,Over voltage (>4.2v per cell),b,0,1,~,~,~,Bool,8,Done
,,,,42,Under Voltage Error,Under voltage (<2.5v per
cell),b,0,1,~,~,~,Bool,8,Done
,,,,43,Over Current Error,Over current (discharge),b,0,1,~,~,~,Bool,8,Done
,,,,44,Under Current Error,Over current (charging),b,0,1,~,~,~,Bool,8,Done
,,,,45,Under Voltage 20v Warning,20v GLV too low (<15?),b,0,1,~,~,~,Bool,8,Done
,,,,46,Under Voltage 12v Warning,12v supply too low
(<10?),b,0,1,~,~,~,Bool,8,Done
,,,,47,Under Voltage SDC Warning,SDC too low (<9?),b,0,1,~,~,~,Bool,8,Done
,,,6,48,Precharge Error,Precharge timed out,b,0,1,~,~,~,Bool,8,Done
,,,,49,IR- / Precharge State,0: Open           1: Closed,b,0,1,~,~,~,Bool,8,Done
,,,,50,IR+ State,0: Open           1: Closed,b,0,1,~,~,~,Bool,8,Done
,,,,51,Software Latch,0: Open           1: Closed,b,0,1,~,~,~,Bool,8,Done
,,,,52-55,Reserved,Future Use,,,,,,,,8,
0x009,ACU Status 3,8,,,,,,,,,,,,9,
,,,0-1,0,HV Input Voltage,600v input
voltage,u16,0,65535,0,655.35,0.01x,Volts,9,Done
,,,2-3,16,HV Output Voltage,20v output
voltage,u16,0,65535,0,655.35,0.01x,Volts,9,Done
,,,4-5,32,HV Input Current,600v input
current,u16,0,65535,0,65.535,0.001x,Amps,9,Done
,,,6-7,48,HV Output Current,20v output
current,u16,0,65535,0,65.535,0.001x,Amps,9,Done 0x00A,ACU
Precharge,1,,,,,,,,,,,,10,
,,,0,0,Set TS Active,"0: shutdown, 1: go TS
Active/Precharge",b,0,1,~,~,~,Bool,10,Done 0x00B,ACU Config Charge
Parameters,4,,,,,,,,,,,,11,
,,,0-1,0,Charge Voltage,Sets the Target Charging
voltage,u16,0,65535,0,6553.5,0.1x,Volts,11,Done
,,,2-3,16,Charge Current,Sets the Target Charging
Current,u16,0,65535,0,6553.5,0.1x,Amps,11,Done 0x00C,ACU Config Operational
Parameters,2,,,,,,,,,,,,12,
,,,0,0,Minimium Cell Voltage,Sets the threshold for Minimum Cell Voltage before
Shutdown,u8,0,255,2,4.55,0.01x+2,Volts,12,Done
,,,1,8,Max Cell Temperature,Sets the threshold for Max Cell Temperature before
Shutdown,u8,0,255,0,63.75,0.25x,Celsius,12,Done 0x00D,ACU Cell Data
1,64,,,,,,,,,,,,13,
,,,0,0,Cell 0 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,1,8,Cell 0 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,2,16,Cell 1 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,3,24,Cell 1 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,4,32,Cell 2 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,5,40,Cell 2 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,6,48,Cell 3 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,7,56,Cell 3 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,8,64,Cell 4 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,9,72,Cell 4 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,10,80,Cell 5 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,11,88,Cell 5 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,12,96,Cell 6 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,13,104,Cell 6 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,14,112,Cell 7 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,15,120,Cell 7 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,16,128,Cell 8 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,17,136,Cell 8 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,18,144,Cell 9 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,19,152,Cell 9 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,20,160,Cell 10 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,21,168,Cell 10 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,22,176,Cell 11 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,23,184,Cell 11 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,24,192,Cell 12 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,25,200,Cell 12 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,26,208,Cell 13 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,27,216,Cell 13 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,28,224,Cell 14 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,29,232,Cell 14 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,30,240,Cell 15 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,31,248,Cell 15 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,32,256,Cell 16 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,33,264,Cell 16 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,34,272,Cell 17 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,35,280,Cell 17 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,36,288,Cell 18 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,37,296,Cell 18 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,38,304,Cell 19 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,39,312,Cell 19 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,40,320,Cell 20 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,41,328,Cell 20 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,42,336,Cell 21 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,43,344,Cell 21 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,44,352,Cell 22 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,45,360,Cell 22 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,46,368,Cell 23 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,47,376,Cell 23 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,48,384,Cell 24 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,49,392,Cell 24 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,50,400,Cell 25 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,51,408,Cell 25 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,52,416,Cell 26 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,53,424,Cell 26 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,54,432,Cell 27 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,55,440,Cell 27 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,56,448,Cell 28 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,57,456,Cell 28 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,58,464,Cell 29 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,59,472,Cell 29 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,60,480,Cell 30 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,61,488,Cell 30 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
,,,62,496,Cell 31 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,13,Done
,,,63,504,Cell 31 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,13,Done
0x00E,ACU Cell Data 2,64,,,,,,,,,,,,14,
,,,0,0,Cell 32 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,1,8,Cell 32 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,2,16,Cell 33 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,3,24,Cell 33 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,4,32,Cell 34 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,5,40,Cell 34 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,6,48,Cell 35 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,7,56,Cell 35 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,8,64,Cell 36 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,9,72,Cell 36 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,10,80,Cell 37 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,11,88,Cell 37 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,12,96,Cell 38 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,13,104,Cell 38 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,14,112,Cell 39 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,15,120,Cell 39 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,16,128,Cell 40 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,17,136,Cell 40 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,18,144,Cell 41 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,19,152,Cell 41 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,20,160,Cell 42 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,21,168,Cell 42 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,22,176,Cell 43 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,23,184,Cell 43 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,24,192,Cell 44 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,25,200,Cell 44 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,26,208,Cell 45 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,27,216,Cell 45 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,28,224,Cell 46 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,29,232,Cell 46 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,30,240,Cell 47 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,31,248,Cell 47 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,32,256,Cell 48 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,33,264,Cell 48 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,34,272,Cell 49 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,35,280,Cell 49 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,36,288,Cell 50 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,37,296,Cell 50 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,38,304,Cell 51 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,39,312,Cell 51 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,40,320,Cell 52 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,41,328,Cell 52 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,42,336,Cell 53 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,43,344,Cell 53 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,44,352,Cell 54 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,45,360,Cell 54 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,46,368,Cell 55 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,47,376,Cell 55 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,48,384,Cell 56 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,49,392,Cell 56 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,50,400,Cell 57 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,51,408,Cell 57 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,52,416,Cell 58 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,53,424,Cell 58 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,54,432,Cell 59 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,55,440,Cell 59 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,56,448,Cell 60 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,57,456,Cell 60 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,58,464,Cell 61 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,59,472,Cell 61 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,60,480,Cell 62 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,61,488,Cell 62 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
,,,62,496,Cell 63 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,14,Done
,,,63,504,Cell 63 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,14,Done
0x00F,ACU Cell Data 3,64,,,,,,,,,,,,15,
,,,0,0,Cell 64 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,1,8,Cell 64 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,2,16,Cell 65 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,3,24,Cell 65 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,4,32,Cell 66 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,5,40,Cell 66 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,6,48,Cell 67 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,7,56,Cell 67 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,8,64,Cell 68 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,9,72,Cell 68 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,10,80,Cell 69 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,11,88,Cell 69 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,12,96,Cell 70 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,13,104,Cell 70 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,14,112,Cell 71 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,15,120,Cell 71 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,16,128,Cell 72 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,17,136,Cell 72 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,18,144,Cell 73 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,19,152,Cell 73 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,20,160,Cell 74 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,21,168,Cell 74 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,22,176,Cell 75 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,23,184,Cell 75 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,24,192,Cell 76 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,25,200,Cell 76 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,26,208,Cell 77 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,27,216,Cell 77 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,28,224,Cell 78 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,29,232,Cell 78 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,30,240,Cell 79 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,31,248,Cell 79 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,32,256,Cell 80 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,33,264,Cell 80 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,34,272,Cell 81 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,35,280,Cell 81 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,36,288,Cell 82 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,37,296,Cell 82 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,38,304,Cell 83 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,39,312,Cell 83 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,40,320,Cell 84 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,41,328,Cell 84 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,42,336,Cell 85 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,43,344,Cell 85 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,44,352,Cell 86 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,45,360,Cell 86 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,46,368,Cell 87 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,47,376,Cell 87 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,48,384,Cell 88 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,49,392,Cell 88 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,50,400,Cell 89 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,51,408,Cell 89 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,52,416,Cell 90 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,53,424,Cell 90 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,54,432,Cell 91 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,55,440,Cell 91 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,56,448,Cell 92 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,57,456,Cell 92 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,58,464,Cell 93 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,59,472,Cell 93 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,60,480,Cell 94 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,61,488,Cell 94 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
,,,62,496,Cell 95 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,15,
,,,63,504,Cell 95 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,15,
0x010,ACU Cell Data 4,64,,,,,,,,,,,,16,
,,,0,0,Cell 96 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,1,8,Cell 96 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,2,16,Cell 97 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,3,24,Cell 97 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,4,32,Cell 98 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,5,40,Cell 98 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,6,48,Cell 99 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,7,56,Cell 99 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,8,64,Cell 100 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,9,72,Cell 100 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,10,80,Cell 101 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,11,88,Cell 101 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,12,96,Cell 102 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,13,104,Cell 102 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,14,112,Cell 103 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,15,120,Cell 103 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,16,128,Cell 104 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,17,136,Cell 104 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,18,144,Cell 105 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,19,152,Cell 105 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,20,160,Cell 106 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,21,168,Cell 106 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,22,176,Cell 107 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,23,184,Cell 107 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,24,192,Cell 108 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,25,200,Cell 108 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,26,208,Cell 109 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,27,216,Cell 109 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,28,224,Cell 110 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,29,232,Cell 110 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,30,240,Cell 111 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,31,248,Cell 111 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,32,256,Cell 112 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,33,264,Cell 112 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,34,272,Cell 113 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,35,280,Cell 113 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,36,288,Cell 114 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,37,296,Cell 114 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,38,304,Cell 115 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,39,312,Cell 115 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,40,320,Cell 116 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,41,328,Cell 116 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,42,336,Cell 117 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,43,344,Cell 117 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,44,352,Cell 118 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,45,360,Cell 118 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,46,368,Cell 119 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,47,376,Cell 119 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,48,384,Cell 120 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,49,392,Cell 120 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,50,400,Cell 121 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,51,408,Cell 121 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,52,416,Cell 122 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,53,424,Cell 122 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,54,432,Cell 123 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,55,440,Cell 123 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,56,448,Cell 124 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,57,456,Cell 124 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,58,464,Cell 125 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,59,472,Cell 125 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,60,480,Cell 126 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,61,488,Cell 126 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
,,,62,496,Cell 127 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,16,
,,,63,504,Cell 127 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,16,
0x011,ACU Cell Data 5,64,,,,,,,,,,,,17,
,,,0,0,Cell 128 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,1,8,Cell 128 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,2,16,Cell 129 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,3,24,Cell 129 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,4,32,Cell 130 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,5,40,Cell 130 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,6,48,Cell 131 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,7,56,Cell 131 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,8,64,Cell 132 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,9,72,Cell 132 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,10,80,Cell 133 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,11,88,Cell 133 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,12,96,Cell 134 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,13,104,Cell 134 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,14,112,Cell 135 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,15,120,Cell 135 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,16,128,Cell 136 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,17,136,Cell 136 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,18,144,Cell 137 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,19,152,Cell 137 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,20,160,Cell 138 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,21,168,Cell 138 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,22,176,Cell 139 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,23,184,Cell 139 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,24,192,Cell 140 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,25,200,Cell 140 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,26,208,Cell 141 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,27,216,Cell 141 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,28,224,Cell 142 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,29,232,Cell 142 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,30,240,Cell 143 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,31,248,Cell 143 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,32,256,Cell 144 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,33,264,Cell 144 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,34,272,Cell 145 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,35,280,Cell 145 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,36,288,Cell 146 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,37,296,Cell 146 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,38,304,Cell 147 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,39,312,Cell 147 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,40,320,Cell 148 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,41,328,Cell 148 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,42,336,Cell 149 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,43,344,Cell 149 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,44,352,Cell 150 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,45,360,Cell 150 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,46,368,Cell 151 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,47,376,Cell 151 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,48,384,Cell 152 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,49,392,Cell 152 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,50,400,Cell 153 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,51,408,Cell 153 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,52,416,Cell 154 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,53,424,Cell 154 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,54,432,Cell 155 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,55,440,Cell 155 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,56,448,Cell 156 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,57,456,Cell 156 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,58,464,Cell 157 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,59,472,Cell 157 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,60,480,Cell 158 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,61,488,Cell 158 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
,,,62,496,Cell 159 Voltage,Cell  n voltage,u8,0,255,2,4.55,0.01x+2,Volts,17,
,,,63,504,Cell 159 Temp,Cell n temperature,u8,0,255,0,63.75,0.25x,Celsius,17,
0x012,DC-DC Status,7,,,,,,,,,,,,18,
,,,0-1,0,Input Voltage,~20v for LV (LV only. Send 0 for
HV),u16,0,65535,0,65.535,x/1000,Volts,18,Done
,,,2-3,16,Output Voltage,~12v for LV and ~20v for
HV,u16,0,65535,0,65.535,x/1000,Volts,18,Done
,,,4,32,Input Current,Input current (LV only. Send 0 for
HV),u8,0,255,0,25.5,0.1x,Amps,18,Done
,,,5,40,Output Current,Output current,u8,0,255,0,25.5,0.1x,Amps,18,Done
,,,6,48,DC-DC Temp,Temp of DC-DC converter,u8,0,255,0,255,1x,Celsius,18,Done
0x013,Inverter Status 1,6,,,,,,,,,,,,19,
,,,0-1,0,AC current,"0.01 * current,
int16_t",u16,0,65535,-327.68,327.67,0.01x-327.68,Amps,19,
,,,2-3,16,DC current,"0.01 * current,
int16_t",u16,0,65535,0,655.35,0.01x,Amps,19,
,,,4-5,32,Motor RPM,"RPM, int16_t",u16,0,65535,-32768,32767,x-32768,RPM,19,
0x014,Inverter Status 2,6,,,,,,,,,,,,20,
,,,0-1,0,U MOSFET temperature,"Celsius + 40,
uint8_t",u8,0,255,-40,215,x-40,Celsius,20,
,,,2-3,16,V MOSFET temperature,"Celsius + 40,
uint8_t",u8,0,255,-40,215,x-40,Celsius,20,
,,,4-5,32,W MOSFET temperature,"Celsius + 40,
uint8_t",u8,0,255,-40,215,x-40,Celsius,20, 0x015,Inverter Status
3,2,,,,,,,,,,,,21,
,,,0,0,Motor temperature,"Celsius + 40,
uint8_t",u8,0,255,-40,215,x-40,Celsius,21,
,,,1,16,Over voltage faults,TS above set max voltage,b,0,1,~,~,~,Bool,21,
,,,,17,Under voltage fault,TS below set min voltage,b,0,1,~,~,~,Bool,21,
,,,,18,Inv. overtemp fault,Inverter over set max temp,b,0,1,~,~,~,Bool,21,
,,,,19,Motor overtemp fault,Motor over set max temp,b,0,1,~,~,~,Bool,21,
,,,,20,Transistor fault,Mosfet or mosfet drive error,b,0,1,~,~,~,Bool,21,
,,,,21,Encoder fault,Encoder communication or calc error,b,0,1,~,~,~,Bool,21,
,,,,22,CAN fault,CAN message error or timeout,b,0,1,~,~,~,Bool,21,
,,,,23,Future use,,b,0,1,~,~,~,Bool,21,
0x016,Inverter Config,7,,,,,,,,,,,,22,
,,,0-1,0,Max AC Current,Max AC
Current,u16,0,65535,-327.68,327.67,0.01x-327.68,Amps,22,
,,,2-3,16,Max DC Current,Max DC
Current,u16,0,65535,-327.68,327.67,0.01x-327.68,Amps,22,
,,,4-5,32,Absolute Max RPM Limit,0: No limit      n :limited at n
RPM,u16,0,65535,-32768,32767,x-32768,RPM,22,
,,,6,48,Motor direction,Write 1 inverts direction,b,0,1,-,-,-,Enable,22,
0x017,Inverter Command,8,,,,,,,,,,,,23,
,,,0-1,0,Set AC Current,Commanded AC
Current,u16,0,65535,-327.68,327.67,0.01x-327.68,Amps,23,
,,,2-3,16,Set DC Current,Commanded DC
Current,u16,0,65535,-327.68,327.67,0.01x-327.68,Amps,23,
,,,4-5,32,RPM Limit,0: No limit      n :limited at n
RPM,u16,0,65535,-32768,32767,x-32768,RPM,23,
,,,6,48,Field weakening,Field weakening strength,u8,0,255,0,25.5,0.1x,Amps,23,
,,,7,56,Drive enable,Write this to 1 every 100ms to enable
inverter,b,0,1,-,-,-,Enable,23, 0x018,Fan Status,4,,,,,,,,,,,,24,
,,,0-1,0,Fan Speed,Fan RPM,u16,0,65535,0,65535,1x,RPM,24,
,,,2,16,Input Voltage,0-22,u8,0,255,0,25.5,0.1x,Volts,24,
,,,3,24,Input Current,0-10,u8,0,255,0,25.5,0.1x,Amps,24,
0x019,Fan Command,1,,,,,,,,,,,,25,
,,,0,0,Fan Command,0-100 Percent,u8,0,255,0,255,1x,%,25,
0x01A,Dash Status,3,,,,,,,,,,,,26,
,,,0,0,BMS LED,State of BMS LED,b,0,1,~,~,~,Bool,26,
,,,,1,IMD LED,State of IMD LED,b,0,1,~,~,~,Bool,26,
,,,,2,BSPD LED,State of BSPD LED,b,0,1,~,~,~,Bool,26,
,,,1,8,TS Button Data,"MSB is state (1: pressed)
Other 7 bits represent the time in 0.1s that it has been in that
state",s8,-128,127,0,12.7,abs(x)/10,Seconds,26,
,,,2,16,RTD Button Data,"MSB is state (1: pressed)
Other 7 bits represent the time in 0.1s that it has been in that
state",s8,-128,127,0,12.7,abs(x)/10,Seconds,26, 0x01B,Dash
Config,7,,,,,,,,,,,,27,
,,,0,0,BMS LED,"LED command (0: off, 1: on)",b,0,1,~,~,~,Bool,27,
,,,,1,IMD LED,"LED command (0: off, 1: on)",b,0,1,~,~,~,Bool,27,
,,,,2,BSPD LED,"LED command (0: off, 1: on)",b,0,1,~,~,~,Bool,27,
,,,1,8,Button LED 1 R,"Discarded input but still send it, set by ECU
State",u8,0,255,0,255,1x,RGB,27,
,,,2,16,Button LED 1 G,"Discarded input but still send it, set by ECU
State",u8,0,255,0,255,1x,RGB,27,
,,,3,24,Button LED 1 B,"Discarded input but still send it, set by ECU
State",u8,0,255,0,255,1x,RGB,27,
,,,4,32,Button LED 2 R,"Discarded input but still send it, set by ECU
State",u8,0,255,0,255,1x,RGB,27,
,,,5,40,Button LED 2 G,"Discarded input but still send it, set by ECU
State",u8,0,255,0,255,1x,RGB,27,
,,,6,48,Button LED 2 B,"Discarded input but still send it, set by ECU
State",u8,0,255,0,255,1x,RGB,27, 0x01C,Steering Status,2,,,,,,,,,,,,28,
,,,0,0,Current Encoder,Position of knob (1-16),u4,0,15,1,16,1x,Position,28,
,,,,4,Torque Map Encoder,Position of knob (1-16),u4,0,15,1,16,1x,Position,28,
,,,1,8,Regen,Position of knob (1-16),u4,0,15,1,16,1x,Position,28,
,,,,12,Button 1,Button State,b,0,1,~,~,~,Bool,28,
,,,,13,Button 2,Button State,b,0,1,~,~,~,Bool,28,
,,,,14,Button 3,Button State,b,0,1,~,~,~,Bool,28,
,,,,15,Button 4,Button State,b,0,1,~,~,~,Bool,28,
0x01D,Steering Config,0,,,,,,,,,,,,29,
,,,0,0,Reserved,Theoretically Steering wheel will just update based on ECU
status,,,,,,,,29, 0x01E,SAM Brake IR,1,,,,,,,,,,,,30,
,,,0,0,Temp,IR Temp of Brakes,u8,0,255,0,255,1x,Celsius,30,
0x01F,SAM Tire Temp,4,,,,,,,,,,,,31,
,,,0,0,Outside Temp,Furthest from chassis,u8,0,255,0,255,1x,Celsius,31,
,,,1,8,Outside Middle Temp,Middle of tire,u8,0,255,0,255,1x,Celsius,31,
,,,2,16,Inside Middle Temp,Middle of tire,u8,0,255,0,255,1x,Celsius,31,
,,,3,24,Inside Temp,Closest to chassis,u8,0,255,0,255,1x,Celsius,31,
0x020,SAM IMU,12,,,,,,,,,,,,32,
,,,0-1,0,Accel X,Acceleration in
X-axis,u16,0,65535,-327.68,327.67,0.01x-327.68,Meters/s^2,32,
,,,2-3,16,Accel Y,Acceleration in
Y-axis,u16,0,65535,-327.68,327.67,0.01x-327.68,Meters/s^2,32,
,,,4-5,32,Accel Z,Acceleration in
Z-axis,u16,0,65535,-327.68,327.67,0.01x-327.68,Meters/s^2,32,
,,,6-7,48,Gyro X,Angular velocity in
X-axis,u16,0,65535,-32.768,32.767,0.001x-32.768,Meters/s^2,32,
,,,8-9,64,Gyro Y,Angular velocity in
Y-axis,u16,0,65535,-32.768,32.767,0.001x-32.768,Meters/s^2,32,
,,,10-11,80,Gyro Z,Angular velocity in
Z-axis,u16,0,65535,-32.768,32.767,0.001x-32.768,Meters/s^2,32, 0x021,SAM GPS
1,8,,,,,,,,,,,,33,
,,,0-3,0,Lat,Latitude in decimal degrees,u32,0,,,,,Degrees,33,
,,,4-7,32,Long,Longitude in decimal degrees,u32,0,,,,,Degrees,33,
0x022,SAM GPS 2,8,,,,,,,,,,,,34,
,,,0-3,0,Accuracy,GPS position accuracy,u32,0,,,,,,34,
,,,4-7,32,Attitude,Vehicle attitude,u32,0,,,,,,34,
0x023,SAM GPS Time,8,,,,,,,,,,,,35,
,,,0-3,0,Time,Time in seconds since GPS Epoch,u32,0,,,,,,35,
,,,4-7,32,Time of Week Ms,Time of week in milliseconds,u32,0,,,,,,35,
0x024,SAM GPS Heading,4,,,,,,,,,,,,36,
,,,0-3,0,Heading from North,Heading angle relative to true North,u32,0,,,,,,36,
0x025,SAM Sus Pots,1,,,,,,,,,,,,37,
,,,0,0,Suspension Angle,Pot Pos,u8,0,255,0,255,1x,degrees,37,
0x026,SAM TOF,2,,,,,,,,,,,,38,
,,,0-1,0,Height,Ride Height,u16,0,65535,0,255,x/256,mm,38,
0x027,SAM Rear Wheelspeed,2,,,,,,,,,,,,39,
,,,0-1,0,Speed,Wheel RPM,u16,0,65535,-3276.8,3276.7,0.1x--3276.8,RPM,39,
0x028,SAM Pushrod Force,2,,,,,,,,,,,,40,
,,,0-1,0,Load Force,Pushrod Force,u16,0,65535,-3276.8,3276.7,0.1x--3276.8,N,40,
0x029,TCM Status,8,,,,,,,,,,,,41,
,,,0,0,Connection Status,"1: OK, 0: Timeout",b,0,1,,,,Bool,41,
,,,,1,MQTT Status,"1: OK, 0: Timeout",b,0,1,,,,Bool,41,
,,,,2,Epic Shelter Status,"1: In Progress, 0: Idle",b,0,1,,,,Bool,41,
,,,,3,Camera Status,"1: Recording, 0: Idle",b,0,1,,,,Bool,41,
,,,,4-7,Reserved,Reserved,,,,,,,,41,
,,,1-2,8,Ping,Mapache ping (upload),u16,0,65535,,,,ms,41,
,,,3-6,24,Cache Size,# of messages on cache
(non-synced),u32,0,"4,294,967,296",,,,#,41,
,,,7,56,Reserved,,,,,,,,,41,
0x02A,TCM Resource Utilization,44,,,,,,,,,,,,42,
,,,0-1,,CPU 0 Freq,core 0 frequency in MHz,u16,,,,,,%,42,
,,,2,,CPU 0 Util,core 0 utilization in %,u8,,,,,,%,42,
,,,3-4,,CPU 1 Freq,core 1 frequency in MHz,u16,,,,,,%,42,
,,,5,,CPU 1 Util,core 1 utilization in %,u8,,,,,,,42,
,,,6-7,,CPU 2 Freq,core 2 frequency in MHz,u16,,,,,,%,42,
,,,8,,CPU 2 Util,core 2 utilization in %,u8,,,,,,,42,
,,,9-10,,CPU 3 Freq,core 3 frequency in MHz,u16,,,,,,,42,
,,,11,,CPU 3 Util,core 3 utilization in %,u8,,,,,,,42,
,,,12-13,,CPU 4 Freq,core 4 frequency in MHz,u16,,,,,,,42,
,,,14,,CPU 4 Util,core 4 utilization in %,u8,,,,,,,42,
,,,15-16,,CPU 5 Freq,core 5 frequency in MHz,u16,,,,,,,42,
,,,17,,CPU 5 Util,core 5 utilization in %,u8,,,,,,Watts,42,
,,,18,,CPU Total Util,total cpu utilization in %,u8,,,,,,,42,
,,,19-20,,RAM Total,total memory in MB,u16,,,,,,,42,
,,,21-22,,RAM Used,used memory in MB,u16,,,,,,,42,
,,,23,,RAM Util,memory utilization in %,u8,,,,,,,42,
,,,24,,GPU Util,gpu utilization in %,u8,,,,,,,42,
,,,25-26,,GPU Freq,gpu frequency in MHz,u16,,,,,,,42,
,,,27-30,,Disk Total,total disk space in MB,u32,,,,,,,42,
,,,31-34,,Disk Used,used disk space in MB,u32,,,,,,,42,
,,,35,,Disk Util,disk utilization in %,u8,,,,,,Celsius,42,
,,,36,,CPU Temp,cpu temp in ˚C,u8,,,,,,,42,
,,,37,,GPU Temp,gpu temp in ˚C,u8,,,,,,,42,
,,,38-39,,Voltage Draw,voltage draw in mV,u16,,,,,,,42,
,,,40-41,,Current Draw,current draw in mA,u16,,,,,,,42,
,,,42-43,,Power Draw,power draw in mW,u16,,,,,,Celsius,42,
0x02B,Dash Warning Flags,1,,,,,,,,,,,,43,
,,,0,0,BSE APPS Violation,"1: Violation, 0: OK",b,0,1,,,,Bool,43,
,,,,1,RESERVED,RESERVED,,,,,,,,43,
,,,,2,RESERVED,RESERVED,,,,,,,,43,
,,,,3,RESERVED,RESERVED,,,,,,,,43,
,,,,4,RESERVED,RESERVED,,,,,,,,43,
,,,,5,RESERVED,RESERVED,,,,,,,,43,
,,,,6,RESERVED,RESERVED,,,,,,,,43,
,,,,7,RESERVED,RESERVED,,,,,,,,43,
0x02C,Specific Brake IR,2,,,,,,,,,,,,44,
,,,0,0,Wheel identifier,Wheel identifier according to the wiki,u8,0,255,,,,,44,
,,,1,8,Temp,IR Temp of Brakes,u8,0,255,0,255,1x,Celsius,44,
0x02D,ECU Ping Information,3,,,,,,,,,,,,45,
,,,0-2,0,Online pings,Literal copy of ECU Status's status bit
map,b[24],,,,,,Bool map,45, 0x02E,ECU Pedals Data,10,,,,,,,,,,,,46,
,,,0-1,0,APPS1_SIGNAL,APPS 1 Signal,u16,0,65535,0,100,x/655.35,%,46,
,,,2-3,16,APPS2_SIGNAL,APPS 2 Signal,u16,0,65535,0,100,x/655.35,%,46,
,,,4-5,32,BSE_SIGNAL,Brake Force Signal,u16,0,65535,0,100,x/655.35,%,46,
,,,6-8,48,BRAKE_F_SIGNAL,Brake Pressure Signal,u16,0,65535,0,100,x/655.35,%,46,
,,,6-9,48,BRAKE_R_SIGNAL,Brake Pressure Signal,u16,0,65535,0,100,x/655.35,%,46,
0x031,GPS LAT,8,,,,,,,,,,,,47,
,,,0-8,0,LAT,lattitude ,double,,,,,,,47,
0x032,GPS LON,8,,,,,,,,,,,,48,
,,,0-8,0,LON,longitude ,double,,,,,,,48,
0x033,GPS ALT,8,,,,,,,,,,,,49,
,,,0-8,0,ALT,altitude ,double,,,,,,,49,
0x034,GPS PX,8,,,,,,,,,,,,50,
,,,0-1,0,Theta,,i16,,,,,x/1000,,50,
,,,2-3,16,Acc,,i16,,,,,x/100,,50,
,,,4-7,32,status,,u32,,,,,,,50,
0x035,GPS QY,8,,,,,,,,,,,,51,
,,,0-1,0,Theta,,i16,,,,,x/1000,,51,
,,,2-3,16,Acc,,i16,,,,,x/100,,51,
,,,4-7,32,status,,u32,,,,,,,51,
0x036,GPS RZ,8,,,,,,,,,,,,52,
,,,0-1,0,Theta,,i16,,,,,x/1000,,52,
,,,2-3,16,Acc,,i16,,,,,x/100,,52,
,,,4-7,32,status,,u32,,,,,,,52,
0x030,UVW DGPS,6,,,,,,,,,,,,53,
,,,0-1,0,DGPS_U,U,i16,,,,,x/100,,53,
,,,2-3,16,DGPS_V,V,i16,,,,,x/100,,53,
,,,4-5,32,DGPS_W,W,i16,,,,,x/100,,53,
,,,,,,,,,,,,,,54,
,,,,,,,,,,,,,,55,
,,,,,,,,,,,,,,56,
,,,,,,,,,,,,,,57,
,,,,,,,,,,,,,,58,
,,,,,,,,,,,,,,59,
,,,,,,,,,,,,,,60,
,,,,,,,,,,,,,,61,
,,,,,,,,,,,,,,62,
,,,,,,,,,,,,,,63,
,,,,,,,,,,,,,,64,
,,,,,,,,,,,,,,65,
,,,,,,,,,,,,,,66,
,,,,,,,,,,,,,,67,
,,,,,,,,,,,,,,68,
,,,,,,,,,,,,,,69,
,,,,,,,,,,,,,,70,
,,,,,,,,,,,,,,71,
,,,,,,,,,,,,,,72,
,,,,,,,,,,,,,,73,
,,,,,,,,,,,,,,74,
,,,,,,,,,,,,,,75,
,,,,,,,,,,,,,,76,
,,,,,,,,,,,,,,77,
,,,,,,,,,,,,,,78,
,,,,,,,,,,,,,,79,
,,,,,,,,,,,,,,80,
,,,,,,,,,,,,,,81,
,,,,,,,,,,,,,,82,
,,,,,,,,,,,,,,83,
,,,,,,,,,,,,,,84,
,,,,,,,,,,,,,,85,
,,,,,,,,,,,,,,86,
,,,,,,,,,,,,,,87,
,,,,,,,,,,,,,,88,
,,,,,,,,,,,,,,89,
,,,,,,,,,,,,,,90,
,,,,,,,,,,,,,,91,
,,,,,,,,,,,,,,92,
,,,,,,,,,,,,,,93,
,,,,,,,,,,,,,,94,
,,,,,,,,,,,,,,95,
,,,,,,,,,,,,,,96,
,,,,,,,,,,,,,,97,
,,,,,,,,,,,,,,98,
,,,,,,,,,,,,,,99,
,,,,,,,,,,,,,,100,
,,,,,,,,,,,,,,101,
,,,,,,,,,,,,,,102,
,,,,,,,,,,,,,,103,
,,,,,,,,,,,,,,104,
,,,,,,,,,,,,,,105,
,,,,,,,,,,,,,,106,
,,,,,,,,,,,,,,107,
,,,,,,,,,,,,,,108,
,,,,,,,,,,,,,,109,
,,,,,,,,,,,,,,110,
,,,,,,,,,,,,,,111,
,,,,,,,,,,,,,,112,
,,,,,,,,,,,,,,113,
,,,,,,,,,,,,,,114,
,,,,,,,,,,,,,,115,
,,,,,,,,,,,,,,116,
,,,,,,,,,,,,,,117,
,,,,,,,,,,,,,,118,
,,,,,,,,,,,,,,119,
,,,,,,,,,,,,,,120,
,,,,,,,,,,,,,,121,
,,,,,,,,,,,,,,122,
,,,,,,,,,,,,,,123,
,,,,,,,,,,,,,,124,
,,,,,,,,,,,,,,125,
,,,,,,,,,,,,,,126,
,,,,,,,,,,,,,,127,
,,,,,,,,,,,,,,128,
,,,,,,,,,,,,,,129,
,,,,,,,,,,,,,,130,
,,,,,,,,,,,,,,131,
,,,,,,,,,,,,,,132,
,,,,,,,,,,,,,,133,
,,,,,,,,,,,,,,134,
,,,,,,,,,,,,,,135,
,,,,,,,,,,,,,,136,
,,,,,,,,,,,,,,137,
,,,,,,,,,,,,,,138,
,,,,,,,,,,,,,,139,
*/
