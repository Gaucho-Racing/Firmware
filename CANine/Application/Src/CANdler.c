#include "CANdler.h"

#include <stdint.h>

#include "Logomatic.h"

void LoopbackTest(uint32_t ID, void *data, uint32_t size)
{
	uint8_t *bytes = (uint8_t *)data;
	LOGOMATIC("Received CAN message with ID: 0x%lX, data: 0x", ID);
	for (uint32_t i = 0; i < size; i++) {
		LOGOMATIC("%02X", bytes[i]);
	}
	LOGOMATIC("\n");
}
void ECU_CAN_MessageHandler(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, uint8_t *data, uint32_t data_length) {
    switch (msg_id){

        case GRCAN_PING_MSG:
            GRCAN_PING_MSG *grcan_ping = (GRCAN_PING_MSG *)data;
        case GRCAN_ECU_STATUS_1_MSG:
            GRCAN_ECU_STATUS_1_MSG *grcan_ecu_status_1 = (GRCAN_ECU_STATUS_1_MSG *)data;
        case GRCAN_ECU_STATUS_2_MSG:
            GRCAN_ECU_STATUS_2_MSG *grcan_ecu_status_2 = (GRCAN_ECU_STATUS_2_MSG *)data;
        case GRCAN_ECU_STATUS_3_MSG:
            GRCAN_ECU_STATUS_3_MSG *grcan_ecu_status_3 = (GRCAN_ECU_STATUS_3_MSG *)data;
        case GRCAN_ECU_CONFIG_MSG:
            GRCAN_ECU_CONFIG_MSG *grcan_ecu_config = (GRCAN_ECU_CONFIG_MSG *)data;
        case GRCAN_BCU_STATUS_1_MSG:
            GRCAN_BCU_STATUS_1_MSG *grcan_bcu_status_1 = (GRCAN_BCU_STATUS_1_MSG *)data;
        case GRCAN_BCU_STATUS_2_MSG:
            GRCAN_BCU_STATUS_2_MSG *bcu_status_2 = (GRCAN_BCU_STATUS_2_MSG *)data;
        case GRCAN_BCU_STATUS_3_MSG:
            GRCAN_BCU_STATUS_3_MSG *grcan_bcu_status_3 = (GRCAN_BCU_STATUS_3_MSG *)data;
        case GRCAN_BCU_PRECHARGE_MSG:
            GRCAN_BCU_PRECHARGE_MSG *grcan_bcu_precharge = (GRCAN_BCU_PRECHARGE_MSG *)data;
        case GRCAN_BCU_CONFIG_CHARGE_PARAMETERS_MSG:
            GRCAN_BCU_CONFIG_CHARGE_PARAMETERS_MSG *grcan_bcu_config_charge_parameters = (GRCAN_BCU_CONFIG_CHARGE_PARAMETERS_MSG *)data;
        case GRCAN_BCU_CONFIG_OPERATIONAL_PARAMETERS_MSG:
            GRCAN_BCU_CONFIG_OPERATIONAL_PARAMETERS_MSG *grcan_bcu_config_operational_parameters = (GRCAN_BCU_CONFIG_OPERATIONAL_PARAMETERS_MSG *)data;
        case GRCAN_BCU_CELL_DATA_1_MSG:
            GRCAN_BCU_CELL_DATA_1_MSG *grcan_bcu_cell_data_1 = (GRCAN_BCU_CELL_DATA_1_MSG *)data;
        case GRCAN_BCU_CELL_DATA_2_MSG:
            GRCAN_BCU_CELL_DATA_2_MSG *grcan_bcu_cell_data_2 = (GRCAN_BCU_CELL_DATA_2_MSG *)data;
        case GRCAN_BCU_CELL_DATA_3_MSG:
            GRCAN_BCU_CELL_DATA_3_MSG *grcan_bcu_cell_data_3 = (GRCAN_BCU_CELL_DATA_3_MSG *)data;
        case GRCAN_BCU_CELL_DATA_4_MSG:
            GRCAN_BCU_CELL_DATA_4_MSG *grcan_bcu_cell_data_4 = (GRCAN_BCU_CELL_DATA_4_MSG *)data;
        case GRCAN_BCU_CELL_DATA_5_MSG:
            GRCAN_BCU_CELL_DATA_5_MSG *grcan_bcu_cell_data_5 = (GRCAN_BCU_CELL_DATA_5_MSG *)data;
        case GRCAN_INVERTER_STATUS_1_MSG:
            GRCAN_INVERTER_STATUS_1_MSG *grcan_inverter_status_1 = (GRCAN_INVERTER_STATUS_1_MSG *)data;
        case GRCAN_INVERTER_STATUS_2_MSG:
            GRCAN_INVERTER_STATUS_2_MSG *grcan_inverter_status_2 = (GRCAN_INVERTER_STATUS_2_MSG *)data;
 