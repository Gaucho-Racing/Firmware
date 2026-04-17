# Fancy GRCAN

It provides:
- default bus configuration
- internal CAN initialization
- local node ID handling
- simplified message transmission
- optional bus deactivation when switching buses


How to use:
Make sure to #define USECANx in your can_cfg.h
1:
void GRCAN_SetDefaultBusConfig(GRCAN_BusConfig *busCfg, GRCAN_BUS_ID bus);
-- applies default settings for CAN based on ECU CAN
-- default uses PB12 RD, PB13 TD, and FDCAN2
-- Make sure to change the necessary items in GRCAN_BUS_ID struct

GRCAN_BusConfig is defined in grcan_utils.h

2:
bool GRCAN_InitBus(GRCAN_BUS_ID bus)
-- initializes CAN (similar to MX_FDCANx_Init)
-- returns bool to see if succesful

3:
void GRCAN_SetLocalNodeID(GRCAN_NODE_ID localID)
-- sets local node ID for transmission
-- make sure to do this before sending

4:
bool GRCAN_Fancy_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size)
--use this to send
--returns true or false based on success

5:
bool GRCAN_DeactivateBus(GRCAN_BUS_ID bus)
--make sure to deactivate current bus if you choose to send from different bus

Note:
For changing GRCAN_BusConfig settings only touch the structs,
the initialization is handled internally
