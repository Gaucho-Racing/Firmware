#include "CubeCAN.h"
#include "Unused.h"


HAL_StatusTypeDef CubeCAN_Send(CubeCAN_Handle *const handle, const GRCAN_NODE_ID rx_node, const GRCAN_MSG_ID msg_id, const void *const data, const uint8_t size){

    UNUSED(handle);
    UNUSED(rx_node);
    UNUSED(msg_id);
    UNUSED(data);
    UNUSED(size);
    return HAL_OK;



}

HAL_StatusTypeDef CubeCAN_Send_Custom(CubeCAN_Handle *const handle, const GRCAN_CUSTOM_ID custom_id, const void *const data, const uint8_t size){
    UNUSED(handle);
    UNUSED(custom_id);
    UNUSED(data);
    UNUSED(size);
    return HAL_OK;

}
