#include "circularBuffer.h"

#ifndef SPI_H
#define SPI_H

// ============================= Handler =============================

// Generic type
typedef struct {
    //LL structs
    LL_SPI_InitTypeDef* config;
    SPI_TypeDef* spi;
    //GR structs
    CircularBuffer* buffer;
} SPI_Handler;

// Generic type
typedef struct {

} SPI_Message;

// ============================= Enums =============================

typedef enum {
    MASTER,
    SLAVE
} GR_SPI_Mode;

typedef enum {
    DATAWIDTH4,
    DATAWIDTH5,
    DATAWIDTH6,
    DATAWIDTH7,
    DATAWIDTH8,
    DATAWIDTH9,
    DATAWIDTH10,
    DATAWIDTH11,
    DATAWIDTH12,
    DATAWIDTH13,
    DATAWIDTH14,
    DATAWIDTH15,
    DATAWIDTH16
} GR_SPI_Data_Width;

typedef enum {
    FULL_DUPLEX,
    SIMPLEX_RX,
    HALF_DUPLEX_RX,
    HALF_DUPLEX_TX
} GR_SPI_Transfer_Direction;

typedef enum {
    HIGH,
    LOW
} GR_SPI_Clock_Polarity;

typedef enum {
    EDGE1,
    EDGE2
} GR_SPI_Clock_Phase;

typedef enum {
    SOFT,
    HARD_INPUT,
    HARD_OUTPUT
} GR_SPI_NSS;

typedef enum {
    PSC_DIV2,
    PSC_DIV4,
    PSC_DIV8,
    PSC_DIV16,
    PSC_DIV32,
    PSC_DIV64,
    PSC_DIV128,
    PSC_DIV256
} GR_SPI_Baud_Rate_Prescalar;

typedef enum {
    LSB_FIRST,
    MSB_FIRST
} GR_SPI_Bit_Order;

typedef enum {
    CRCCalculationEnable,
    CRCCalculationDisable
} GR_SPI_CRC_Calculation;

// ============================= Getter/Setter Functions =============================

/**
 * @brief Sets the SPI transfer direction
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_Transfer_Direction(SPI_Handler* handle, GR_SPI_Transfer_Direction transfer_direction);

/**
 * @brief Sets the SPI mode
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_Mode(SPI_Handler* handle, GR_SPI_Mode mode);

/**
 * @brief Sets the SPI data width
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_Data_Width(SPI_Handler* handle, GR_SPI_Data_Width data_width);

/**
 * @brief Sets the SPI clock polarity
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_Clock_Polarity(SPI_Handler* handle, GR_SPI_Clock_Polarity clock_polarity);

/**
 * @brief Sets the SPI clock phase
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_Clock_Phase(SPI_Handler* handle, GR_SPI_Clock_Phase clock_phase);

/**
 * @brief Sets the SPI NSS
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_NSS(SPI_Handler* handle, GR_SPI_NSS nss);

/**
 * @brief Sets the SPI baud rate
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_Baud_Rate_Prescalar(SPI_Handler* handle, GR_SPI_Baud_Rate_Prescalar baud_rate_prescalar);

/**
 * @brief Sets the SPI bit order
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_Bit_Order(SPI_Handler* handle, GR_SPI_Bit_Order bit_order);

/**
 * @brief Sets the SPI CRC calculation
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_CRC_Calculation(SPI_Handler* handle, GR_SPI_CRC_Calculation crc_calculation);

/**
 * @brief Sets the SPI crc poly. CRCPoly must be between 0x0000 and 0xFFFF.
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Set_CRC_Poly(SPI_Handler* handle, uint32_t crc_poly);

// ============================= Initialization =============================

/**
 * @brief Initializes SPI config to default values
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Setup_Config(SPI_Handler* handle) { LL_SPI_StructInit(handle->config); }

/**
 * @brief Initializes SPI with config values
 * 
 * @param handle
 * @return 
 */
void GR_SPI_Initialize(SPI_Handler* handle) { LL_SPI_Init(handle->spi, handle->config); }

/**
 * @brief Enable SPI
 * 
 * @param config 
 * @return void 
 */
void GR_SPI_Enable(SPI_Handler* handle);

// ============================= Tx/Rx =============================

/**
 * @brief Send data through SPI
 * 
 * @param handler
 * @param data
 */
void GR_SPI_Send(SPI_Handler* handler, SPI_Message data);

/**
 * @brief Read from the SPI buffer; primarily intended for polling. Rely on the passed in handler when setup() is called 
 * 
 * @param handler
 * @return SPI_Message 
 */
SPI_Message GR_SPI_Receive(SPI_Handler* handler);

/**
 * @brief Shutdown the SPI handler
 * 
 * @param handler
 */
void GR_SPI_Close(SPI_Handler* handler);

#endif