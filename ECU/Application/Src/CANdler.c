#include "CANdler.h"

#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "Logomatic.h"
#include "StateData.h"

void ReportBadMessageLength(GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id,
			    GR_OLD_NODE_ID sender_id)
{
	LOGOMATIC("Bad ECU CAN Rx length! Bus: %d, Msg: %X, Sender: %X\n",
		  bus_id, msg_id, sender_id);
}

void ReportUnhandledMessage(GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id,
			    GR_OLD_NODE_ID sender_id)
{
	LOGOMATIC("Unhandled ECU CAN Rx msg! Bus: %d, Msg: %X, Sender: %X\n",
		  bus_id, msg_id, sender_id);
}

void ECU_CAN_MessageHandler(ECU_StateData *state_data, GR_OLD_BUS_ID bus_id,
			    GR_OLD_MSG_ID msg_id, GR_OLD_NODE_ID sender_id,
			    uint8_t *data, uint32_t data_length,
			    uint32_t timestamp)
{
	switch (msg_id) {
		case MSG_DEBUG_2_0:
			if (data_length > sizeof(GR_OLD_DEBUG_2_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n",
				  sender_id, bus_id, data_length, data);
			break;
		case MSG_DEBUG_FD:
			if (data_length > sizeof(GR_OLD_DEBUG_2_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			LOGOMATIC("Received from %02X on bus %d: %.*s\n",
				  sender_id, bus_id, data_length, data);
			break;
		case MSG_PING:
			if (data_length != sizeof(GR_OLD_PING_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			// TODO See #143
			break;
		case MSG_ACU_STATUS_1:
			if (data_length != sizeof(GR_OLD_BCU_STATUS_1_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			GR_OLD_BCU_STATUS_1_MSG *bcu_status_1 =
			    (GR_OLD_BCU_STATUS_1_MSG *)data;
			state_data->AccumulatorStateOfCharge =
			    bcu_status_1->tractivebattery_soc;
			state_data->GLVStateOfCharge = bcu_status_1->glv_soc;
			state_data->TractiveSystemVoltage =
			    bcu_status_1->ts_voltage;
			break;
		case MSG_ACU_STATUS_2:
			if (data_length != sizeof(GR_OLD_BCU_STATUS_2_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			GR_OLD_BCU_STATUS_2_MSG *bcu_status_2 =
			    (GR_OLD_BCU_STATUS_2_MSG *)data;
			state_data->MaxCellTemp = bcu_status_2->max_cell_temp;
			state_data->acu_error_warning_bits =
			    bcu_status_2->error_bits;
			break;
		case MSG_INVERTER_STATUS_1:
			if (data_length !=
			    sizeof(GR_OLD_INVERTER_STATUS_1_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			GR_OLD_INVERTER_STATUS_1_MSG *inverter_status_1 =
			    (GR_OLD_INVERTER_STATUS_1_MSG *)data;
			state_data->RLWheelRPM = inverter_status_1->motor_rpm;
			state_data->RRWheelRPM = inverter_status_1->motor_rpm;
			break;
		case MSG_INVERTER_STATUS_3:
			if (data_length !=
			    sizeof(GR_OLD_INVERTER_STATUS_3_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			GR_OLD_INVERTER_STATUS_3_MSG *inverter_status_3 =
			    (GR_OLD_INVERTER_STATUS_3_MSG *)data;
			state_data->inverter_fault_map =
			    inverter_status_3->fault_bits;
			break;
		case MSG_DASH_STATUS:
			if (data_length != sizeof(GR_OLD_DASH_STATUS_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			GR_OLD_DASH_STATUS_MSG *dash_status =
			    (GR_OLD_DASH_STATUS_MSG *)data;
			state_data->ts_active = dash_status->ts_button;
			state_data->rtd_engaged = dash_status->rtd_button;
			break;
		case MSG_STEERING_STATUS:
			if (data_length != sizeof(GR_OLD_STEERING_STATUS_MSG)) {
				ReportBadMessageLength(bus_id, msg_id,
						       sender_id);
				break;
			}
			GR_OLD_STEERING_STATUS_MSG *steering_status =
			    (GR_OLD_STEERING_STATUS_MSG *)data;
			state_data->PowerLevelTorqueMap =
			    steering_status->encoder_bits;
			break;
		default:
			ReportUnhandledMessage(bus_id, msg_id, sender_id);
			break;
	}
}
