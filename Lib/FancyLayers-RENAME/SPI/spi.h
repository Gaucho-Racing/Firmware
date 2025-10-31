#ifndef SPI_H
#define SPI_H

// Generic type
typedef struct {

} SPI_Handler;

// Generic type
typedef struct {

} SPI_Config;

// Generic type
typedef struct {

} SPI_Message;


/**
 * @brief Initialize the SPI handler
 * 
 * @param config 
 * @return SPI_Handler 
 */
SPI_Handler GR_SPI_Setup(SPI_Config config);

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