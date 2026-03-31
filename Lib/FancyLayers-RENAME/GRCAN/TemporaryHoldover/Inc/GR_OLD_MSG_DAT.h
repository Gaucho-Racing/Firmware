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
	uint8_t ecu_status;	      /** ECU state (see diagram) */
	uint8_t ping_block[3];	      /** Node timeout status bits (1=OK, 0=Timeout) */
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

/** BCU Status 1 */
typedef struct {
	uint16_t tractivebattery_voltage; /** All cells sum, 0.01V */
	uint16_t ts_voltage;		  /** TS output voltage, 0.01V */
	uint16_t tractivebattery_current; /** Current out (i16), 0.01A */
	uint8_t tractivebattery_soc;	  /** Accum SoC (lowest cell), 20x/51=% */
	uint8_t glv_soc;		  /** GLV SoC, 20x/51=% */
} GR_OLD_BCU_STATUS_1_MSG;

/** BCU Status 2 */
typedef struct {
	uint8_t voltage_20v;	  /** 20V GLV, 0.1V */
	uint8_t voltage_12v;	  /** 12V supply, 0.1V */
	uint8_t voltage_sdc;	  /** SDC voltage before latch, 0.1V */
	uint8_t voltage_min_cell; /** Min cell V, 0.01x+2 V */
	uint8_t max_cell_temp;	  /** Hottest cell, 0.25°C */
	uint8_t error_bits;	  /** Error flags (temp/V/I) */
	uint8_t precharge_bits;	  /** Precharge & relay states */
} GR_OLD_BCU_STATUS_2_MSG;

/** BCU Status 3 */
typedef struct {
	uint16_t hv_input_voltage;  /** 600V input, 0.01V */
	uint16_t hv_output_voltage; /** 20V output, 0.01V */
	uint16_t hv_input_current;  /** 600V input, 0.001A */
	uint16_t hv_output_current; /** 20V output, 0.001A */
} GR_OLD_BCU_STATUS_3_MSG;

/** BCU Precharge cmd */
typedef struct {
	uint8_t precharge; /** Set TS active (0=shutdown, 1=precharge) */
} GR_OLD_BCU_PRECHARGE_MSG;

/** BCU Config Charge Params */
typedef struct {
	uint16_t charge_voltage; /** Target charge V, 0.1V */
	uint16_t charge_current; /** Target charge I, 0.1A */
} GR_OLD_BCU_CONFIG_CHARGE_PARAMS_MSG;

/** BCU Config Operational Params */
typedef struct {
	uint8_t min_cell_voltage; /** Min cell V thresh, 0.01x+2 V */
	uint8_t max_cell_voltage; /** Max cell temp thresh, 0.25°C */
} GR_OLD_BCU_CONFIG_OPS_PARAMS_MSG;

/** BCU Cell Data 1 - Cells 0-31 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	} cells[32];
} GR_OLD_BCU_CELL_DATA_1_MSG;

/** BCU Cell Data 2 - Cells 32-63 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	} cells[32];
} GR_OLD_BCU_CELL_DATA_2_MSG;

/** BCU Cell Data 3 - Cells 64-95 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	} cells[32];
} GR_OLD_BCU_CELL_DATA_3_MSG;

/** BCU Cell Data 4 - Cells 96-127 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	} cells[32];
} GR_OLD_BCU_CELL_DATA_4_MSG;

/** BCU Cell Data 5 - Cells 128-159 */
typedef struct {
	struct {
		uint8_t voltage;     /** Cell V, 0.01x+2 V */
		uint8_t temperature; /** Cell temp, 0.25°C */
	} cells[32];
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
	uint8_t led_bits;   /** BMS/IMD/BSPD LED states */
	uint8_t ts_button;  /** TS button state & time (MSB=pressed, 7b=0.1s) */
	uint8_t rtd_button; /** RTD button state & time (MSB=pressed, 7b=0.1s) */
} GR_OLD_DASH_STATUS_MSG;

typedef struct {
	uint8_t led_bits;   /** BMS/IMD/BSPD LED states */
	uint8_t button_bits;
} GR_MEDIUM_DASH_STATUS_MSG;

/** Dashboard Config */
typedef struct {
	uint8_t led_bits; /** BMS/IMD/BSPD LED cmds (0=off, 1=on) */
} GR_OLD_DASH_CONFIG_MSG;

/** Steering Wheel Status */
typedef struct {
	uint8_t encoder_bits;	       /** Current encoder pos (1-16) & torque map (1-16) */
	uint8_t regen_and_button_bits; /** Regen pos (1-16) & 4 button states */
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
