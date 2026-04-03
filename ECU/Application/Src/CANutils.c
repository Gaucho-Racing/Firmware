#include "CANutils.h"

#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateTicks.h"
#include "StateUtils.h"
#include "can.h"
#include "main.h"
#include "string.h"

uint32_t lastTickECUStateDataSent = 0;

void ECU_CAN_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size)
{
	if (size > FDCAN_MAX_DATA_BYTES) {
		size = FDCAN_MAX_DATA_BYTES;
		LOGOMATIC("Tried to send more than 64 bytes over CAN");
	}

	uint32_t ID = ((0xFF & LOCAL_GR_ID) << 20) | ((0xFFF & messageID) << 8) | (0xFF & destNode);

	FDCAN_TxHeaderTypeDef header = {
	    .Identifier = ID,
	    .IdType = FDCAN_STANDARD_ID,
	    .TxFrameType = FDCAN_DATA_FRAME,
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
	    .DataLength = size,
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
	    .MessageMarker = 0,
	};

	FDCANTxMessage msg = {0};
	msg.tx_header = header;

	memcpy(&(msg.data), data, size);

	switch (bus) {
		case GRCAN_BUS_PRIMARY:
			can_send(primary_can, &msg);
			break;
		case GRCAN_BUS_DATA:
			can_send(data_can, &msg);
			break;
		default:
			LOGOMATIC("CAN: Invalid bus ID %d\n", bus);
			break;
	}
}

// TODO: If you try to send anything but control messages, you are cooked buddy
// Doesn't actually use Motorola order for multiple fields, just sends the bytes in reverse order
/*void ECU_CAN_Send_DTI(uint16_t msgID, uint8_t data[], uint32_t length)
{
	if ((MSG_DTI_CONTROL_10 & 0xFF) != 0x16) {
		LOGOMATIC("NOT A DTI MESSAGE");
	}

	FDCAN_TxHeaderTypeDef TxHeader = {
    .IdType = FDCAN_EXTENDED_ID,
    .TxFrameType = FDCAN_DATA_FRAME,
    .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
	// FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
    .BitRateSwitch = FDCAN_BRS_OFF,
    .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
    .MessageMarker = 0 // also change this to a real address if you change fifo control
	};

    TxHeader.Identifier = msgID;
    TxHeader.DataLength = length;

    TxHeader.IdType = FDCAN_EXTENDED_ID;

    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;

    uint8_t temp;
    for(uint16_t i = 0; i < length / 2; ++i)
    {
	temp = data[i];
	data[i] = data[length - i - 1];
	data[length - i - 1] = temp;
    }

	can_send(primary_can, &msg);
}*/

void SendECUStateDataOverCAN(ECU_StateData *stateData)
{
	uint32_t currentTime = MillisecondsSinceBoot();

	if (lastTickECUStateDataSent > currentTime - ECU_STATE_DATA_SEND_INTERVAL_MS) {
		return;
	}

	lastTickECUStateDataSent = currentTime;

	ECU_StateDataToSend messages = {.ECUState = stateData->ecu_state,
					.StatusBits = {stateData->status_bits[0], stateData->status_bits[1], stateData->status_bits[2]},
					.PowerLevelTorqueMap = stateData->powerlevel_torquemap,
					.MaxCellTemp = (uint8_t)(stateData->max_cell_temp_c * 4),
					.AccumulatorStateOfCharge = (uint8_t)(stateData->tractivebattery_soc),
					.GLVStateOfCharge = (uint8_t)(stateData->glv_soc),
					.TractiveSystemVoltage = (uint16_t)(stateData->ts_voltage * 100),
					.VehicleSpeed = (uint16_t)(stateData->vehicle_speed_mph * 100),
					.FRWheelRPM = (uint16_t)(stateData->fr_wheel_rpm * 10 + 32768),
					.FLWheelRPM = (uint16_t)(stateData->fl_wheel_rpm * 10 + 32768),
					.RRWheelRPM = (uint16_t)(stateData->rr_wheel_rpm * 10 + 32768),
					.RLWheelRPM = (uint16_t)(stateData->rl_wheel_rpm * 10 + 32768)};

	LOGOMATIC("Sending ECU State Data over CAN");

	ECU_CAN_Send(GRCAN_BUS_PRIMARY, ALL, MSG_ECU_STATUS_1, (void *)&messages.ECUStatusMsgOne, sizeof(messages.ECUStatusMsgOne));
	ECU_CAN_Send(GRCAN_BUS_PRIMARY, ALL, MSG_ECU_STATUS_2, (void *)&messages.ECUStatusMsgTwo, sizeof(messages.ECUStatusMsgTwo));
	ECU_CAN_Send(GRCAN_BUS_PRIMARY, ALL, MSG_ECU_STATUS_3, (void *)&messages.ECUStatusMsgThree, sizeof(messages.ECUStatusMsgThree));
}
