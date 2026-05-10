#include "CANdler.h"

#include <stdint.h>

#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "Pinging.h"
#include "StateData.h"
#include "bitManipulations.h"

#define WHEEL_RPM_TO_MPH_RATIO 0.0476f

extern ECU_StateData stateLump;

void ReportBadMessageLength(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id)
{
	// TODO Ideally change some state data to note a bad message, ie if ACU
	// that can be a comms error
	LOGOMATIC("Bad ECU CAN Rx length! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
}

void ReportUnhandledMessage(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id)
{
	// Filtering likely needs to be adjusted if this is happening often
	LOGOMATIC("Unhandled ECU CAN Rx msg! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
}

void ECU_CAN_MessageHandler(ECU_StateData *state_data, GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, uint8_t *data, uint32_t data_length)
{
	switch (msg_id) {
		case GRCAN_DEBUG_2_0:
			if (data_length > 8) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n", sender_id, bus_id, (int)data_length, data);
			break;

		case GRCAN_DEBUG_FD:
			if (data_length > 64) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n", sender_id, bus_id, (int)data_length, data);
			break;

		case GRCAN_PING:
			if (data_length != sizeof(GRCAN_PING_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			respondToPing(sender_id, ((GRCAN_PING_MSG *)data)->timestamp);
			break;

		case GRCAN_ACU_STATUS_1:
			if (data_length != sizeof(GRCAN_ACU_STATUS_1_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_ACU_STATUS_1_MSG *acu_status_1 = (GRCAN_ACU_STATUS_1_MSG *)data;
			state_data->tractivebattery_soc = acu_status_1->accumulator_soc;
			state_data->glv_soc = acu_status_1->glv_soc;
			state_data->ts_voltage = acu_status_1->ts_voltage * 0.01f;
			break;

		case GRCAN_ACU_STATUS_2:
			if (data_length != sizeof(GRCAN_ACU_STATUS_2_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_ACU_STATUS_2_MSG *acu_status_2 = (GRCAN_ACU_STATUS_2_MSG *)data;
			state_data->max_cell_temp_c = acu_status_2->max_cell_temp * 0.25f;
			state_data->acu_error_warning_bits = acu_status_2->status_flags;
			state_data->ir_minus = GETBIT(acu_status_2->precharge_latch_flags, 1);
			state_data->ir_plus = GETBIT(acu_status_2->precharge_latch_flags, 2);
			state_data->acu_software_latch = GETBIT(acu_status_2->precharge_latch_flags, 3);
			break;

		case GRCAN_INV_STATUS_1:
			if (data_length != sizeof(GRCAN_INV_STATUS_1_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_INV_STATUS_1_MSG *inv_status_1 = (GRCAN_INV_STATUS_1_MSG *)data;
			UNUSED(inv_status_1);
			break;
		case GRCAN_INV_STATUS_3:
			if (data_length != sizeof(GRCAN_INV_STATUS_3_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_INV_STATUS_3_MSG *inv_status_3 = (GRCAN_INV_STATUS_3_MSG *)data;
			state_data->inv_fault_map = inv_status_3->fault_bits;
			break;
		case GRCAN_DASH_STATUS:
			if (data_length != sizeof(GRCAN_DASH_STATUS_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_DASH_STATUS_MSG *dash_data = (GRCAN_DASH_STATUS_MSG *)data;
			if (state_data->ecu_state != GR_TS_DISCHARGE) {
				state_data->ts_active_button_pressed = GETBIT(dash_data->button_led_flags, 0);
			} else {
				state_data->ts_active_button_pressed = false;
			}
			if (state_data->ecu_state == GR_DRIVE_ACTIVE || state_data->ecu_state == GR_PRECHARGE_COMPLETE) {
				state_data->rtd_button_pressed = GETBIT(dash_data->button_led_flags, 1);
			} else {
				state_data->rtd_button_pressed = false;
			}

			break;
		/*
		case GRCAN_STEERING_STATUS:
			if (data_length != sizeof(GRCAN_STEERING_STATUS_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_STEERING_STATUS_MSG *steering_status = (GRCAN_STEERING_STATUS_MSG *)data;
			state_data->powerlevel_torquemap = steering_status->encoder_bits;
			break;
		*/

		// TODO: fix when sensors done
		/*
		case GRCAN_SAM_REAR_WHEELSPEED:
			if (data_length != sizeof(GRCAN_SAM_REAR_WHEELSPEED_MSG)) {
				ReportBadMessageLength(bus_id, msg_id, sender_id);
				break;
			}
			GRCAN_SAM_REAR_WHEELSPEED_MSG *encoder_status = (GRCAN_SAM_REAR_WHEELSPEED_MSG *)data;
			state_data->rr_wheel_rpm = encoder_status->wheel_speed * 0.1 - 32768; // TODO: find out which wheel this actually measures: one or 4 sensors?
			state_data->vehicle_speed_mph = state_data->rr_wheel_rpm * WHEEL_RPM_TO_MPH_RATIO;
			break;
		*/
		default:
			ReportUnhandledMessage(bus_id, msg_id, sender_id);
			break;
	}
}
