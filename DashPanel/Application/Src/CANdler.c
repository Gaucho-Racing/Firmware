#include "CANdler.h"

#include <string.h>

#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "Unused.h"
#include "bitManipulations.h"
#include "can.h"
#include "dashutils.h"
#include "main.h"
#include "stm32g4xx_hal_fdcan.h"

#define ECU_ID GRCAN_ECU
#define PING_ID MSG_PING

CANHandle *can_handler;
DashStatus dashStatus = {0};
bool canReadyToSend;

void CANInitialize()
{
	canReadyToSend = false;

	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

	CANConfig my_cfg;

	get_cfg(FDCAN1, on_receive, &my_cfg, FDCAN_MODE_NORMAL);

	CANHandle *h1 = can_init(&my_cfg);
}

void CAN_sendPing(GRCAN_NODE_ID to, uint32_t data)
{
	FDCANTxMessage pingMsg;
	pingMsg.tx_header.Identifier = (GRCAN_Dash_Panel << 20) | (GRCAN_PING << 8) | to;
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

void CAN_sendECU(CANHandle *c, GRCAN_DASH_STATUS_MSG *msg, GRCAN_NODE_ID to)
{

	FDCANTxMessage sendECUMsg;
	sendECUMsg.tx_header.Identifier = (GRCAN_Dash_Panel << 20) | (GRCAN_DASH_STATUS << 8) | to;
	sendECUMsg.tx_header.IdType = FDCAN_EXTENDED_ID;
	sendECUMsg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
	sendECUMsg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	sendECUMsg.tx_header.DataLength = FDCAN_DLC_BYTES_2;
	sendECUMsg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	sendECUMsg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	sendECUMsg.tx_header.MessageMarker = 0;

	((uint32_t *)(sendECUMsg.data))[0] = (uint32_t)msg;

	can_send(c, &sendECUMsg);
}

void CAN_callback(uint32_t ID, void *data, uint32_t size)
{

	GRCAN_MSG_ID msg_id = (0x000FFF00 & ID) >> 8;
	GRCAN_NODE_ID node_id = (0x0FF00000 & ID) >> 20;

	// Process data
	// ECU Status
	if (msg_id == GRCAN_ECU_STATUS_1 && size == sizeof(GRCAN_ECU_STATUS_1_MSG)) {
		GRCAN_ECU_STATUS_1_MSG *ecu_data = (GRCAN_ECU_STATUS_1_MSG *)data;
		dashStatus.ECUState = ecu_data->ecu_state; // Get ECU Status

		// Dash Config
	} else if (msg_id == GRCAN_DASH_CONFIG && size == sizeof(GRCAN_DASH_CONFIG_MSG)) {
		GRCAN_DASH_CONFIG_MSG *dash_data = (GRCAN_DASH_CONFIG_MSG *)data;
		dashStatus.led_bits = dash_data->led_bits; // Get LED bits

		// Ping
	} else if (msg_id == GRCAN_PING && size == sizeof(GRCAN_PING_MSG)) {
		CAN_sendPing(node_id, *(uint32_t *)data);

	} else {
		// Check that you are sending the correct sizes if you get this message
		LOGOMATIC("Unrecognized CAN message.\n");
	}
}
