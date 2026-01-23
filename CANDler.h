/* Auto-generated header file */
#ifndef GR_OLD_MESSAGES_H
#define GR_OLD_MESSAGES_H

#include <stdint.h>

/** Debug 2.0 */
typedef struct {
	uint8_t    debug;                         /** Byte 0 */
} GR_OLD_D_2_0_MSG;

/** Debug FD */
typedef struct {
	uint8_t    debug;                         /** Byte 0 */
} GR_OLD_D_FD_MSG;

/** Ping */
typedef struct {
	uint32_t   timestamp;                     /** Byte 0 */
} GR_OLD_P_MSG;

/** ECU Status 1 */
typedef struct {
	uint8_t    ecu_state;                     /** Byte 0 */
	uint8_t    ping_block[3];
	uint8_t    power_level_torque_map;        /** Byte 4 */
	uint8_t    max_cell_temp;                 /** Byte 5 */
	uint8_t    accumulator_state_of_charge;   /** Byte 6 */
	uint8_t    glv_state_of_charge;           /** Byte 7 */
} GR_OLD_ECU_S_1_MSG;

/** ECU Status 2 */
typedef struct {
	uint16_t   tractive_system_voltage;       /** Byte 0 */
	uint16_t   vehicle_speed;                 /** Byte 2 */
	uint16_t   fr_wheel_rpm;                  /** Byte 4 */
	uint16_t   fl_wheel_rpm;                  /** Byte 6 */
} GR_OLD_ECU_S_2_MSG;

/** ECU Status 3 */
typedef struct {
	uint16_t   rr_wheel_rpm;                  /** Byte 0 */
	uint16_t   rl_wheel_rpm;                  /** Byte 2 */
} GR_OLD_ECU_S_3_MSG;

/** ECU config */
typedef struct {
	uint8_t    reserved;                      /** Byte 0 */
} GR_OLD_ECU_MSG;

/** ACU Status 1 */
typedef struct {
	uint16_t   accumulator_voltage;           /** Byte 0 */
	uint16_t   ts_voltage;                    /** Byte 2 */
	uint16_t   accumulator_current;           /** Byte 4 */
	uint8_t    accumulator_soc;               /** Byte 6 */
	uint8_t    glv_soc;                       /** Byte 7 */
} GR_OLD_ACU_S_1_MSG;

/** ACU Status 2 */
typedef struct {
	uint8_t    _20v_voltage;                  /** Byte 0 */
	uint8_t    _12v_voltage;                  /** Byte 1 */
	uint8_t    sdc_voltage;                   /** Byte 2 */
	uint8_t    min_cell_voltage;              /** Byte 3 */
	uint8_t    max_cell_temp;                 /** Byte 4 */
	uint8_t    error_fault_violation_bits[2];
} GR_OLD_ACU_S_2_MSG;

/** ACU Status 3 */
typedef struct {
	uint16_t   hv_input_voltage;              /** Byte 0 */
	uint16_t   hv_output_voltage;             /** Byte 2 */
	uint16_t   hv_input_current;              /** Byte 4 */
	uint16_t   hv_output_current;             /** Byte 6 */
} GR_OLD_ACU_S_3_MSG;

/** ACU Precharge */
typedef struct {
	uint8_t    set_ts_active;                 /** Byte 0 */
} GR_OLD_ACU_P_MSG;

/** ACU Config Charge Parameters */
typedef struct {
	uint16_t   charge_voltage;                /** Byte 0 */
	uint16_t   charge_current;                /** Byte 2 */
} GR_OLD_ACU_C_C_P_MSG;

/** ACU Config Operational Parameters */
typedef struct {
	uint8_t    minimium_cell_voltage;         /** Byte 0 */
	uint8_t    max_cell_temperature;          /** Byte 1 */
} GR_OLD_ACU_C_O_P_MSG;

/** ACU Cell Data 1 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_C_D_1_MSG;

/** ACU Cell Data 2 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_C_D_2_MSG;

/** ACU Cell Data 3 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_C_D_3_MSG;

/** ACU Cell Data 4 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_C_D_4_MSG;

/** ACU Cell Data 5 */
typedef struct {
	struct {
		uint8_t voltage;
		uint8_t temperature;
	} cells[32];
} GR_OLD_ACU_C_D_5_MSG;

/** DC-DC Status */
typedef struct {
	uint16_t   input_voltage;                 /** Byte 0 */
	uint16_t   output_voltage;                /** Byte 2 */
	uint8_t    input_current;                 /** Byte 4 */
	uint8_t    output_current;                /** Byte 5 */
	uint8_t    dc_dc_temp;                    /** Byte 6 */
} GR_OLD_DC_DC_S_MSG;

/** Inverter Status 1 */
typedef struct {
	uint16_t   ac_current;                    /** Byte 0 */
	uint16_t   dc_current;                    /** Byte 2 */
	uint16_t   motor_rpm;                     /** Byte 4 */
} GR_OLD_I_S_1_MSG;

/** Inverter Status 2 */
typedef struct {
	uint8_t    u_mosfet_temperature;          /** Byte 0 */
	uint8_t    v_mosfet_temperature;          /** Byte 2 */
	uint8_t    w_mosfet_temperature;          /** Byte 4 */
} GR_OLD_I_S_2_MSG;

/** Inverter Status 3 */
typedef struct {
	uint8_t    motor_temperature;             /** Byte 0 */
	uint8_t    error_fault_violation_bits;
} GR_OLD_I_S_3_MSG;

/** Inverter Config */
typedef struct {
	uint16_t   max_ac_current;                /** Byte 0 */
	uint16_t   max_dc_current;                /** Byte 2 */
	uint16_t   absolute_max_rpm_limit;        /** Byte 4 */
	uint8_t    motor_direction;               /** Byte 6 */
} GR_OLD_I_C_MSG;

/** Inverter Command */
typedef struct {
	uint16_t   set_ac_current;                /** Byte 0 */
	uint16_t   set_dc_current;                /** Byte 2 */
	uint16_t   rpm_limit;                     /** Byte 4 */
	uint8_t    field_weakening;               /** Byte 6 */
	uint8_t    drive_enable;                  /** Byte 7 */
} GR_OLD_I_C_MSG;

/** Fan Status */
typedef struct {
	uint16_t   fan_speed;                     /** Byte 0 */
	uint8_t    input_voltage;                 /** Byte 2 */
	uint8_t    input_current;                 /** Byte 3 */
} GR_OLD_F_S_MSG;

/** Fan Command */
typedef struct {
	uint8_t    fan_command;                   /** Byte 0 */
} GR_OLD_F_C_MSG;

/** Dash Status */
typedef struct {
	uint8_t    ping_block;
	uint8_t    ts_button_data;                /** Byte 1 */
	uint8_t    rtd_button_data;               /** Byte 2 */
} GR_OLD_D_S_MSG;

/** Dash Config */
typedef struct {
	uint8_t    ping_block;
	uint8_t    button_led_1_r;                /** Byte 1 */
	uint8_t    button_led_1_g;                /** Byte 2 */
	uint8_t    button_led_1_b;                /** Byte 3 */
	uint8_t    button_led_2_r;                /** Byte 4 */
	uint8_t    button_led_2_g;                /** Byte 5 */
	uint8_t    button_led_2_b;                /** Byte 6 */
} GR_OLD_D_C_MSG;

/** Steering Status */
typedef struct {
	uint8_t    current_encoder_torque_map_encoder; /** Byte 0 */
	uint8_t    ping_block;
} GR_OLD_S_S_MSG;

/** Steering Config */
typedef struct {
	uint8_t    reserved;                      /** Byte 0 */
} GR_OLD_S_C_MSG;

/** SAM Brake IR */
typedef struct {
	uint8_t    temp;                          /** Byte 0 */
} GR_OLD_SAM_B_IR_MSG;

/** SAM Tire Temp */
typedef struct {
	uint8_t    outside_temp;                  /** Byte 0 */
	uint8_t    outside_middle_temp;           /** Byte 1 */
	uint8_t    inside_middle_temp;            /** Byte 2 */
	uint8_t    inside_temp;                   /** Byte 3 */
} GR_OLD_SAM_T_T_MSG;

/** SAM IMU */
typedef struct {
	uint16_t   accel_x;                       /** Byte 0 */
	uint16_t   accel_y;                       /** Byte 2 */
	uint16_t   accel_z;                       /** Byte 4 */
	uint16_t   gyro_x;                        /** Byte 6 */
	uint16_t   gyro_y;                        /** Byte 8 */
	uint16_t   gyro_z;                        /** Byte 10 */
} GR_OLD_SAM_IMU_MSG;

/** SAM GPS 1 */
typedef struct {
	uint32_t   latitude;                      /** Byte 0 */
	uint32_t   longitude;                     /** Byte 4 */
} GR_OLD_SAM_GPS_1_MSG;

/** SAM GPS 2 */
typedef struct {
	uint32_t   accuracy;                      /** Byte 0 */
	uint32_t   attitude;                      /** Byte 4 */
} GR_OLD_SAM_GPS_2_MSG;

/** SAM GPS Time */
typedef struct {
	uint32_t   time;                          /** Byte 0 */
	uint32_t   time_of_week_ms;               /** Byte 4 */
} GR_OLD_SAM_GPS_T_MSG;

/** SAM GPS Heading */
typedef struct {
	uint32_t   heading_from_north;            /** Byte 0 */
} GR_OLD_SAM_GPS_H_MSG;

/** SAM Sus Pots */
typedef struct {
	uint8_t    suspension_angle;              /** Byte 0 */
} GR_OLD_SAM_S_P_MSG;

/** SAM TOF */
typedef struct {
	uint16_t   height;                        /** Byte 0 */
} GR_OLD_SAM_TOF_MSG;

/** SAM Rear Wheelspeed */
typedef struct {
	uint16_t   speed;                         /** Byte 0 */
} GR_OLD_SAM_R_W_MSG;

/** SAM Pushrod Force */
typedef struct {
	uint16_t   load_force;                    /** Byte 0 */
} GR_OLD_SAM_P_F_MSG;

/** TCM Status */
typedef struct {
	uint8_t    ping_block;
	uint16_t   ping;                          /** Byte 1 */
	uint32_t   cache_size;                    /** Byte 3 */
	uint8_t    reserved;                      /** Byte 7 */
} GR_OLD_TCM_S_MSG;

/** TCM Resource Utilization */
typedef struct {
} GR_OLD_TCM_R_U_MSG;

/** Dash Warning Flags */
typedef struct {
	uint8_t    error_fault_violation_bits;
} GR_OLD_D_W_F_MSG;

/** Specific Brake IR */
typedef struct {
	uint8_t    wheel_identifier;              /** Byte 0 */
	uint8_t    temp;                          /** Byte 1 */
} GR_OLD_S_B_IR_MSG;

/** ECU Ping Information */
typedef struct {
	uint8_t    online_pings;                  /** Byte 0 */
} GR_OLD_ECU_P_I_MSG;

/** ECU Pedals Data */
typedef struct {
	uint16_t   apps1_signal;                  /** Byte 0 */
	uint16_t   apps2_signal;                  /** Byte 2 */
	uint16_t   bse_signal;                    /** Byte 4 */
	uint16_t   brake_f_signal_brake_r_signal; /** Byte 6 */
} GR_OLD_ECU_P_D_MSG;

/** GPS LAT */
typedef struct {
	uint8_t    lat;                           /** Byte 0 */
} GR_OLD_GPS_LAT_MSG;

/** GPS LON */
typedef struct {
	uint8_t    lon;                           /** Byte 0 */
} GR_OLD_GPS_LON_MSG;

/** GPS ALT */
typedef struct {
	uint8_t    alt;                           /** Byte 0 */
} GR_OLD_GPS_ALT_MSG;

/** GPS PX */
typedef struct {
	uint16_t   theta;                         /** Byte 0 */
	uint16_t   acc;                           /** Byte 2 */
	uint32_t   status;                        /** Byte 4 */
} GR_OLD_GPS_PX_MSG;

/** GPS QY */
typedef struct {
	uint16_t   theta;                         /** Byte 0 */
	uint16_t   acc;                           /** Byte 2 */
	uint32_t   status;                        /** Byte 4 */
} GR_OLD_GPS_QY_MSG;

/** GPS RZ */
typedef struct {
	uint16_t   theta;                         /** Byte 0 */
	uint16_t   acc;                           /** Byte 2 */
	uint32_t   status;                        /** Byte 4 */
} GR_OLD_GPS_RZ_MSG;

/** UVW DGPS */
typedef struct {
	uint16_t   dgps_u;                        /** Byte 0 */
	uint16_t   dgps_v;                        /** Byte 2 */
	uint16_t   dgps_w;                        /** Byte 4 */
} GR_OLD_UVW_DGPS_MSG;

#endif
