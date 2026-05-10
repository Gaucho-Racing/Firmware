#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "GRCAN_FancyLayer.h"
#include "Logomatic.h"
#include "stdlib.h"

static volatile uint32_t rx_received = 0;
static volatile bool data_valid = false;
static uint8_t expected_data[TX_BUFFER_2_SIZE];
static volatile uint32_t expected_size = 0;
static volatile uint32_t expected_id = 0;
static volatile bool id_valid = false;

static void can_test_rx_callback(uint32_t id, void *data, uint32_t size)
{
	rx_received++;
	if (size > sizeof(expected_data)) {
		data_valid = false;
		return;
	}
	data_valid = (size == expected_size) && (memcmp(data, expected_data, size) == 0);
	id_valid = (id == expected_id);

	LOGOMATIC("\nCallback triggered: ID=%" PRIu32 ", Size=%lu, Data[0]=0x%x\n", id, (unsigned long)size, *(uint8_t *)data);
}

// TODO - allow user to send data without needing to construct a header for the buffer
//  TODO: G4 tests are dependent on the System clock configuration??

int GRCAN_Validate_InitBus(GRCAN_BUS_ID bus, GRCAN_OperatingMode mode, FDCAN_GlobalTypeDef *fdcan_instance)
{
	GRCAN_BusConfig bus_config;
	GRCAN_SetDefaultBusConfig(&bus_config, bus);
	bus_config.operating_mode = mode; // GRCAN_OPMODE_INTERNAL_LOOPBACK is only one tested so far
	bus_config.fdcan_instance = fdcan_instance;
	bus_config.rx_callback = can_test_rx_callback; // test loopback

	LOGOMATIC("Testing GRCAN_InitBus...\n");
	bool result = GRCAN_InitBus(&bus_config);

	if (result == true) {
		LOGOMATIC("GRCAN_InitBus PASSED.\n");
		return 1;
	} else {
		LOGOMATIC("GRCAN_InitBus FAILED.\n");
		return 0;
	}
}

// GRCAN_ALL = 0xFF,
// 	GRCAN_ACU = 0x03,
// 	GRCAN_ECU = 0x02,
// 	GRCAN_CCU = 0x02,
// 	GRCAN_Charger = 0x00,
// 	GRCAN_Charging_SDC = 0x0C,
// 	GRCAN_DGPS = 0x30,
// 	GRCAN_Dash_Panel = 0x05,
// 	GRCAN_Debugger = 0x01,
// 	GRCAN_Fan_Controller_1 = 0x0D,
// 	GRCAN_Fan_Controller_2 = 0x0E,
// 	GRCAN_Fan_Controller_3 = 0x0F,
// 	GRCAN_GR_Inv = 0x08,
// 	GRCAN_TCM = 0x04,

GRCAN_NODE_ID get_nodeID(GRCAN_BUS_ID bus)
{
	switch (bus) {
		case GRCAN_BUS_PRIMARY:
			return GRCAN_ACU;
		case GRCAN_BUS_DATA:
			return GRCAN_ECU;
		case GRCAN_BUS_TESTING:
			return GRCAN_Debugger;
		case GRCAN_BUS_CHARGER:
			return GRCAN_Charger;
		default:
			return GRCAN_ALL; // All causes error
	}
}

// typedef enum {
// 	GRCAN_DEBUG_2_0 = 0x000,
// 	GRCAN_DEBUG_FD = 0x001,
// 	GRCAN_PING = 0x002,
// 	GRCAN_ECU_STATUS_1 = 0x003,
// 	GRCAN_ECU_STATUS_2 = 0x004,
// 	GRCAN_ECU_STATUS_3 = 0x005,
// 	GRCAN_ECU_CONFIG = 0x006,
// 	GRCAN_ACU_STATUS_1 = 0x007,
// 	GRCAN_ACU_STATUS_2 = 0x008,
// 	GRCAN_ACU_STATUS_3 = 0x009,
// 	GRCAN_ACU_PRECHARGE = 0x00A,
// 	GRCAN_ACU_CONFIG_CHARGE_PARAMETERS = 0x00B,
// 	GRCAN_ACU_CONFIG_OPERATIONAL_PARAMETERS = 0x00C,
// 	GRCAN_ACU_CELL_DATA_1 = 0x00D,
// 	GRCAN_ACU_CELL_DATA_2 = 0x00E,
// 	GRCAN_ACU_CELL_DATA_3 = 0x00F,
// 	GRCAN_ACU_CELL_DATA_4 = 0x010,
// 	GRCAN_ACU_CELL_DATA_5 = 0x011,
// 	GRCAN_INV_STATUS_1 = 0x013,
// 	GRCAN_INV_STATUS_2 = 0x014,
// 	GRCAN_INV_STATUS_3 = 0x015,
// 	GRCAN_INV_CONFIG = 0x016,
// 	GRCAN_INV_CMD = 0x017,
// 	GRCAN_FAN_STATUS = 0x018,
// 	GRCAN_FAN_COMMAND = 0x019,
// 	GRCAN_DASH_STATUS = 0x01A,
// 	GRCAN_DASH_CONFIG = 0x01B,
// 	GRCAN_TCM_STATUS = 0x029,
// 	GRCAN_TCM_RESOURCE_UTILIZATION = 0x02A,
// 	GRCAN_DASH_WARNING_FLAGS = 0x02B,
// 	GRCAN_ECU_ANALOG_DATA = 0x02E,
// 	GRCAN_GPS_LAT = 0x031,
// 	GRCAN_GPS_LON = 0x032,
// 	GRCAN_GPS_ALT = 0x033,
// 	GRCAN_GPS_PX = 0x034,
// 	GRCAN_GPS_QY = 0x035,
// 	GRCAN_GPS_RZ = 0x036,
// 	GRCAN_UVW_DGPS = 0x030,
// 	GRCAN_ECU_PERFORMANCE = 0x123,
// } GRCAN_MSG_ID;

GRCAN_MSG_ID get_messageID(GRCAN_BUS_ID bus)
{
	switch (bus) {
		case GRCAN_BUS_PRIMARY:
			return GRCAN_ACU_STATUS_1;
		case GRCAN_BUS_DATA:
			return GRCAN_ECU_STATUS_1;
		case GRCAN_BUS_TESTING:
			return GRCAN_DEBUG_2_0;
		case GRCAN_BUS_CHARGER:
			return GRCAN_ACU_CONFIG_CHARGE_PARAMETERS;
		default:
			return (GRCAN_MSG_ID)0xFF; // Invalid message ID
	}
}

int GRCAN_SendReceive(GRCAN_BUS_ID bus, GRCAN_NODE_ID nodeID, GRCAN_NODE_ID dest_nodeID, GRCAN_MSG_ID messageID, void *data, uint32_t size)
{
	if (data == NULL) {
		data = "Hello";
		size = sizeof("Hello");
	}

	if (size > sizeof(expected_data)) {
		LOGOMATIC("Data size exceeds expected_data buffer size. Test cannot proceed.\n");
		return 0;
	}

	// typedef struct {
	// 	GRCAN_NODE_ID srcID;
	// 	GRCAN_NODE_ID destNode;
	// 	GRCAN_MSG_ID messageID;
	// } GRCAN_Fancy_ID;

	expected_size = size;
	GRCAN_Fancy_ID GRCAN_Fancy_ID;
	GRCAN_Fancy_ID.srcID = nodeID;
	GRCAN_Fancy_ID.destNode = dest_nodeID;
	GRCAN_Fancy_ID.messageID = messageID;
	expected_id = GRCAN_Fancy_EncodeID(&GRCAN_Fancy_ID);

	rx_received = 0;
	data_valid = false;
	id_valid = false;

	GRCAN_SetLocalNodeID(nodeID);

	memcpy(expected_data, data, size);
	bool send_result = GRCAN_Fancy_Send(bus, dest_nodeID, messageID, data, size);

	if (!send_result) {
		LOGOMATIC("GRCAN_Fancy_Send failed to send message on bus %d\n", bus);
		return 0;
	}

	HAL_Delay(1);

	if (rx_received > 0 && data_valid && id_valid) {
		LOGOMATIC("GRCAN_Fancy_Send PASSED. Callback verified and data is valid.\n");
		return 1;
	} else {
		LOGOMATIC("GRCAN_Fancy_Send FAILED. Callback not triggered or data is invalid.\n");
		return 0;
	}
}

int GRCAN_ErrorHandling()
{
	GRCAN_BusConfig invalid_config = {0};

	LOGOMATIC("\nTesting GRCAN_InitBus with invalid config...\n");
	bool result = GRCAN_InitBus(&invalid_config);
	if (result == false) {
		LOGOMATIC("GRCAN_InitBus error handling PASSED.\n");
		return 1;
	} else {
		LOGOMATIC("GRCAN_InitBus error handling FAILED.\n");
		return 0;
	}
}

void get_test_payload(int idx, void **data, uint32_t *size)
{
	static uint8_t msg1[] = "Hello";
	static uint8_t msg2[] = "World";
	static uint8_t msg3[] = {12};
	static uint8_t msg4[] = {23};
	static uint8_t msg5[] = {0x00, 0x00, 0x00, 0x00};
	static uint8_t msg6[] = {0xFF, 0xFF, 0xFF, 0xFF};
	static uint8_t msg7[] = {0xAA, 0x55, 0xAA, 0x55};
	static uint8_t msg8[] = {0, 1, 2, 3, 4, 5, 6, 7};

	switch (idx) {
		case 0:
			*data = msg1;
			*size = sizeof(msg1);
			break;
		case 1:
			*data = msg2;
			*size = sizeof(msg2);
			break;
		case 2:
			*data = msg3;
			*size = sizeof(msg3);
			break;
		case 3:
			*data = msg4;
			*size = sizeof(msg4);
			break;
		case 4:
			*data = msg5;
			*size = sizeof(msg5);
			break;
		case 5:
			*data = msg6;
			*size = sizeof(msg6);
			break;
		case 6:
			*data = msg7;
			*size = sizeof(msg7);
			break;
		case 7:
			*data = msg8;
			*size = sizeof(msg8);
			break;
		default:
			*data = NULL;
			*size = 0;
			break;
	}
}

int GRCAN_BurstSendTest(GRCAN_BUS_ID bus, GRCAN_NODE_ID nodeID, GRCAN_NODE_ID dest_nodeID, GRCAN_MSG_ID messageID, uint32_t count)
{
	uint8_t burst_msg[] = {0x10, 0x20, 0x30, 0x40};

	LOGOMATIC("Starting burst send test on bus %d with %lu messages...\n", bus, (unsigned long)count);

	for (uint32_t i = 0; i < count; i++) {
		if (!GRCAN_SendReceive(bus, nodeID, dest_nodeID, messageID, burst_msg, sizeof(burst_msg))) {
			LOGOMATIC("Burst send test FAILED on iteration %lu.\n", (unsigned long)i);
			return 0;
		}
	}

	LOGOMATIC("Burst send test PASSED on bus %d.\n", bus);
	return 1;
}

int GRCAN_InitDeactivateStressTest(GRCAN_BUS_ID bus, GRCAN_NODE_ID nodeID, GRCAN_NODE_ID dest_nodeID, GRCAN_MSG_ID messageID, FDCAN_GlobalTypeDef *fdcan_instance, uint32_t iterations)
{
	uint8_t stress_msg[] = "Stress";

	LOGOMATIC("Starting init/deactivate stress test on bus %d for %lu iterations...\n", bus, (unsigned long)iterations);

	for (uint32_t i = 0; i < iterations; i++) {
		if (!GRCAN_Validate_InitBus(bus, GRCAN_OPMODE_INTERNAL_LOOPBACK, fdcan_instance)) {
			LOGOMATIC("Stress test FAILED during init on iteration %lu.\n", (unsigned long)i);
			return 0;
		}

		if (!GRCAN_SendReceive(bus, nodeID, dest_nodeID, messageID, stress_msg, sizeof(stress_msg))) {
			LOGOMATIC("Stress test FAILED during send/receive on iteration %lu.\n", (unsigned long)i);
			GRCAN_DeactivateBus(bus);
			return 0;
		}

		if (!GRCAN_DeactivateBus(bus)) {
			LOGOMATIC("Stress test FAILED during deactivate on iteration %lu.\n", (unsigned long)i);
			return 0;
		}
	}

	LOGOMATIC("Init/deactivate stress test PASSED on bus %d.\n", bus);
	return 1;
}

int FancyCAN_LoopbackTest(void)
{
	GRCAN_BUS_ID bus;

	for (bus = GRCAN_BUS_TESTING; bus <= GRCAN_BUS_CHARGER; bus++) {
		LOGOMATIC("\n--- Testing bus %d ---\n", bus);
		int res1 = GRCAN_Validate_InitBus(bus, GRCAN_OPMODE_INTERNAL_LOOPBACK, FDCAN2);
		// GRCAN_OPMODE_EXTERNAL_LOOPBACK needs to be tested, can change FDCAN: make sure to #define USECANx
		GRCAN_NODE_ID nodeID = get_nodeID(bus);
		GRCAN_MSG_ID messageID = get_messageID(bus);
		GRCAN_NODE_ID dest_nodeID = nodeID; // Loopback to self for internal
		int res2 = 1;
		for (int i = 0; i < 8; i++) {
			void *data;
			uint32_t size;
			get_test_payload(i, &data, &size);

			LOGOMATIC("\nTesting GRCAN_SendReceive on bus %d...\n", bus);
			res2 = GRCAN_SendReceive(bus, nodeID, dest_nodeID, messageID, data, size); //
			if (res2 == 0) {
				uint8_t data_value = *(uint8_t *)data;
				switch (bus) {
					case GRCAN_BUS_PRIMARY:
						LOGOMATIC("Testing Bus:PRIMARY Loopback Test FAILED for message with first byte: %d.\n", data_value);
						break;
					case GRCAN_BUS_DATA:
						LOGOMATIC("Testing Bus:DATA Loopback Test FAILED for message with first byte: %d.\n", data_value);
						break;
					case GRCAN_BUS_CHARGER:
						LOGOMATIC("Testing Bus:CHARGER Loopback Test FAILED for message with first byte: %d.\n", data_value);
						break;
					case GRCAN_BUS_TESTING:
						LOGOMATIC("Testing Bus:TESTING Loopback Test FAILED for message with first byte: %d.\n", data_value);
						break;
					default:
						LOGOMATIC("Testing Bus:UNKNOWN Loopback Test FAILED for message with first byte: %d.\n", data_value);
						break;
				}
				break;
			}
		}

		bool res4 = GRCAN_DeactivateBus(bus);

		if (!res1 || !res2) {
			switch (bus) {
				case GRCAN_BUS_PRIMARY:
					LOGOMATIC("Testing Bus:PRIMARY Loopback Test FAILED during initialization or send/receive test.\n");
					break;
				case GRCAN_BUS_DATA:
					LOGOMATIC("Testing Bus:DATA Loopback Test FAILED during initialization or send/receive test.\n");
					break;
				case GRCAN_BUS_TESTING:
					LOGOMATIC("Testing Bus:TESTING Loopback Test FAILED during initialization or send/receive test.\n");
					break;
				case GRCAN_BUS_CHARGER:
					LOGOMATIC("Testing Bus:CHARGER Loopback Test FAILED during initialization or send/receive test.\n");
					break;
				default:
					LOGOMATIC("Testing Bus:UNKNOWN Loopback Test FAILED during initialization or send/receive test.\n");
					break;
			}
		}
		if (!res4) {
			switch (bus) {
				case GRCAN_BUS_PRIMARY:
					LOGOMATIC("Testing Bus:PRIMARY Loopback Test FAILED during bus deactivation.\n");
					break;
				case GRCAN_BUS_DATA:
					LOGOMATIC("Testing Bus:DATA Loopback Test FAILED during bus deactivation.\n");
					break;
				case GRCAN_BUS_TESTING:
					LOGOMATIC("Testing Bus:TESTING Loopback Test FAILED during bus deactivation.\n");
					break;
				case GRCAN_BUS_CHARGER:
					LOGOMATIC("Testing Bus:CHARGER Loopback Test FAILED during bus deactivation.\n");
					break;
				default:
					LOGOMATIC("Testing Bus:UNKNOWN Loopback Test FAILED during bus deactivation.\n");
					break;
			}
		}
	}

	if (bus < 4) {
		LOGOMATIC("\nLoopback Test FAILED. Not all buses were tested.\n");
		return 0;
	}

	// The next tests takes a while, completely fine to remove
	for (bus = GRCAN_BUS_TESTING; bus <= GRCAN_BUS_CHARGER; bus++) {
		LOGOMATIC("\n--- Testing burst send on bus %d ---\n", bus);
		GRCAN_Validate_InitBus(bus, GRCAN_OPMODE_INTERNAL_LOOPBACK, FDCAN2);
		int burst_result = GRCAN_BurstSendTest(bus, get_nodeID(bus), get_nodeID(bus), get_messageID(bus), 100);
		if (!burst_result) {
			LOGOMATIC("\nLoopback Test FAILED during burst send test on bus %d.\n", bus);
			return 0;
		}
		GRCAN_DeactivateBus(bus);
	}

	for (bus = GRCAN_BUS_TESTING; bus <= GRCAN_BUS_CHARGER; bus++) {
		LOGOMATIC("\n--- Testing init/deactivate stress test on bus %d ---\n", bus);
		int stress_result = GRCAN_InitDeactivateStressTest(bus, get_nodeID(bus), get_nodeID(bus), get_messageID(bus), FDCAN2, 50);
		if (!stress_result) {
			LOGOMATIC("\nLoopback Test FAILED during init/deactivate stress test on bus %d.\n", bus);
			return 0;
		}
	}

	int res3 = GRCAN_ErrorHandling();
	if (!res3) {
		LOGOMATIC("\nLoopback Test FAILED during error handling test.\n");
		return 0;
	}

	return 1;
}
