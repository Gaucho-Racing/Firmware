#include "CANdler.h"

#include <stdint.h>

#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "Logomatic.h"
#include "Pinging.h"
#include "StateData.h"
#include "bitManipulations.h"

#define WHEEL_RPM_TO_MPH_RATIO 0.0476

extern ECU_StateData stateLump;

void ReportBadMessageLength(GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id, GR_OLD_NODE_ID sender_id)
{
	// TODO Ideally change some state data to note a bad message, ie if BCU
	// that can be a comms error
	LOGOMATIC("Bad ECU CAN Rx length! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
}

void ReportUnhandledMessage(GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id, GR_OLD_NODE_ID sender_id)
{
	// Filtering likely needs to be adjusted if this is happening often
	LOGOMATIC("Unhandled ECU CAN Rx msg! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
}

void ECU_CAN_MessageHandler(ECU_StateData *state_data, GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id, GR_OLD_NODE_ID sender_id, uint8_t *data, uint32_t data_length)
{
	switch (msg_id) {
		case MSG_DEBUG_2_0:
			if (data_length > sizeof(GR_OLD_DEBUG_2_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n", sender_id, bus_id, (int)data_length, data);
			break;

		case MSG_DEBUG_FD:
			if (data_length > sizeof(GR_OLD_DEBUG_2_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n", sender_id, bus_id, (int)data_length, data);
			break;

		case MSG_PING:
			if (data_length != sizeof(GR_OLD_PING_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			respondToPing(sender_id, ((GR_OLD_PING_MSG *)data)->timestamp);
			break;

		case MSG_BCU_STATUS_1:
			if (data_length != sizeof(GR_OLD_BCU_STATUS_1_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GR_OLD_BCU_STATUS_1_MSG *bcu_status_1 = (GR_OLD_BCU_STATUS_1_MSG *)data;
			state_data->tractivebattery_soc = bcu_status_1->tractivebattery_soc;
			state_data->glv_soc = bcu_status_1->glv_soc;
			state_data->ts_voltage = bcu_status_1->ts_voltage * 0.01;
			break;

		case MSG_BCU_STATUS_2:
			if (data_length != sizeof(GR_OLD_BCU_STATUS_2_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GR_OLD_BCU_STATUS_2_MSG *bcu_status_2 = (GR_OLD_BCU_STATUS_2_MSG *)data;
			state_data->max_cell_temp_c = bcu_status_2->max_cell_temp * 0.25;
			state_data->bcu_error_warning_bits = bcu_status_2->error_bits;
			state_data->ir_minus = GETBIT(bcu_status_2->precharge_bits, 1);
			state_data->ir_plus = GETBIT(bcu_status_2->precharge_bits, 2);
			state_data->bcu_software_latch = GETBIT(bcu_status_2->precharge_bits, 3);
			break;

		case MSG_INVERTER_STATUS_1:
			if (data_length != sizeof(GR_OLD_INVERTER_STATUS_1_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GR_OLD_INVERTER_STATUS_1_MSG *inverter_status_1 = (GR_OLD_INVERTER_STATUS_1_MSG *)data;
			break;
		case MSG_INVERTER_STATUS_3:
			if (data_length != sizeof(GR_OLD_INVERTER_STATUS_3_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GR_OLD_INVERTER_STATUS_3_MSG *inverter_status_3 = (GR_OLD_INVERTER_STATUS_3_MSG *)data;
			state_data->inverter_fault_map = inverter_status_3->fault_bits;
			break;
		case MSG_STEERING_STATUS:
			if (data_length != sizeof(GR_OLD_STEERING_STATUS_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GR_OLD_STEERING_STATUS_MSG *steering_status = (GR_OLD_STEERING_STATUS_MSG *)data;
			state_data->powerlevel_torquemap = steering_status->encoder_bits;
			break;
		case MSG_SAM_REAR_WHEELSPEED:
			if (data_length != sizeof(GR_OLD_SAM_REAR_WHEELSPEED_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GR_OLD_SAM_REAR_WHEELSPEED_MSG *encoder_status = (GR_OLD_SAM_REAR_WHEELSPEED_MSG *)data;
			state_data->rr_wheel_rpm = encoder_status->wheel_speed * 0.1 - 32768; // TODO: find out which wheel this actually measures: sender_id???
			state_data->vehicle_speed_mph = state_data->rr_wheel_rpm * WHEEL_RPM_TO_MPH_RATIO;
			break;
		default:
			ReportUnhandledMessage(bus_id, msg_id, sender_id);
			break;
	}
}
