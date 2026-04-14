#include "CANdler.h"

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
	LOGOMATIC("Unhandled ECU CAN Rx msg! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
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
			if (data_length > sizeof(GRCAN_DEBUG_2_0)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n", sender_id, bus_id, (int)data_length, data);
			break;

		case GRCAN_DEBUG_FD:
			if (data_length > sizeof(GRCAN_DEBUG_FD)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n", sender_id, bus_id, (int)data_length, data);
			break;
		case GRCAN_DASH_CONFIG:
			if (data_length > sizeof(GRCAN_DASH_CONFIG)) {
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
