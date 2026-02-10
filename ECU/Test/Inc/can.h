#ifndef CAN_H
#define CAN_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_gpio.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint32_t CREL;    /*!< FDCAN Core Release register, Address offset: 0x000 */
	uint32_t ENDN;    /*!< FDCAN Endian register, Address offset: 0x004 */
	uint32_t RESERVED1;    /*!< Reserved, 0x008 */
	uint32_t DBTP;    /*!< FDCAN Data Bit Timing & Prescaler register,
				  Address offset: 0x00C */
	uint32_t TEST;    /*!< FDCAN Test register, Address offset: 0x010 */
	uint32_t RWD;     /*!< FDCAN RAM Watchdog register, Address offset: 0x014 */
	uint32_t CCCR;    /*!< FDCAN CC Control register, Address offset: 0x018 */
	uint32_t NBTP;    /*!< FDCAN Nominal Bit Timing & Prescaler register,
				  Address offset: 0x01C */
	uint32_t TSCC;    /*!< FDCAN Timestamp Counter Configuration register,
				  Address offset: 0x020 */
	uint32_t TSCV;    /*!< FDCAN Timestamp Counter Value register, Address
				  offset: 0x024 */
	uint32_t TOCC;    /*!< FDCAN Timeout Counter Configuration register,
				  Address offset: 0x028 */
	uint32_t TOCV;    /*!< FDCAN Timeout Counter Value register, Address
				  offset: 0x02C */
	uint32_t RESERVED2[4]; /*!< Reserved, 0x030 - 0x03C */
	uint32_t ECR;     /*!< FDCAN Error Counter register, Address offset: 0x040 */
	uint32_t PSR;     /*!< FDCAN Protocol Status register, Address offset: 0x044 */
	uint32_t TDCR;    /*!< FDCAN Transmitter Delay Compensation register,
				  Address offset: 0x048 */
	uint32_t RESERVED3;    /*!< Reserved, 0x04C */
	uint32_t IR;      /*!< FDCAN Interrupt register, Address offset: 0x050 */
	uint32_t IE;      /*!< FDCAN Interrupt Enable register, Address offset: 0x054 */
	uint32_t ILS;     /*!< FDCAN Interrupt Line Select register, Address
				  offset: 0x058 */
	uint32_t ILE;     /*!< FDCAN Interrupt Line Enable register, Address
				  offset: 0x05C */
	uint32_t RESERVED4[8]; /*!< Reserved, 0x060 - 0x07C */
	uint32_t RXGFC;   /*!< FDCAN Global Filter Configuration register,
				  Address offset: 0x080 */
	uint32_t XIDAM;   /*!< FDCAN Extended ID AND Mask register,   Address
				  offset: 0x084 */
	uint32_t HPMS;    /*!< FDCAN High Priority Message Status register,
				  Address offset: 0x088 */
	uint32_t RESERVED5;    /*!< Reserved,    0x08C */
	uint32_t RXF0S;   /*!< FDCAN Rx FIFO 0 Status register,   Address
				  offset: 0x090 */
	uint32_t RXF0A;   /*!< FDCAN Rx FIFO 0 Acknowledge register, Address
				  offset: 0x094 */
	uint32_t RXF1S;   /*!< FDCAN Rx FIFO 1 Status register, Address
				  offset: 0x098 */
	uint32_t RXF1A;   /*!< FDCAN Rx FIFO 1 Acknowledge register, Address
				  offset: 0x09C */
	uint32_t RESERVED6[8]; /*!< Reserved, 0x0A0 - 0x0BC */
	uint32_t TXBC;    /*!< FDCAN Tx Buffer Configuration register, Address
				  offset: 0x0C0 */
	uint32_t TXFQS;   /*!< FDCAN Tx FIFO/Queue Status register, Address
				  offset: 0x0C4 */
	uint32_t TXBRP;   /*!< FDCAN Tx Buffer Request Pending register,
				  Address offset: 0x0C8 */
	uint32_t TXBAR;   /*!< FDCAN Tx Buffer Add Request register, Address
				  offset: 0x0CC */
	uint32_t TXBCR;   /*!< FDCAN Tx Buffer Cancellation Request register,
				  Address offset: 0x0D0 */
	uint32_t TXBTO;   /*!< FDCAN Tx Buffer Transmission Occurred
				  register, Address offset: 0x0D4 */
	uint32_t TXBCF;   /*!< FDCAN Tx Buffer Cancellation Finished
				  register, Address offset: 0x0D8 */
	uint32_t TXBTIE;  /*!< FDCAN Tx Buffer Transmission Interrupt Enable
				  register,          Address offset: 0x0DC */
	uint32_t TXBCIE;  /*!< FDCAN Tx Buffer Cancellation Finished Interrupt Enable
				  register, Address offset: 0x0E0 */
	uint32_t TXEFS;   /*!< FDCAN Tx Event FIFO Status register, Address
				  offset: 0x0E4 */
	uint32_t TXEFA;   /*!< FDCAN Tx Event FIFO Acknowledge register,
				  Address offset: 0x0E8 */
} FDCAN_GlobalTypeDef_e;

typedef enum { DISABLE = 0, ENABLE = !DISABLE } FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

/**
 * @brief FDCAN Init structure definition
 */
typedef struct {
	uint32_t ClockDivider; /*!< Specifies the FDCAN kernel clock divider.
				    The clock is common to all FDCAN instances.
				    This parameter is applied only at initialisation
				  of first FDCAN instance. This parameter can be a
				  value of @ref FDCAN_clock_divider.   */

	uint32_t FrameFormat; /*!< Specifies the FDCAN frame format.
				   This parameter can be a value of @ref
				 FDCAN_frame_format     */

	uint32_t Mode; /*!< Specifies the FDCAN mode.
			    This parameter can be a value of @ref
			  FDCAN_operating_mode   */

	FunctionalState AutoRetransmission; /*!< Enable or disable the automatic
					       retransmission mode. This parameter can be
					       set to ENABLE or DISABLE               */

	FunctionalState TransmitPause; /*!< Enable or disable the Transmit Pause
					  feature. This parameter can be set to
					  ENABLE or DISABLE               */

	FunctionalState ProtocolException; /*!< Enable or disable the Protocol
					      Exception Handling. This parameter
					      can be set to ENABLE or DISABLE */

	uint32_t NominalPrescaler; /*!< Specifies the value by which the oscillator
				      frequency is divided for generating the nominal
				      bit time quanta. This parameter must be a
				      number between 1 and 512            */

	uint32_t NominalSyncJumpWidth; /*!< Specifies the maximum number of time
					  quanta the FDCAN hardware is allowed to
					  lengthen or shorten a bit to perform
					    resynchronization.
					    This parameter must be a number between 1
					  and 128            */

	uint32_t NominalTimeSeg1; /*!< Specifies the number of time quanta in
				     Bit Segment 1. This parameter must be a
				     number between 2 and 256            */

	uint32_t NominalTimeSeg2; /*!< Specifies the number of time quanta in
				     Bit Segment 2. This parameter must be a
				     number between 2 and 128            */

	uint32_t DataPrescaler; /*!< Specifies the value by which the oscillator
				   frequency is divided for generating the data
				   bit time quanta. This parameter must be a
				   number between 1 and 32             */

	uint32_t DataSyncJumpWidth; /*!< Specifies the maximum number of time
				       quanta the FDCAN hardware is allowed to
				       lengthen or shorten a data bit to perform
				       resynchronization. This parameter must be
				       a number between 1 and 16             */

	uint32_t DataTimeSeg1; /*!< Specifies the number of time quanta in Data
				  Bit Segment 1. This parameter must be a number
				  between 1 and 32             */

	uint32_t DataTimeSeg2; /*!< Specifies the number of time quanta in Data
				  Bit Segment 2. This parameter must be a number
				  between 1 and 16             */

	uint32_t StdFiltersNbr; /*!< Specifies the number of standard Message ID
				   filters. This parameter must be a number
				   between 0 and 28             */

	uint32_t ExtFiltersNbr; /*!< Specifies the number of extended Message ID
				   filters. This parameter must be a number
				   between 0 and 8             */

	uint32_t TxFifoQueueMode; /*!< Tx FIFO/Queue Mode selection.
				       This parameter can be a value of @ref
				     FDCAN_txFifoQueue_Mode */

} FDCAN_InitTypeDef_e;


// RX Callback must perform a deep copy of the data
//
typedef void (*CAN_RXCallback)(uint32_t ID, void *data, uint32_t size);
typedef struct {
	// can baud rate is set by fdcan prescaler and RCC clock configurations
	FDCAN_GlobalTypeDef_e *fdcan_instance; // Base address of FDCAN peripheral in memory (FDCAN1, FDCAN2, FDCAN3 macros)

	FDCAN_InitTypeDef_e hal_fdcan_init;
	CAN_RXCallback rx_callback;
	uint32_t rx_interrupt_priority;
	uint32_t tx_interrupt_priority;

	// Circular Buffer
	uint32_t tx_buffer_length;

	GPIO_TypeDef_e *rx_gpio;	       // Instance name, like GPIOA, GPIOB, etc.
	GPIO_InitTypeDef_e init_rx_gpio; // GPIO Parameters - set correct Alternate Function, no pullup/pulldown, high/very_high frequency
	GPIO_TypeDef_e *tx_gpio;
	GPIO_InitTypeDef_e init_tx_gpio;

	// additional parameters
} CANConfig;

typedef struct FDCAN_HandleTypeDef_st FDCAN_HandleTypeDef_e;
typedef struct CircularBuffer_st CircularBuffer_e;
// FDCAN peripheral for STM32G4
typedef struct {
	FDCAN_HandleTypeDef_e *hal_fdcanP;
	CircularBuffer_e *tx_buffer;
	uint32_t tx_buffer_length;

	CAN_RXCallback rx_callback;

	// for release
	GPIO_TypeDef_e *rx_gpio;
	GPIO_TypeDef_e *tx_gpio;
	uint32_t Clock_Source;

	// state
	bool init;
	bool started;

	// error states
} CANHandle;

#define FDCAN_MAX_DATA_BYTES 64


/**
 * @brief  FDCAN Tx header structure definition
 */
typedef struct {
	uint32_t Identifier; /*!< Specifies the identifier.
				  This parameter must be a number between:
				   - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
				   - 0 and 0x1FFFFFFF, if IdType is
				FDCAN_EXTENDED_ID               */

	uint32_t IdType; /*!< Specifies the identifier type for the message that
			    will be transmitted. This parameter can be a value
			    of @ref FDCAN_id_type               */

	uint32_t TxFrameType; /*!< Specifies the frame type of the message that
				 will be transmitted. This parameter can be a
				 value of @ref FDCAN_frame_type            */

	uint32_t DataLength; /*!< Specifies the length of the frame that will be
				transmitted. This parameter can be a value of
				@ref FDCAN_data_length_code     */

	uint32_t ErrorStateIndicator; /*!< Specifies the error state indicator.
					   This parameter can be a value of @ref
					 FDCAN_error_state_indicator */

	uint32_t BitRateSwitch; /*!< Specifies whether the Tx frame will be
				   transmitted with or without bit rate
				   switching. This parameter can be a value of
				   @ref FDCAN_bit_rate_switching    */

	uint32_t FDFormat; /*!< Specifies whether the Tx frame will be
			      transmitted in classic or FD format. This
			      parameter can be a value of @ref FDCAN_format */

	uint32_t TxEventFifoControl; /*!< Specifies the event FIFO control.
					  This parameter can be a value of @ref
					FDCAN_EFC                   */

	uint32_t MessageMarker; /*!< Specifies the message marker to be copied
				   into Tx Event FIFO element for identification
				   of Tx message status. This parameter must be
				   a number between 0 and 0xFF                */

} FDCAN_TxHeaderTypeDef_e;

typedef struct {
	FDCAN_TxHeaderTypeDef_e tx_header;
	uint8_t data[FDCAN_MAX_DATA_BYTES];
} FDCANTxMessage;

#define FDCAN_STANDARD_ID 0
#define FDCAN_DATA_FRAME 0
#define FDCAN_ESI_ACTIVE 0
#define FDCAN_BRS_OFF 0
#define FDCAN_NO_TX_EVENTS 0

/**
 * @brief  FDCAN Rx header structure definition
 */
typedef struct {
	uint32_t Identifier; /*!< Specifies the identifier.
				  This parameter must be a number between:
				   - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
				   - 0 and 0x1FFFFFFF, if IdType is
				FDCAN_EXTENDED_ID               */

	uint32_t IdType; /*!< Specifies the identifier type of the received message.
			      This parameter can be a value of @ref FDCAN_id_type */

	uint32_t RxFrameType; /*!< Specifies the the received message frame
				 type. This parameter can be a value of @ref
				 FDCAN_frame_type            */

	uint32_t DataLength; /*!< Specifies the received frame length.
				   This parameter can be a value of @ref
				FDCAN_data_length_code     */

	uint32_t ErrorStateIndicator; /*!< Specifies the error state indicator.
					   This parameter can be a value of @ref
					 FDCAN_error_state_indicator */

	uint32_t BitRateSwitch; /*!< Specifies whether the Rx frame is received with
				   or without bit rate switching. This parameter can
				   be a value of @ref FDCAN_bit_rate_switching    */

	uint32_t FDFormat; /*!< Specifies whether the Rx frame is received in
			      classic or FD format. This parameter can be a
			      value of @ref FDCAN_format                */

	uint32_t RxTimestamp; /*!< Specifies the timestamp counter value captured on
				 start of frame reception. This parameter must be a
				 number between 0 and 0xFFFF              */

	uint32_t FilterIndex; /*!< Specifies the index of matching Rx acceptance
				 filter element. This parameter must be a number
				 between:
				    - 0 and (SRAMCAN_FLS_NBR-1), if IdType is
				 FDCAN_STANDARD_ID
				    - 0 and (SRAMCAN_FLE_NBR-1), if IdType is
				 FDCAN_EXTENDED_ID When the frame is a Non-Filter
				 matching frame, this parameter is unused. */

	uint32_t IsFilterMatchingFrame; /*!< Specifies whether the accepted frame did
					   not match any Rx filter. Acceptance of
					   non-matching frames may be enabled via
					     HAL_FDCAN_ConfigGlobalFilter().
					     This parameter takes 0 if the frame
					   matched an Rx filter or 1 if it did not
					   match any Rx filter */

} FDCAN_RxHeaderTypeDef_e;

typedef struct {
	FDCAN_RxHeaderTypeDef_e rx_header;
	uint8_t data[FDCAN_MAX_DATA_BYTES];
} FDCANRxMessage;

/**
 * @brief  FDCAN filter structure definition
 */
typedef struct {
	uint32_t IdType; /*!< Specifies the identifier type.
			      This parameter can be a value of @ref FDCAN_id_type */

	uint32_t FilterIndex; /*!< Specifies the filter which will be initialized.
				   This parameter must be a number between:
				    - 0 and (SRAMCAN_FLS_NBR-1), if IdType is
				 FDCAN_STANDARD_ID
				    - 0 and (SRAMCAN_FLE_NBR-1), if IdType is
				 FDCAN_EXTENDED_ID */

	uint32_t FilterType; /*!< Specifies the filter type.
				  This parameter can be a value of @ref
				FDCAN_filter_type. The value
				FDCAN_FILTER_RANGE_NO_EIDM is permitted only
				when IdType is FDCAN_EXTENDED_ID. */

	uint32_t FilterConfig; /*!< Specifies the filter configuration.
				    This parameter can be a value of @ref
				  FDCAN_filter_config */

	uint32_t FilterID1; /*!< Specifies the filter identification 1.
				 This parameter must be a number between:
				  - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
				  - 0 and 0x1FFFFFFF, if IdType is
			       FDCAN_EXTENDED_ID       */

	uint32_t FilterID2; /*!< Specifies the filter identification 2.
				 This parameter must be a number between:
				  - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
				  - 0 and 0x1FFFFFFF, if IdType is
			       FDCAN_EXTENDED_ID       */

} FDCAN_FilterTypeDef_e;


CANHandle *can_init(const CANConfig *config); // user must supply an rx callback function
int can_start(CANHandle *handle);
int can_stop(CANHandle *handle);
int can_send(CANHandle *handle, FDCANTxMessage *buffer);
int can_release(CANHandle *handle); // deinit circular buffer and turn off can peripheral and gpios
int can_add_filter(CANHandle *handle, FDCAN_FilterTypeDef_e *filter);
// alternatively use
// HAL_FDCAN_ConfigGlobalFilter() //important to accept nonmatching frames into
// HAL_FDCAN_ConfigFilter()

// doesn't need a handle, CAN cores share peripheral clock
void can_set_clksource(uint32_t clksource); // ex. LL_RCC_FDCAN_CLKSOURCE_PCLK1 for STM32G474RE

#endif
