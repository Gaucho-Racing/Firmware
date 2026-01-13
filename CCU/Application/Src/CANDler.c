#include "CANdler.h"

#include <stdint.h>

#include "CCUStateData.h"
#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "Logomatic.h"

void ReportBadMessageLength(GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id, GR_OLD_NODE_ID sender_id) { LOGOMATIC("Bad CCU CAN Rx length! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id); }

void ACU_CAN_MSG_Handler(CCU_StateData *state_data, GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id, GR_OLD_NODE_ID sender_id, uint8_t *data, uint32_t data_length)
{

	switch (msg_id) {
		case MSG_DEBUG_2_0:
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
			// TODO See Issue #143
			break;

		case MSG_ACU_STATUS_2:

			GR_OLD_BCU_STATUS_2_MSG *bcu_status_2 = (GR_OLD_BCU_STATUS_2_MSG *)data;
			state_data->ACU_S2_MAX_CELL_Volt = bcu_status_2->max_cell_temp;
			state_data->ACU_S2_ERROR_BITS = bcu_status_2->error_bits;
			break;

		case MSG_ACU_STATUS_3:
			// etc
			GR_OLD_BCU_STATUS_3_MSG *bcu_status_3 = (GR_OLD_BCU_STATUS_3_MSG *)data;
			state_data->ACU_S3_HV_INPUTVolt = bcu_status_3->hv_input_voltage;
			state_data->ACU_S3_HV_INPUTCurr = bcu_status_3->hv_input_current;
			break;
	}
}
