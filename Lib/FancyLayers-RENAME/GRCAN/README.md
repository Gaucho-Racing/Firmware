# Fancy GRCAN

How to use:
Make sure to #define USECANx in your can_cfg.h
1:
void GRCAN_SetDefaultBusConfig(GRCAN_BusConfig *busCfg, GRCAN_BUS_ID bus);
-- grcan_utils.c
-- applies default settings for CAN based on ECU CAN
-- default uses PB12 RD, PB13 TD, and FDCAN2
-- change the necessary items in GRCAN_BusConfig struct

GRCAN_BusConfig is defined in grcan_utils.h

2:
bool GRCAN_InitBus(GRCAN_BUS_ID bus)
-- grcan_fancylayer.c
-- initializes CAN for bus (similar to MX_FDCANx_Init)

3:
void GRCAN_SetLocalNodeID(GRCAN_NODE_ID localID)
-- grcan_fancylayer.c
-- sets local node ID for transmission

4:
bool GRCAN_Fancy_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size)
--grcan_fancylayer.c

5:
bool GRCAN_DeactivateBus(GRCAN_BUS_ID bus)
--grcan_fancylayer.c
--deactivate current bus if you choose to send from different bus
