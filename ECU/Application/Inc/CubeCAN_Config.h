#ifndef CUBE_CAN_CONFIG_H
#define CUBE_CAN_CONFIG_H

#define CUBEMX_CAN_TX_QUEUE_SIZE 16U
#define CUBEMX_CAN_MAX_INSTANCES 3U
#define ECU_CAN_MAX_DATA_BYTES 64 // Replacing FDCAN_MAX_DATA_BYTES macro since can.h doesnt exist anymore

#endif
