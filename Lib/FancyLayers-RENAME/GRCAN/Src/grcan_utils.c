#include <stdint.h>

#include "GRCAN_FancyLayer.h"
#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "Logomatic.h"
#include "can.h"
#include "main.h"

uint32_t FDCAN_DLC_BYTES(uint32_t size)
{
	switch (size) {
		case 0:
			return FDCAN_DLC_BYTES_0;
		case 1:
			return FDCAN_DLC_BYTES_1;
		case 2:
			return FDCAN_DLC_BYTES_2;
		case 3:
			return FDCAN_DLC_BYTES_3;
		case 4:
			return FDCAN_DLC_BYTES_4;
		case 5:
			return FDCAN_DLC_BYTES_5;
		case 6:
			return FDCAN_DLC_BYTES_6;
		case 7:
			return FDCAN_DLC_BYTES_7;
		case 8:
			return FDCAN_DLC_BYTES_8;
		case 12:
			return FDCAN_DLC_BYTES_12;
		case 16:
			return FDCAN_DLC_BYTES_16;
		case 20:
			return FDCAN_DLC_BYTES_20;
		case 24:
			return FDCAN_DLC_BYTES_24;
		case 32:
			return FDCAN_DLC_BYTES_32;
		case 48:
			return FDCAN_DLC_BYTES_48;
		case 64:
			return FDCAN_DLC_BYTES_64;

		default:
			LOGOMATIC("Invalid CAN FD size: %lu\n", size);
			return 0;
	}
}

uint32_t FDCAN_DLC_SIZE(uint32_t dlc)
{
	switch (dlc) {
		case FDCAN_DLC_BYTES_0:
			return 0;
		case FDCAN_DLC_BYTES_1:
			return 1;
		case FDCAN_DLC_BYTES_2:
			return 2;
		case FDCAN_DLC_BYTES_3:
			return 3;
		case FDCAN_DLC_BYTES_4:
			return 4;
		case FDCAN_DLC_BYTES_5:
			return 5;
		case FDCAN_DLC_BYTES_6:
			return 6;
		case FDCAN_DLC_BYTES_7:
			return 7;
		case FDCAN_DLC_BYTES_8:
			return 8;
		case FDCAN_DLC_BYTES_12:
			return 12;
		case FDCAN_DLC_BYTES_16:
			return 16;
		case FDCAN_DLC_BYTES_20:
			return 20;
		case FDCAN_DLC_BYTES_24:
			return 24;
		case FDCAN_DLC_BYTES_32:
			return 32;
		case FDCAN_DLC_BYTES_48:
			return 48;
		case FDCAN_DLC_BYTES_64:
			return 64;

		default:
			LOGOMATIC("Invalid CAN FD DLC code: %lu\n", dlc);
			return 8;
	}
}
