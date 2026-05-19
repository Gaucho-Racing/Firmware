#include "CANdler.h"

#include <string.h>

#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "NodeID.h"
#include "Unused.h"
#include "bitManipulations.h"
#include "can.h"
#include "main.h"
#include "stm32g4xx_hal_fdcan.h"

#define ECU_ID GRCAN_ECU
#define PING_ID MSG_PING

CANHandle *can_handler;

void CANInitialize()
{
	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PLL);
	CANConfig my_cfg;
	get_cfg(FDCAN1, CAN_callback, &my_cfg, FDCAN_MODE_EXTERNAL_LOOPBACK, 0, 0);
	can_handler = can_init(&my_cfg);
}

uint16_t _temp_f2u16(float temp)
{
	float v = ((temp + 70.f) / 450.0f); // Normalize temp (ranges from -70 C to +380 C)
	if (v < 0.0f) {
		v = 0.0f;
	}
	if (v > 1.0f) {
		v = 1.0f;
	}
	return (uint16_t)(v * 65535.0f);
}

void CAN_sendTemp(float to)
{
	FDCANTxMessage msg;
	msg.tx_header.Identifier = (LOCAL_GR_ID << 20) | (GRCAN_BRAKE_TEMP << 8) | GRCAN_TCM; // do this
	msg.tx_header.IdType = FDCAN_EXTENDED_ID;
	msg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
	msg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE; // honestly this might be a value you have to read from a node
							      // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
	msg.tx_header.DataLength = FDCAN_DLC_BYTES_2;
	msg.tx_header.BitRateSwitch = FDCAN_BRS_ON;
	msg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
	msg.tx_header.MessageMarker = 0;		       // also change this to a real address if you change fifo control

	((uint16_t *)(msg.data))[0] = _temp_f2u16(to);

	can_send(can_handler, &msg);
}

void CAN_sendPing(GRCAN_NODE_ID to, uint32_t data)
{
	FDCANTxMessage pingMsg;
	pingMsg.tx_header.Identifier = (LOCAL_GR_ID << 20) | (GRCAN_PING << 8) | to;
	pingMsg.tx_header.IdType = FDCAN_EXTENDED_ID;
	pingMsg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
	pingMsg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	pingMsg.tx_header.DataLength = FDCAN_DLC_BYTES_4;
	pingMsg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	pingMsg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	pingMsg.tx_header.MessageMarker = 0;

	GRCAN_PING_MSG msg = {.timestamp = data};
	memcpy(pingMsg.data, &msg, sizeof(msg));

	can_send(can_handler, &pingMsg);
}

void CAN_callback(uint32_t ID, void *data, uint32_t size)
{
	UNUSED(ID);
	UNUSED(data);
	UNUSED(size);
	volatile int i = 0;
	// GRCAN_MSG_ID msg_id = (0x000FFF00 & ID) >> 8;
	// GRCAN_NODE_ID node_id = (0x0FF00000 & ID) >> 20;

	// // Process data
	// // ECU Status
	// if (msg_id == GRCAN_ECU_STATUS_1 && size == sizeof(GRCAN_ECU_STATUS_1_MSG)) {
	// 	GRCAN_ECU_STATUS_1_MSG *ecu_data = (GRCAN_ECU_STATUS_1_MSG *)data;
	// 	dashStatus.ECUState = ecu_data->ecu_state; // Get ECU Status

	// 	// Dash Config
	// } else if (msg_id == GRCAN_DASH_CONFIG && size == sizeof(GRCAN_DASH_CONFIG_MSG)) {
	// 	GRCAN_DASH_CONFIG_MSG *dash_data = (GRCAN_DASH_CONFIG_MSG *)data;
	// 	dashStatus.led_bits = dash_data->led_bits; // Get LED bits

	// 	// Ping
	// } else if (msg_id == GRCAN_PING && size == sizeof(GRCAN_PING_MSG)) {
	// 	CAN_sendPing(node_id, *(uint32_t *)data);

	// } else {
	// 	// Check that you are sending the correct sizes if you get this message
	// 	LOGOMATIC("Unrecognized CAN message.\n");
	// }

	return;
}
