// Auto-generated GRCAN Message Data (Pure Perl Parser)
// Resolves #338 (Naming), #367 (Duplicates), and C Syntax Errors
#ifndef GRCAN_MSG_DATA_H
#define GRCAN_MSG_DATA_H

#include <stdint.h>

typedef struct {
	uint8_t acu;
	uint8_t charger;
	uint8_t dash_panel;
	uint8_t debugger;
	uint8_t dti_inverter;
	uint8_t ecu;
	uint8_t energy_meter;
	uint8_t fan_controller_1;
	uint8_t fan_controller_2;
	uint8_t fan_controller_3;
	uint8_t fan_controller_4;
	uint8_t fan_controller_5;
	uint8_t fan_controller_6;
	uint8_t fan_controller_7;
	uint8_t fan_controller_8;
	uint8_t gr_inverter_1;
	uint8_t gr_inverter_2;
	uint8_t gr_inverter_3;
	uint8_t gr_inverter_4;
	uint8_t imd;
	uint8_t lv_dc_dc;
	uint8_t sam1;
	uint8_t sam2;
	uint8_t sam3;
	uint8_t sam4;
	uint8_t sam5;
	uint8_t sam6;
	uint8_t sam7;
	uint8_t sam8;
	uint8_t sam9;
	uint8_t sam10;
	uint8_t sam11;
	uint8_t sam12;
	uint8_t sam13;
	uint8_t sam14;
	uint8_t sam15;
	uint8_t sam16;
	uint8_t sam17;
	uint8_t sam18;
	uint8_t sam19;
	uint8_t sam20;
	uint8_t steering_wheel;
	uint8_t tcm;
	uint8_t dgps;
} GRCAN_MESSAGES_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t debug;
} GRCAN_DEBUG_2_0_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t debug;
} GRCAN_DEBUG_FD_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t timestamp;
} GRCAN_PING_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t ecu_state;
	uint8_t ping_block;
	uint8_t power_level;
	uint8_t torque_map;
	uint8_t max_cell_temp;
	uint8_t accumulator_state_of_charge;
	uint8_t glv_state_of_charge;
} GRCAN_ECU_STATUS_1_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t tractive_system_voltage;
	uint8_t vehicle_speed;
	uint8_t fr_wheel_rpm;
	uint8_t fl_wheel_rpm;
} GRCAN_ECU_STATUS_2_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t rr_wheel_rpm;
	uint8_t rl_wheel_rpm;
} GRCAN_ECU_STATUS_3_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t reserved;
} GRCAN_ECU_CONFIG_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t accumulator_voltage;
	uint8_t ts_voltage;
	uint8_t accumulator_current;
	uint8_t accumulator_soc;
	uint8_t glv_soc;
} GRCAN_ACU_STATUS_1_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t _20v_voltage;
	uint8_t _12v_voltage;
	uint8_t sdc_voltage;
	uint8_t min_cell_voltage;
	uint8_t max_cell_temp;
	uint8_t over_temp_error;
	uint8_t over_voltage_error;
	uint8_t under_voltage_error;
	uint8_t over_current_error;
	uint8_t under_current_error;
	uint8_t under_voltage_20v_warning;
	uint8_t under_voltage_12v_warning;
	uint8_t under_voltage_sdc_warning;
	uint8_t precharge_error;
	uint8_t ir_precharge_state;
	uint8_t ir_state;
	uint8_t software_latch;
	uint8_t reserved;
} GRCAN_ACU_STATUS_2_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t hv_input_voltage;
	uint8_t hv_output_voltage;
	uint8_t hv_input_current;
	uint8_t hv_output_current;
} GRCAN_ACU_STATUS_3_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t set_ts_active;
} GRCAN_ACU_PRECHARGE_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t charge_voltage;
	uint8_t charge_current;
} GRCAN_ACU_CONFIG_CHARGE_PARAMETERS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t minimium_cell_voltage;
	uint8_t max_cell_temperature;
} GRCAN_ACU_CONFIG_OPERATIONAL_PARAMETERS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t cell_0_voltage;
	uint8_t cell_0_temp;
	uint8_t cell_1_voltage;
	uint8_t cell_1_temp;
	uint8_t cell_2_voltage;
	uint8_t cell_2_temp;
	uint8_t cell_3_voltage;
	uint8_t cell_3_temp;
	uint8_t cell_4_voltage;
	uint8_t cell_4_temp;
	uint8_t cell_5_voltage;
	uint8_t cell_5_temp;
	uint8_t cell_6_voltage;
	uint8_t cell_6_temp;
	uint8_t cell_7_voltage;
	uint8_t cell_7_temp;
	uint8_t cell_8_voltage;
	uint8_t cell_8_temp;
	uint8_t cell_9_voltage;
	uint8_t cell_9_temp;
	uint8_t cell_10_voltage;
	uint8_t cell_10_temp;
	uint8_t cell_11_voltage;
	uint8_t cell_11_temp;
	uint8_t cell_12_voltage;
	uint8_t cell_12_temp;
	uint8_t cell_13_voltage;
	uint8_t cell_13_temp;
	uint8_t cell_14_voltage;
	uint8_t cell_14_temp;
	uint8_t cell_15_voltage;
	uint8_t cell_15_temp;
	uint8_t cell_16_voltage;
	uint8_t cell_16_temp;
	uint8_t cell_17_voltage;
	uint8_t cell_17_temp;
	uint8_t cell_18_voltage;
	uint8_t cell_18_temp;
	uint8_t cell_19_voltage;
	uint8_t cell_19_temp;
	uint8_t cell_20_voltage;
	uint8_t cell_20_temp;
	uint8_t cell_21_voltage;
	uint8_t cell_21_temp;
	uint8_t cell_22_voltage;
	uint8_t cell_22_temp;
	uint8_t cell_23_voltage;
	uint8_t cell_23_temp;
	uint8_t cell_24_voltage;
	uint8_t cell_24_temp;
	uint8_t cell_25_voltage;
	uint8_t cell_25_temp;
	uint8_t cell_26_voltage;
	uint8_t cell_26_temp;
	uint8_t cell_27_voltage;
	uint8_t cell_27_temp;
	uint8_t cell_28_voltage;
	uint8_t cell_28_temp;
	uint8_t cell_29_voltage;
	uint8_t cell_29_temp;
	uint8_t cell_30_voltage;
	uint8_t cell_30_temp;
	uint8_t cell_31_voltage;
	uint8_t cell_31_temp;
} GRCAN_ACU_CELL_DATA_1_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t cell_32_voltage;
	uint8_t cell_32_temp;
	uint8_t cell_33_voltage;
	uint8_t cell_33_temp;
	uint8_t cell_34_voltage;
	uint8_t cell_34_temp;
	uint8_t cell_35_voltage;
	uint8_t cell_35_temp;
	uint8_t cell_36_voltage;
	uint8_t cell_36_temp;
	uint8_t cell_37_voltage;
	uint8_t cell_37_temp;
	uint8_t cell_38_voltage;
	uint8_t cell_38_temp;
	uint8_t cell_39_voltage;
	uint8_t cell_39_temp;
	uint8_t cell_40_voltage;
	uint8_t cell_40_temp;
	uint8_t cell_41_voltage;
	uint8_t cell_41_temp;
	uint8_t cell_42_voltage;
	uint8_t cell_42_temp;
	uint8_t cell_43_voltage;
	uint8_t cell_43_temp;
	uint8_t cell_44_voltage;
	uint8_t cell_44_temp;
	uint8_t cell_45_voltage;
	uint8_t cell_45_temp;
	uint8_t cell_46_voltage;
	uint8_t cell_46_temp;
	uint8_t cell_47_voltage;
	uint8_t cell_47_temp;
	uint8_t cell_48_voltage;
	uint8_t cell_48_temp;
	uint8_t cell_49_voltage;
	uint8_t cell_49_temp;
	uint8_t cell_50_voltage;
	uint8_t cell_50_temp;
	uint8_t cell_51_voltage;
	uint8_t cell_51_temp;
	uint8_t cell_52_voltage;
	uint8_t cell_52_temp;
	uint8_t cell_53_voltage;
	uint8_t cell_53_temp;
	uint8_t cell_54_voltage;
	uint8_t cell_54_temp;
	uint8_t cell_55_voltage;
	uint8_t cell_55_temp;
	uint8_t cell_56_voltage;
	uint8_t cell_56_temp;
	uint8_t cell_57_voltage;
	uint8_t cell_57_temp;
	uint8_t cell_58_voltage;
	uint8_t cell_58_temp;
	uint8_t cell_59_voltage;
	uint8_t cell_59_temp;
	uint8_t cell_60_voltage;
	uint8_t cell_60_temp;
	uint8_t cell_61_voltage;
	uint8_t cell_61_temp;
	uint8_t cell_62_voltage;
	uint8_t cell_62_temp;
	uint8_t cell_63_voltage;
	uint8_t cell_63_temp;
} GRCAN_ACU_CELL_DATA_2_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t cell_64_voltage;
	uint8_t cell_64_temp;
	uint8_t cell_65_voltage;
	uint8_t cell_65_temp;
	uint8_t cell_66_voltage;
	uint8_t cell_66_temp;
	uint8_t cell_67_voltage;
	uint8_t cell_67_temp;
	uint8_t cell_68_voltage;
	uint8_t cell_68_temp;
	uint8_t cell_69_voltage;
	uint8_t cell_69_temp;
	uint8_t cell_70_voltage;
	uint8_t cell_70_temp;
	uint8_t cell_71_voltage;
	uint8_t cell_71_temp;
	uint8_t cell_72_voltage;
	uint8_t cell_72_temp;
	uint8_t cell_73_voltage;
	uint8_t cell_73_temp;
	uint8_t cell_74_voltage;
	uint8_t cell_74_temp;
	uint8_t cell_75_voltage;
	uint8_t cell_75_temp;
	uint8_t cell_76_voltage;
	uint8_t cell_76_temp;
	uint8_t cell_77_voltage;
	uint8_t cell_77_temp;
	uint8_t cell_78_voltage;
	uint8_t cell_78_temp;
	uint8_t cell_79_voltage;
	uint8_t cell_79_temp;
	uint8_t cell_80_voltage;
	uint8_t cell_80_temp;
	uint8_t cell_81_voltage;
	uint8_t cell_81_temp;
	uint8_t cell_82_voltage;
	uint8_t cell_82_temp;
	uint8_t cell_83_voltage;
	uint8_t cell_83_temp;
	uint8_t cell_84_voltage;
	uint8_t cell_84_temp;
	uint8_t cell_85_voltage;
	uint8_t cell_85_temp;
	uint8_t cell_86_voltage;
	uint8_t cell_86_temp;
	uint8_t cell_87_voltage;
	uint8_t cell_87_temp;
	uint8_t cell_88_voltage;
	uint8_t cell_88_temp;
	uint8_t cell_89_voltage;
	uint8_t cell_89_temp;
	uint8_t cell_90_voltage;
	uint8_t cell_90_temp;
	uint8_t cell_91_voltage;
	uint8_t cell_91_temp;
	uint8_t cell_92_voltage;
	uint8_t cell_92_temp;
	uint8_t cell_93_voltage;
	uint8_t cell_93_temp;
	uint8_t cell_94_voltage;
	uint8_t cell_94_temp;
	uint8_t cell_95_voltage;
	uint8_t cell_95_temp;
} GRCAN_ACU_CELL_DATA_3_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t cell_96_voltage;
	uint8_t cell_96_temp;
	uint8_t cell_97_voltage;
	uint8_t cell_97_temp;
	uint8_t cell_98_voltage;
	uint8_t cell_98_temp;
	uint8_t cell_99_voltage;
	uint8_t cell_99_temp;
	uint8_t cell_100_voltage;
	uint8_t cell_100_temp;
	uint8_t cell_101_voltage;
	uint8_t cell_101_temp;
	uint8_t cell_102_voltage;
	uint8_t cell_102_temp;
	uint8_t cell_103_voltage;
	uint8_t cell_103_temp;
	uint8_t cell_104_voltage;
	uint8_t cell_104_temp;
	uint8_t cell_105_voltage;
	uint8_t cell_105_temp;
	uint8_t cell_106_voltage;
	uint8_t cell_106_temp;
	uint8_t cell_107_voltage;
	uint8_t cell_107_temp;
	uint8_t cell_108_voltage;
	uint8_t cell_108_temp;
	uint8_t cell_109_voltage;
	uint8_t cell_109_temp;
	uint8_t cell_110_voltage;
	uint8_t cell_110_temp;
	uint8_t cell_111_voltage;
	uint8_t cell_111_temp;
	uint8_t cell_112_voltage;
	uint8_t cell_112_temp;
	uint8_t cell_113_voltage;
	uint8_t cell_113_temp;
	uint8_t cell_114_voltage;
	uint8_t cell_114_temp;
	uint8_t cell_115_voltage;
	uint8_t cell_115_temp;
	uint8_t cell_116_voltage;
	uint8_t cell_116_temp;
	uint8_t cell_117_voltage;
	uint8_t cell_117_temp;
	uint8_t cell_118_voltage;
	uint8_t cell_118_temp;
	uint8_t cell_119_voltage;
	uint8_t cell_119_temp;
	uint8_t cell_120_voltage;
	uint8_t cell_120_temp;
	uint8_t cell_121_voltage;
	uint8_t cell_121_temp;
	uint8_t cell_122_voltage;
	uint8_t cell_122_temp;
	uint8_t cell_123_voltage;
	uint8_t cell_123_temp;
	uint8_t cell_124_voltage;
	uint8_t cell_124_temp;
	uint8_t cell_125_voltage;
	uint8_t cell_125_temp;
	uint8_t cell_126_voltage;
	uint8_t cell_126_temp;
	uint8_t cell_127_voltage;
	uint8_t cell_127_temp;
} GRCAN_ACU_CELL_DATA_4_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t cell_128_voltage;
	uint8_t cell_128_temp;
	uint8_t cell_129_voltage;
	uint8_t cell_129_temp;
	uint8_t cell_130_voltage;
	uint8_t cell_130_temp;
	uint8_t cell_131_voltage;
	uint8_t cell_131_temp;
	uint8_t cell_132_voltage;
	uint8_t cell_132_temp;
	uint8_t cell_133_voltage;
	uint8_t cell_133_temp;
	uint8_t cell_134_voltage;
	uint8_t cell_134_temp;
	uint8_t cell_135_voltage;
	uint8_t cell_135_temp;
	uint8_t cell_136_voltage;
	uint8_t cell_136_temp;
	uint8_t cell_137_voltage;
	uint8_t cell_137_temp;
	uint8_t cell_138_voltage;
	uint8_t cell_138_temp;
	uint8_t cell_139_voltage;
	uint8_t cell_139_temp;
	uint8_t cell_140_voltage;
	uint8_t cell_140_temp;
	uint8_t cell_141_voltage;
	uint8_t cell_141_temp;
	uint8_t cell_142_voltage;
	uint8_t cell_142_temp;
	uint8_t cell_143_voltage;
	uint8_t cell_143_temp;
	uint8_t cell_144_voltage;
	uint8_t cell_144_temp;
	uint8_t cell_145_voltage;
	uint8_t cell_145_temp;
	uint8_t cell_146_voltage;
	uint8_t cell_146_temp;
	uint8_t cell_147_voltage;
	uint8_t cell_147_temp;
	uint8_t cell_148_voltage;
	uint8_t cell_148_temp;
	uint8_t cell_149_voltage;
	uint8_t cell_149_temp;
	uint8_t cell_150_voltage;
	uint8_t cell_150_temp;
	uint8_t cell_151_voltage;
	uint8_t cell_151_temp;
	uint8_t cell_152_voltage;
	uint8_t cell_152_temp;
	uint8_t cell_153_voltage;
	uint8_t cell_153_temp;
	uint8_t cell_154_voltage;
	uint8_t cell_154_temp;
	uint8_t cell_155_voltage;
	uint8_t cell_155_temp;
	uint8_t cell_156_voltage;
	uint8_t cell_156_temp;
	uint8_t cell_157_voltage;
	uint8_t cell_157_temp;
	uint8_t cell_158_voltage;
	uint8_t cell_158_temp;
	uint8_t cell_159_voltage;
	uint8_t cell_159_temp;
} GRCAN_ACU_CELL_DATA_5_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t input_voltage;
	uint8_t output_voltage;
	uint8_t input_current;
	uint8_t output_current;
	uint8_t dc_dc_temp;
} GRCAN_DC_DC_STATUS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t ac_current;
	uint8_t dc_current;
	uint8_t motor_rpm;
} GRCAN_INVERTER_STATUS_1_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t u_mosfet_temperature;
	uint8_t v_mosfet_temperature;
	uint8_t w_mosfet_temperature;
} GRCAN_INVERTER_STATUS_2_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t motor_temperature;
	uint8_t over_voltage_faults;
	uint8_t under_voltage_fault;
	uint8_t inv_overtemp_fault;
	uint8_t motor_overtemp_fault;
	uint8_t transistor_fault;
	uint8_t encoder_fault;
	uint8_t can_fault;
	uint8_t future_use;
} GRCAN_INVERTER_STATUS_3_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t max_ac_current;
	uint8_t max_dc_current;
	uint8_t absolute_max_rpm_limit;
	uint8_t motor_direction;
} GRCAN_INVERTER_CONFIG_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t set_ac_current;
	uint8_t set_dc_current;
	uint8_t rpm_limit;
	uint8_t field_weakening;
	uint8_t drive_enable;
} GRCAN_INVERTER_COMMAND_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t fan_speed;
	uint8_t input_voltage;
	uint8_t input_current;
} GRCAN_FAN_STATUS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t fan_command;
} GRCAN_FAN_COMMAND_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t bms_led;
	uint8_t imd_led;
	uint8_t bspd_led;
	uint8_t ts_button_data;
	uint8_t rtd_button_data;
} GRCAN_DASH_STATUS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t bms_led;
	uint8_t imd_led;
	uint8_t bspd_led;
} GRCAN_DASH_CONFIG_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t current_encoder;
	uint8_t torque_map_encoder;
	uint8_t regen;
	uint8_t button_1;
	uint8_t button_2;
	uint8_t button_3;
	uint8_t button_4;
} GRCAN_STEERING_STATUS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t reserved;
} GRCAN_STEERING_CONFIG_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t temp;
} GRCAN_SAM_BRAKE_IR_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t outside_temp;
	uint8_t outside_middle_temp;
	uint8_t inside_middle_temp;
	uint8_t inside_temp;
} GRCAN_SAM_TIRE_TEMP_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t accel_x;
	uint8_t accel_y;
	uint8_t accel_z;
	uint8_t gyro_x;
	uint8_t gyro_y;
	uint8_t gyro_z;
} GRCAN_SAM_IMU_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t latitude;
	uint8_t longitude;
} GRCAN_SAM_GPS_1_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t accuracy;
	uint8_t attitude;
} GRCAN_SAM_GPS_2_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t time;
	uint8_t time_of_week_ms;
} GRCAN_SAM_GPS_TIME_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t heading_from_north;
} GRCAN_SAM_GPS_HEADING_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t suspension_angle;
} GRCAN_SAM_SUS_POTS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t height;
} GRCAN_SAM_TOF_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t speed;
} GRCAN_SAM_REAR_WHEELSPEED_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t load_force;
} GRCAN_SAM_PUSHROD_FORCE_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t connection_status;
	uint8_t mqtt_status;
	uint8_t epic_shelter_status;
	uint8_t camera_status;
	uint8_t reserved;
	uint8_t ping;
	uint8_t cache_size;
	uint8_t reserved;
} GRCAN_TCM_STATUS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t cpu_0_freq;
	uint8_t cpu_0_util;
	uint8_t cpu_1_freq;
	uint8_t cpu_1_util;
	uint8_t cpu_2_freq;
	uint8_t cpu_2_util;
	uint8_t cpu_3_freq;
	uint8_t cpu_3_util;
	uint8_t cpu_4_freq;
	uint8_t cpu_4_util;
	uint8_t cpu_5_freq;
	uint8_t cpu_5_util;
	uint8_t cpu_total_util;
	uint8_t ram_total;
	uint8_t ram_used;
	uint8_t ram_util;
	uint8_t gpu_util;
	uint8_t gpu_freq;
	uint8_t disk_total;
	uint8_t disk_used;
	uint8_t disk_util;
	uint8_t cpu_temp;
	uint8_t gpu_temp;
	uint8_t voltage_draw;
	uint8_t current_draw;
	uint8_t power_draw;
} GRCAN_TCM_RESOURCE_UTILIZATION_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t bse_apps_violation;
	uint8_t reserved;
	uint8_t reserved;
	uint8_t reserved;
	uint8_t reserved;
	uint8_t reserved;
	uint8_t reserved;
	uint8_t reserved;
} GRCAN_DASH_WARNING_FLAGS_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t wheel_identifier;
	uint8_t temp;
} GRCAN_SPECIFIC_BRAKE_IR_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t online_pings;
} GRCAN_ECU_PING_INFORMATION_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t apps1_signal;
	uint8_t apps2_signal;
	uint8_t bse_signal;
	uint8_t brake_f_signal;
	uint8_t brake_r_signal;
} GRCAN_ECU_PEDALS_DATA_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t lat;
} GRCAN_GPS_LAT_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t lon;
} GRCAN_GPS_LON_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t alt;
} GRCAN_GPS_ALT_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t theta;
	uint8_t acc;
	uint8_t status;
} GRCAN_GPS_PX_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t theta;
	uint8_t acc;
	uint8_t status;
} GRCAN_GPS_QY_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t theta;
	uint8_t acc;
	uint8_t status;
} GRCAN_GPS_RZ_t;

typedef struct {
	uint8_t msg_id;
	uint8_t msg_length;
	uint8_t dgps_u;
	uint8_t dgps_v;
	uint8_t dgps_w;
} GRCAN_UVW_DGPS_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_1_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_2_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_3_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_4_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_5_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_6_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_7_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_8_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_9_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_10_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_11_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_CONTROL_12_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_DATA_1_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_DATA_2_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_DATA_3_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_DATA_4_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_DTI_DATA_5_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_IMD_GENERAL_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_IMD_ISOLATION_INFO_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_IMD_VOLTAGE_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_IMD_IT_SYSTEM_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_IMD_REQUEST_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_IMD_RESPONSE_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_CHARGER_DATA_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_CHARGER_CONTROL_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_EM_MEASUREMENT_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_EM_STATUS_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_EM_TEMPERATURE_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_EM_TEAM_DATA_1_t;

typedef struct {
	uint8_t can_id;
	uint8_t length;
	uint8_t signals;
} GRCAN_EM_TEAM_DATA_2_t;

#endif // GRCAN_MSG_DATA_H
