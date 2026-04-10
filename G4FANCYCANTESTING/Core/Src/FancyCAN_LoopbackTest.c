#include <inttypes.h>
#include <string.h>

#include "Logomatic.h"
#include "grcan_fancylayer.h"
#include "stdlib.h"

#include <assert.h>


static volatile uint32_t rx_received = 0;
static volatile bool data_valid = false;
static uint8_t expected_data[] = "Hello";

static void can_test_rx_callback(uint32_t id, void *data, uint32_t size) {
    rx_received++;
    if (size == sizeof(expected_data) && memcmp(data, expected_data, size) == 0) {
        data_valid = true;
    } else {
        data_valid = false;
    }
    LOGOMATIC("\nCallback triggered: ID=%" PRIu32 ", Size=%ld, Data[0]=0x%x\n", id, size, *(uint8_t *)data);
}

// TODO - allow user to send data without needing to construct a header for the buffer
//  TODO: G4 tests are dependent on the System clock configuration??

int GRCAN_Validate_InitBus(GRCAN_BUS_ID bus) {
    GRCAN_BusConfig bus_config;
    GRCAN_SetDefaultBusConfig(&bus_config, bus);
    bus_config.operating_mode = GRCAN_OPMODE_INTERNAL_LOOPBACK;
    bus_config.fdcan_instance = FDCAN2;
	bus_config.rx_callback = can_test_rx_callback; //test loopback

    LOGOMATIC("Testing GRCAN_InitBus...\n");
    bool result = GRCAN_InitBus(&bus_config);
    if (result == true) {
        LOGOMATIC("GRCAN_InitBus PASSED.\n");
		return 1;
    }
    else {
        LOGOMATIC("GRCAN_InitBus FAILED.\n");
		return 0;
    }
}

// GRCAN_ALL = 0xFF,
// 	GRCAN_BCU = 0x03,
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
// 	GRCAN_GR_Inverter = 0x08,
// 	GRCAN_TCM = 0x04,
int GRCAN_SendReceive(GRCAN_BUS_ID bus) {
	rx_received = 0;
	data_valid = false;

	if (bus == GRCAN_BUS_TESTING) {
		LOGOMATIC("Testing GRCAN_SendReceive on TESTING bus...\n");
		GRCAN_SetLocalNodeID(0x01);
	}
	else if (bus == GRCAN_BUS_DATA) {
		LOGOMATIC("Testing GRCAN_SendReceive on DATA bus...\n");
		GRCAN_SetLocalNodeID(0x02);
	}
	else if (bus == GRCAN_BUS_CHARGER) {
		LOGOMATIC("Testing GRCAN_SendReceive on CHARGER bus...\n");
		GRCAN_SetLocalNodeID(0x00);
	}
	else if (bus == GRCAN_BUS_PRIMARY) {
		LOGOMATIC("Testing GRCAN_SendReceive on PRIMARY bus...\n");
		GRCAN_SetLocalNodeID(0x03);
	}
	else {
		LOGOMATIC("Testing GRCAN_SendReceive on UNKNOWN bus...\n");
		return 0;
	}

    uint8_t data[] = "Hello";
    memcpy(expected_data, data, sizeof(data));
    GRCAN_Fancy_Send(bus, 2, 0x12, data, sizeof(data));

    HAL_Delay(1000);

    if (rx_received > 0 && data_valid) {
        LOGOMATIC("GRCAN_Fancy_Send PASSED. Callback verified and data is valid.\n");
        return 1;
    } else {
        LOGOMATIC("GRCAN_Fancy_Send FAILED. Callback not triggered or data is invalid.\n");
        return 0;
    }
}

int GRCAN_ErrorHandling() {
    GRCAN_BusConfig invalid_config = {0};

    LOGOMATIC("\nTesting GRCAN_InitBus with invalid config...\n");
    bool result = GRCAN_InitBus(&invalid_config);
    if (result == false) {
        LOGOMATIC("GRCAN_InitBus error handling PASSED.\n");
		return 1;
    }
    else {
       LOGOMATIC("GRCAN_InitBus error handling FAILED.\n");
	   return 0;
    }
}

int FancyCAN_LoopbackTest(void)
{
	for (GRCAN_BUS_ID bus = GRCAN_BUS_PRIMARY; bus <= GRCAN_BUS_CHARGER; bus++) {
		LOGOMATIC("\n--- Testing bus %d ---\n", bus);

		int res1 = GRCAN_Validate_InitBus(GRCAN_BUS_TESTING);
		int res2 = GRCAN_SendReceive(GRCAN_BUS_TESTING);
		int res3 = GRCAN_ErrorHandling();
		bool res4 = GRCAN_DeactivateBus(GRCAN_BUS_TESTING);
		if (!res1 || !res2 || !res3) {
			switch(bus) {
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
			switch(bus) {
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
    return 1;
}
