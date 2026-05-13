#include "CANdler.h"
#include "StateMachine.h"
#include <stdint.h>
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "bitManipulations.h"



void ReportBadMessageLength(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id)
{
	// TODO Ideally change some state data to note a bad message, ie if BCU
	// that can be a comms error
	LOGOMATIC("Bad CAN Message length! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
}

void ReportUnhandledMessage(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id)
{
	// Filtering likely needs to be adjusted if this is happening often
	LOGOMATIC("Unhandled CANine CAN Rx msg! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
}


void LoopbackTest(uint32_t ID, void *data, uint32_t size)
{
	uint8_t *bytes = (uint8_t *)data;
	LOGOMATIC("Received CAN message with ID: 0x%lX, data: 0x", ID);
	for (uint32_t i = 0; i < size; i++) {
		LOGOMATIC("%02X", bytes[i]);
	}
	LOGOMATIC("\n");
}

void CAN_MessageHandler(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, uint8_t *data, uint32_t data_length) {
	switch (msg_id) {
		case GRCAN_DEBUG_2_0:
			if (data_length > sizeof(GRCAN_DEBUG_2_0_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n", sender_id, bus_id, (int)data_length, data);
			break;
		case GRCAN_DEBUG_FD:
			if (data_length > sizeof(GRCAN_DEBUG_FD_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n", sender_id, bus_id, (int)data_length, data);
			break;
		case GRCAN_PING:
			if (data_length > sizeof(GRCAN_PING_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
        	GRCAN_PING_MSG * grcan_ping_msg = (GRCAN_PING_MSG *) data;

			LOGOMATIC("Time in millis: %lu", grcan_ping_msg->timestamp);
        	break;
		case GRCAN_ECU_STATUS_1:
			if (data_length > sizeof(GRCAN_ECU_STATUS_1_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_ECU_STATUS_1_MSG * grcan_ecu_status_1_msg = (GRCAN_ECU_STATUS_1_MSG *) data;
			switch (grcan_ecu_status_1_msg->ecu_state) {
				case GR_GLV_OFF:
					LOGOMATIC("Current ECU State: GR_GLV_OFF \n");
					break;
				case GR_GLV_ON:
					LOGOMATIC("Current ECU State: GR_GLV_ON \n");
					break;
				case GR_PRECHARGE_ENGAGED:
					LOGOMATIC("Current ECU State: GR_PRECHARGE_ENGAGED \n");
					break;
				case GR_PRECHARGE_COMPLETE:
					LOGOMATIC("Current ECU State: GR_PRECHARGE_COMPLETE \n");
					break;
				case GR_DRIVE_ACTIVE:
					LOGOMATIC("Current ECU State: GR_DRIVE_ACTIVE \n");
					break;
				case GR_TS_DISCHARGE:
					LOGOMATIC("Current ECU State: GR_TS_DISCHARGE \n");
					break;
				default:
					LOGOMATIC("Current ECU State: None found \n");
					break;
			}

			LOGOMATIC("BCU Node Status: %s\n", GETBIT(grcan_ecu_status_1_msg->status_flags, 7) ? "OK" : "Timeout");
			LOGOMATIC("GR Inverter Status: %s\n", GETBIT(grcan_ecu_status_1_msg->status_flags, 6) ? "OK" : "Timeout");
			LOGOMATIC("Fan Controller 1 Status: %s\n", GETBIT(grcan_ecu_status_1_msg->status_flags, 5) ? "OK" : "Timeout");
			LOGOMATIC("Fan Controller 2 Status: %s\n", GETBIT(grcan_ecu_status_1_msg->status_flags, 4) ? "OK" : "Timeout");
			LOGOMATIC("Fan Controller 3 Status: %s\n", GETBIT(grcan_ecu_status_1_msg->status_flags, 3) ? "OK" : "Timeout");
			LOGOMATIC("Dash Panel Status: %s\n", GETBIT(grcan_ecu_status_1_msg->status_flags, 2) ? "OK" : "Timeout");
			LOGOMATIC("TCM Node Status: %s\n", GETBIT(grcan_ecu_status_1_msg->status_flags, 1) ? "OK" : "Timeout");

			uint8_t torque_map = grcan_ecu_status_1_msg->power_level_torque_map & 0x0F;
            uint8_t power_lvl = (grcan_ecu_status_1_msg->power_level_torque_map >> 4) & 0x0F;

			LOGOMATIC("Power Level (hex value, controls the AC current limits): 0x%X\n", power_lvl);
            LOGOMATIC("Torque Map (hex value): 0x%X\n", torque_map);

			LOGOMATIC("Max Cell Temperature: %u\n", grcan_ecu_status_1_msg->max_cell_temp);
			LOGOMATIC("Percent of accumlator charged: %u\n", grcan_ecu_status_1_msg->accumulator_state_of_charge);
			LOGOMATIC("Percent of Low Voltage Bat charged: %u\n", grcan_ecu_status_1_msg->glv_state_of_charge);
			LOGOMATIC("Output terminal voltage of accumulator: %u\n", grcan_ecu_status_1_msg->tractive_system_voltage);
			break;
		case GRCAN_ECU_STATUS_2:
			if (data_length > sizeof(GRCAN_ECU_STATUS_2_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_ECU_STATUS_2_MSG * grcan_ecu_status_2_msg = (GRCAN_ECU_STATUS_2_MSG *)data;
			LOGOMATIC("Absolute value of speed: %u\n", grcan_ecu_status_2_msg->vehicle_speed);
			LOGOMATIC("FR Wheel RPM: %u\n", grcan_ecu_status_2_msg->fr_wheel_rpm);
			LOGOMATIC("FL Wheel RPM: %u\n", grcan_ecu_status_2_msg->fl_wheel_rpm);
			LOGOMATIC("RR Wheel RPM: %u\n", grcan_ecu_status_2_msg->rr_wheel_rpm);
			break;







		case GRCAN_ECU_STATUS_3:
			if (data_length > sizeof(GRCAN_ECU_STATUS_3_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_ECU_STATUS_3_MSG * grcan_ecu_status_3 = (GRCAN_ECU_STATUS_3_MSG *)data;
			LOGOMATIC("Wheel RPM: %u\n", grcan_ecu_status_3->rl_wheel_rpm);
			break;
		case GRCAN_BCU_CELL_DATA_1:
            if (data_length > sizeof(GRCAN_BCU_CELL_DATA_1_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_BCU_CELL_DATA_1_MSG * grcan_bcu_cell_data_1_msg = (GRCAN_BCU_CELL_DATA_1_MSG *)data;
            for (int i = 0; i < 32; i++) {
                LOGOMATIC("Cell %d - Voltage: %u, Temp: %u\n", i + 1, grcan_bcu_cell_data_1_msg->cells[i].voltage, grcan_bcu_cell_data_1_msg->cells[i].temperature);
            }
            break;
        case GRCAN_BCU_CELL_DATA_2:
            if (data_length > sizeof(GRCAN_BCU_CELL_DATA_2_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_BCU_CELL_DATA_2_MSG * grcan_bcu_cell_data_2_msg = (GRCAN_BCU_CELL_DATA_2_MSG *)data;
            for (int i = 0; i < 32; i++) {
                LOGOMATIC("Cell %d - Voltage: %u, Temp: %u\n", i + 33, grcan_bcu_cell_data_2_msg->cells[i].voltage, grcan_bcu_cell_data_2_msg->cells[i].temperature);
            }
            break;

        case GRCAN_BCU_CELL_DATA_3:
            if (data_length > sizeof(GRCAN_BCU_CELL_DATA_3_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_BCU_CELL_DATA_3_MSG * grcan_bcu_cell_data_3_msg = (GRCAN_BCU_CELL_DATA_3_MSG *)data;
            for (int i = 0; i < 32; i++) {
                LOGOMATIC("Cell %d - Voltage: %u, Temp: %u\n", i + 65, grcan_bcu_cell_data_3_msg->cells[i].voltage, grcan_bcu_cell_data_3_msg->cells[i].temperature);
            }
            break;

        case GRCAN_BCU_CELL_DATA_4:
            if (data_length > sizeof(GRCAN_BCU_CELL_DATA_4_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_BCU_CELL_DATA_4_MSG * grcan_bcu_cell_data_4_msg = (GRCAN_BCU_CELL_DATA_4_MSG *)data;
            for (int i = 0; i < 32; i++) {
                LOGOMATIC("Cell %d - Voltage: %u, Temp: %u\n", i + 97, grcan_bcu_cell_data_4_msg->cells[i].voltage, grcan_bcu_cell_data_4_msg->cells[i].temperature);
            }
            break;

        case GRCAN_BCU_CELL_DATA_5:
            if (data_length > sizeof(GRCAN_BCU_CELL_DATA_5_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_BCU_CELL_DATA_5_MSG * grcan_bcu_cell_data_5_msg = (GRCAN_BCU_CELL_DATA_5_MSG *)data;
            for (int i = 0; i < 32; i++) {
                LOGOMATIC("Cell %d - Voltage: %u, Temp: %u\n", i + 129, grcan_bcu_cell_data_5_msg->cells[i].voltage, grcan_bcu_cell_data_5_msg->cells[i].temperature);
            }
            break;
		//convert to celsius
        case GRCAN_INVERTER_STATUS_1:
            if (data_length > sizeof(GRCAN_INVERTER_STATUS_1_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_INVERTER_STATUS_1_MSG * grcan_inverter_status_1_msg = (GRCAN_INVERTER_STATUS_1_MSG *)data;
            LOGOMATIC("AC Current: %u (0.01 * current)\n", grcan_inverter_status_1_msg->ac_current);
            LOGOMATIC("DC Current: %u (0.01 * current)\n", grcan_inverter_status_1_msg->dc_current);
            LOGOMATIC("Motor RPM: %u (RPM)\n", grcan_inverter_status_1_msg->motor_rpm);
            break;

        case GRCAN_INVERTER_STATUS_2:
            if (data_length > sizeof(GRCAN_INVERTER_STATUS_2_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_INVERTER_STATUS_2_MSG * grcan_inverter_status_2_msg = (GRCAN_INVERTER_STATUS_2_MSG *)data;
            LOGOMATIC("U MOSFET Temp: %u (Celsius + 40)\n", grcan_inverter_status_2_msg->u_mosfet_temperature);
            LOGOMATIC("V MOSFET Temp: %u (Celsius + 40)\n", grcan_inverter_status_2_msg->v_mosfet_temperature);
            LOGOMATIC("W MOSFET Temp: %u (Celsius + 40)\n", grcan_inverter_status_2_msg->w_mosfet_temperature);
            break;

        case GRCAN_INVERTER_STATUS_3:
            if (data_length > sizeof(GRCAN_INVERTER_STATUS_3_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_INVERTER_STATUS_3_MSG * grcan_inverter_status_3_msg = (GRCAN_INVERTER_STATUS_3_MSG *)data;
            int32_t motor_temp_c = (int32_t)grcan_inverter_status_3_msg->motor_temperature - 40;
            LOGOMATIC("Motor Temp: %ld C\n", motor_temp_c);

            const char *fault_msgs[] = {
                "TS Over Max",
                "TS Under Min",
                "Inverter Over Temp",
                "Motor Over Temp",
                "Mosfet Drive Error",
                "Encoder Error",
                "CAN Error/Timeout",
                "Reserved/Unknown Fault"
            };

            if (grcan_inverter_status_3_msg->fault_bits == 0) {
                LOGOMATIC("System Status: OK\n");
            } else {
                for (int i = 0; i < 8; i++) {
                    if (GETBIT(grcan_inverter_status_3_msg->fault_bits, i)) {
                        LOGOMATIC("FAULT DETECTED: %s\n", fault_msgs[i]);
                    }
                }
            }
            break;

        case GRCAN_INVERTER_CONFIG:
            if (data_length > sizeof(GRCAN_INVERTER_CONFIG_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_INVERTER_CONFIG_MSG * grcan_inverter_config_msg = (GRCAN_INVERTER_CONFIG_MSG *)data;
            LOGOMATIC("Max AC Current: %u\n", grcan_inverter_config_msg->max_ac_current);
            LOGOMATIC("Max DC Current: %u\n", grcan_inverter_config_msg->max_dc_current);
            LOGOMATIC("RPM Limit: %u (0: No limit)\n", grcan_inverter_config_msg->absolute_max_rpm_limit);
            LOGOMATIC("Motor Direction: %s\n", (grcan_inverter_config_msg->motor_direction == 1) ? "Inverted" : "Normal");
            break;

        case GRCAN_INVERTER_COMMAND:
            if (data_length > sizeof(GRCAN_INVERTER_COMMAND_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_INVERTER_COMMAND_MSG * grcan_inverter_command_msg = (GRCAN_INVERTER_COMMAND_MSG *)data;
            LOGOMATIC("Set AC Current: %u\n", grcan_inverter_command_msg->set_ac_current);
            LOGOMATIC("Set DC Current: %u\n", grcan_inverter_command_msg->set_dc_current);
            LOGOMATIC("RPM Limit: %u\n", grcan_inverter_command_msg->rpm_limit);
            LOGOMATIC("Field Weakening: %u\n", grcan_inverter_command_msg->field_weakening);
            LOGOMATIC("Drive Enable: %s\n", (grcan_inverter_command_msg->drive_enable == 1) ? "ENABLED" : "DISABLED");
            break;

        case GRCAN_FAN_STATUS:
            if (data_length > sizeof(GRCAN_FAN_STATUS_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_FAN_STATUS_MSG * grcan_fan_status_msg = (GRCAN_FAN_STATUS_MSG *)data;
            LOGOMATIC("Fan Speed: %u RPM\n", grcan_fan_status_msg->fan_speed);
            LOGOMATIC("Input Voltage: %u (0-22)\n", grcan_fan_status_msg->input_voltage);
            LOGOMATIC("Input Current: %u (0-10)\n", grcan_fan_status_msg->input_current);
            break;

        case GRCAN_FAN_COMMAND:
            if (data_length > sizeof(GRCAN_FAN_COMMAND_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_FAN_COMMAND_MSG * grcan_fan_command_msg = (GRCAN_FAN_COMMAND_MSG *)data;
            LOGOMATIC("Fan Command: %u Percent\n", grcan_fan_command_msg->fan_command);
            break;

		//is this correct
        case GRCAN_DASH_STATUS:
            if (data_length > sizeof(GRCAN_DASH_STATUS_MSG)) {
                ReportBadMessageLength(bus_id, msg_id, sender_id);
                break;
            }
            GRCAN_DASH_STATUS_MSG * grcan_dash_status_msg = (GRCAN_DASH_STATUS_MSG *)data;
            LOGOMATIC("TS Active: %s\n", GETBIT(grcan_dash_status_msg->button_flags, 0) ? "YES" : "NO");
            LOGOMATIC("RTD: %s\n", GETBIT(grcan_dash_status_msg->button_flags, 1) ? "YES" : "NO");
            LOGOMATIC("LED BMS: %s\n", GETBIT(grcan_dash_status_msg->led_bits, 0) ? "ON" : "OFF");
            LOGOMATIC("LED IMD: %s\n", GETBIT(grcan_dash_status_msg->led_bits, 1) ? "ON" : "OFF");
            LOGOMATIC("LED BSPD: %s\n", GETBIT(grcan_dash_status_msg->led_bits, 2) ? "ON" : "OFF");
            break;













		case GRCAN_DASH_CONFIG:
			if (data_length > sizeof(GRCAN_DASH_CONFIG_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_DASH_CONFIG_MSG * grcan_dash_config_msg = (GRCAN_DASH_CONFIG_MSG *) data;
			LOGOMATIC("BMS: %s\n", GETBIT(grcan_dash_config_msg->led_bits, 0) ? "true" : "false");
			LOGOMATIC("IMD %s\n", GETBIT(grcan_dash_config_msg->led_bits, 1) ? "true" : "false");
			LOGOMATIC("BSPD %s\n", GETBIT(grcan_dash_config_msg->led_bits, 2) ? "true" : "false");
			break;
		case GRCAN_TCM_STATUS:
			if (data_length > sizeof(GRCAN_TCM_STATUS_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_TCM_STATUS_MSG * grcan_tcm_status_msg = (GRCAN_TCM_STATUS_MSG *)data;
			LOGOMATIC("Connection Status: %s\n", GETBIT(grcan_tcm_status_msg->status_bits, 0) ? "OK" : "Timeout");
			LOGOMATIC("MQTT Status: %s\n", GETBIT(grcan_tcm_status_msg->status_bits, 1) ? "OK" : "Timeout");
			LOGOMATIC("Epic Shelter Status %s\n", GETBIT(grcan_tcm_status_msg->status_bits, 2) ? "In Progress" : "Idle");
			LOGOMATIC("Camera Status %s\n", GETBIT(grcan_tcm_status_msg->status_bits, 3) ? "Recording" : "Idle");

			LOGOMATIC("Mapache Ping: %u ms\n", grcan_tcm_status_msg->mapache_ping);
			LOGOMATIC("Cache Size: %lu \n", grcan_tcm_status_msg->cache_size);

			break;
		case GRCAN_TCM_RESOURCE_UTILIZATION:
			if (data_length > sizeof(GRCAN_TCM_RESOURCE_UTILIZATION_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_TCM_RESOURCE_UTILIZATION_MSG * grcan_tcm_resource_utilization_msg = (GRCAN_TCM_RESOURCE_UTILIZATION_MSG *)data;
			LOGOMATIC("Core 0 Frequency: %u MHz\n", grcan_tcm_resource_utilization_msg->cpu_0_freq);
			LOGOMATIC("Core 0 Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->cpu_0_util);
			LOGOMATIC("Core 1 Frequency: %u MHz\n", grcan_tcm_resource_utilization_msg->cpu_1_freq);
			LOGOMATIC("Core 1 Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->cpu_1_util);
			LOGOMATIC("Core 2 Frequency: %u MHz\n", grcan_tcm_resource_utilization_msg->cpu_2_freq);
			LOGOMATIC("Core 2 Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->cpu_2_util);
			LOGOMATIC("Core 3 Frequency: %u MHz\n", grcan_tcm_resource_utilization_msg->cpu_3_freq);
			LOGOMATIC("Core 3 Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->cpu_3_util);
			LOGOMATIC("Core 4 Frequency: %u MHz\n", grcan_tcm_resource_utilization_msg->cpu_4_freq);
			LOGOMATIC("Core 4 Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->cpu_4_util);
			LOGOMATIC("Core 5 Frequency: %u MHz\n", grcan_tcm_resource_utilization_msg->cpu_5_freq);
			LOGOMATIC("Core 5 Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->cpu_5_util);
			LOGOMATIC("Total CPU Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->cpu_total_util);
			LOGOMATIC("Total Memory: %u MB\n", grcan_tcm_resource_utilization_msg->ram_total);
			LOGOMATIC("Used Memory: %u MB\n", grcan_tcm_resource_utilization_msg->ram_used);
			LOGOMATIC("Memory Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->ram_util);
			LOGOMATIC("GPU Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->gpu_util);
			LOGOMATIC("GPU Frequency: %u MHz\n", grcan_tcm_resource_utilization_msg->gpu_freq);
			LOGOMATIC("Total Disk Space: %lu MB\n", grcan_tcm_resource_utilization_msg->disk_total);
			LOGOMATIC("Used Disk Space: %lu MB\n", grcan_tcm_resource_utilization_msg->disk_used);
			LOGOMATIC("Disk Utilization: %u%%\n", grcan_tcm_resource_utilization_msg->disk_util);
			LOGOMATIC("CPU Temp: %u ˚C\n", grcan_tcm_resource_utilization_msg->cpu_temp);
			LOGOMATIC("GPU Temp: %u ˚C\n", grcan_tcm_resource_utilization_msg->gpu_temp);
			LOGOMATIC("Voltage Draw: %u mV\n", grcan_tcm_resource_utilization_msg->voltage_draw);
			LOGOMATIC("Current Draw: %u mA\n", grcan_tcm_resource_utilization_msg->current_draw);
			LOGOMATIC("Power Draw: %u mW\n", grcan_tcm_resource_utilization_msg->power_draw);

			break;
		case GRCAN_ECU_ANALOG_DATA:
			if (data_length > sizeof(GRCAN_ECU_ANALOG_DATA_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_ECU_ANALOG_DATA_MSG * grcan_ecu_analog_data_msg = (GRCAN_ECU_ANALOG_DATA_MSG *)data;

			LOGOMATIC("All of the following ECU Analog Signals are in the range of 4-20 mA: ");
			LOGOMATIC("BSPD Signal: %u mA\n", grcan_ecu_analog_data_msg->bspd_signal);
			LOGOMATIC("BSE Signal: %u mA\n", grcan_ecu_analog_data_msg->bse_signal);
			LOGOMATIC("Apps 1 Signal: %u mA\n", grcan_ecu_analog_data_msg->apps_1_signal);
			LOGOMATIC("Apps 2 Signal: %u mA\n", grcan_ecu_analog_data_msg->apps_2_signal);
			LOGOMATIC("Brakeline F Signal: %u mA\n", grcan_ecu_analog_data_msg->brakeline_f_signal);
			LOGOMATIC("Brakeline R Signal: %u mA\n", grcan_ecu_analog_data_msg->brakeline_r_signal);
			LOGOMATIC("Steering Angle Signal: %u mA\n", grcan_ecu_analog_data_msg->steering_angle_signal);
			LOGOMATIC("Aux Signal: %u mA\n", grcan_ecu_analog_data_msg->aux_signal);

			break;
		case GRCAN_GPS_LAT:
			if (data_length > sizeof(GRCAN_GPS_LAT_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_GPS_LAT_MSG * grcan_gps_lat_msg = (GRCAN_GPS_LAT_MSG *)data;

			LOGOMATIC("Lattitude: %u \n", grcan_gps_lat_msg->lat);
			break;
		case GRCAN_GPS_LON:
			if (data_length > sizeof(GRCAN_GPS_LON_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_GPS_LON_MSG * grcan_gps_lon_msg = (GRCAN_GPS_LON_MSG *)data;
			LOGOMATIC("Longitude: %u \n", grcan_gps_lon_msg->lon);
			break;
		case GRCAN_GPS_ALT:
			if (data_length > sizeof(GRCAN_GPS_ALT_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_GPS_ALT_MSG * grcan_gps_alt_msg = (GRCAN_GPS_ALT_MSG *)data;

			LOGOMATIC("Altitude: %u \n", grcan_gps_alt_msg->alt);
			break;
		case GRCAN_GPS_PX:
			if (data_length > sizeof(GRCAN_GPS_PX_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_GPS_PX_MSG * grcan_gps_px_msg = (GRCAN_GPS_PX_MSG *)data;

			LOGOMATIC("Theta: %u \n", grcan_gps_px_msg->theta);
			LOGOMATIC("Acc: %u \n", grcan_gps_px_msg->acc);
			LOGOMATIC("Status: %lu \n", grcan_gps_px_msg->status);

			break;
		case GRCAN_GPS_QY:
			if (data_length > sizeof(GRCAN_GPS_QY_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_GPS_QY_MSG * grcan_gps_qy_msg = (GRCAN_GPS_QY_MSG *)data;

			LOGOMATIC("Theta: %u \n", grcan_gps_qy_msg->theta);
			LOGOMATIC("Acc: %u \n", grcan_gps_qy_msg->acc);
			LOGOMATIC("Status: %lu \n", grcan_gps_qy_msg->status);

			break;
		case GRCAN_GPS_RZ:
			if (data_length > sizeof(GRCAN_GPS_RZ_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_GPS_RZ_MSG * grcan_gps_rz_msg = (GRCAN_GPS_RZ_MSG *)data;

			LOGOMATIC("Theta: %u \n", grcan_gps_rz_msg->theta);
			LOGOMATIC("Acc: %u \n", grcan_gps_rz_msg->acc);
			LOGOMATIC("Status: %lu \n", grcan_gps_rz_msg->status);

			break;
		case GRCAN_UVW_DGPS:
			if (data_length > sizeof(GRCAN_UVW_DGPS_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_UVW_DGPS_MSG * grcan_uvw_dgps_msg = (GRCAN_UVW_DGPS_MSG *)data;

			LOGOMATIC("U: %u \n", grcan_uvw_dgps_msg->dgps_u);
			LOGOMATIC("V: %u \n", grcan_uvw_dgps_msg->dgps_v);
			LOGOMATIC("W: %u \n", grcan_uvw_dgps_msg->dgps_w);

			break;
		case GRCAN_ECU_PERFORMANCE:
			if (data_length > sizeof(GRCAN_ECU_PERFORMANCE_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_ECU_PERFORMANCE_MSG * grcan_ecu_performance_msg = (GRCAN_ECU_PERFORMANCE_MSG *)data;

			LOGOMATIC("Total number of clock cycles elapsed for 10 iterations of the main loop: %lu \n", grcan_ecu_performance_msg->elapsed_cycles);

			break;
		default:
			ReportUnhandledMessage(bus_id, msg_id, sender_id);
			break;
	}
}
