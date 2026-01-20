#include "CANutils.h"

#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_DAT.h"
#include "GR_OLD_MSG_ID.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateTicks.h"
#include "StateUtils.h"
#include "can.h"
#include "main.h"
#include "string.h"

uint32_t lastTickECUStateDataSent = 0;

void ECU_CAN_Send(GR_OLD_BUS_ID bus, GR_OLD_NODE_ID destNode, GR_OLD_MSG_ID messageID, void *data, uint32_t size)
{
	if (size > FDCAN_MAX_DATA_BYTES) {
		size = FDCAN_MAX_DATA_BYTES;
		LOGOMATIC("Tried to send more than 64 bytes over CAN");
	}

	uint32_t ID = ((0xFF & LOCAL_GR_ID) << 20) & ((0xFFF & messageID) << 8) & (0xFF & destNode);

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

	if (bus == GR_OLD_BUS_PRIMARY) {
		can_send(primary_can, &msg);
	} else if (bus == GR_OLD_BUS_DATA) {
		can_send(data_can, &msg);
	} else {
		LOGOMATIC("CAN: Invalid bus ID %d\n", bus);
	}
}

void SendECUStateDataOverCAN(ECU_StateData *stateData)
{
	uint32_t currentTime = millis();

	if (lastTickECUStateDataSent > currentTime - ECU_STATE_DATA_SEND_INTERVAL_MS) {
		return;
	}

	lastTickECUStateDataSent = currentTime;

	ECU_StateDataToSend messages = {.ECUState = stateData->ecu_state,
					.StatusBits = {stateData->status_bits[0], stateData->status_bits[1], stateData->status_bits[2]},
					.PowerLevelTorqueMap = stateData->powerlevel_torquemap,
					.MaxCellTemp = (uint8_t)(stateData->max_cell_temp * 4),
					.AccumulatorStateOfCharge = (uint8_t)(stateData->tractivebattery_soc * 51 / 20),
					.GLVStateOfCharge = (uint8_t)(stateData->glv_soc * 51 / 20),
					.TractiveSystemVoltage = (uint16_t)(stateData->ts_voltage * 100),
					.VehicleSpeed = (uint16_t)(stateData->vehicle_speed * 100),
					.FRWheelRPM = (uint16_t)(stateData->fr_wheel_rpm * 10 + 32768),
					.FLWheelRPM = (uint16_t)(stateData->fl_wheel_rpm * 10 + 32768),
					.RRWheelRPM = (uint16_t)(stateData->rr_wheel_rpm * 10 + 32768),
					.RLWheelRPM = (uint16_t)(stateData->rl_wheel_rpm * 10 + 32768)};

	LOGOMATIC("Sending ECU State Data over CAN");

	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_ALL, MSG_ECU_STATUS_1, (void *)&messages.ECUStatusMsgOne, sizeof(messages.ECUStatusMsgOne));
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_ALL, MSG_ECU_STATUS_2, (void *)&messages.ECUStatusMsgTwo, sizeof(messages.ECUStatusMsgTwo));
	ECU_CAN_Send(GR_OLD_BUS_PRIMARY, GR_ALL, MSG_ECU_STATUS_3, (void *)&messages.ECUStatusMsgThree, sizeof(messages.ECUStatusMsgThree));
}
