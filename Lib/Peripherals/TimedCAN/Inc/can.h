#ifndef CAN_H
#define CAN_H

// Supported STM32 Families
#ifdef STM32G4
#elif defined(STM32L4)
#elif defined(STM32U5)
#error "Unsupported STM32 Family"
#endif

#include "can_cfg.h"
#include "can_platform_deps.h"
// #include "StateData.h"

// #include "circularBuffer.h"
#include <stdbool.h>

// RX Callback must perform a deep copy of the data
//
typedef void (*CAN_RXCallback)(uint32_t ID, void *data, uint32_t size);
typedef struct {
	// can baud rate is set by fdcan prescaler and RCC clock configurations
	FDCAN_GlobalTypeDef *fdcan_instance; // Base address of FDCAN peripheral in memory (FDCAN1, FDCAN2, FDCAN3 macros)

	FDCAN_InitTypeDef hal_fdcan_init;
	CAN_RXCallback rx_callback;
	uint32_t rx_interrupt_priority;
	uint32_t tx_interrupt_priority;

	// Circular Buffer
	// uint32_t tx_buffer_capacity;

	GPIO_TypeDef *rx_gpio;	       // Instance name, like GPIOA, GPIOB, etc.
	GPIO_InitTypeDef init_rx_gpio; // GPIO Parameters - set correct Alternate Function, no pullup/pulldown, high/very_high frequency
	GPIO_TypeDef *tx_gpio;
	GPIO_InitTypeDef init_tx_gpio;

	// additional parameters
} CANConfig;

#define FDCAN_MAX_DATA_BYTES 64
// TODO - allow user to send data without needing to construct a header for the buffer
//  TODO: G4 tests are dependent on the System clock configuration??
typedef struct {
	FDCAN_TxHeaderTypeDef tx_header;
	uint8_t data[FDCAN_MAX_DATA_BYTES];
} FDCANTxMessage;
typedef struct {
	FDCAN_RxHeaderTypeDef rx_header;
	uint8_t data[FDCAN_MAX_DATA_BYTES];
} FDCANRxMessage;

// FDCAN peripheral for STM32G4
typedef struct {
	FDCAN_HandleTypeDef *hal_fdcanP;

	// TX buffer
	FDCANTxMessage *const tx_buffer;
	volatile uint32_t tx_capacity;
	volatile uint32_t tx_tail;
	volatile uint32_t tx_elements;

	// RX Callback
	CAN_RXCallback rx_callback;

	uint8_t rx_interrupt_priority; // only 4 bits
	uint8_t tx_interrupt_priority;

	// for release
	GPIO_TypeDef *rx_gpio;
	uint32_t rx_pin;
	GPIO_TypeDef *tx_gpio;
	uint32_t tx_pin;
	// uint32_t Clock_Source;

	// state
	bool init;
	bool started;

	// error states
	uint32_t lost_rx;
} CANHandle;

typedef enum {
	CAN_SUCCESS = 0,
	CAN_ERROR
} CAN_STATUS;

CANHandle *can_init(const CANConfig *config); // user must supply an rx callback function
CAN_STATUS can_start(CANHandle *handle);
CAN_STATUS can_stop(CANHandle *handle);
CAN_STATUS can_send(CANHandle *handle, FDCANTxMessage *buffer);
CAN_STATUS can_release(CANHandle *handle); // deinit circular buffer and turn off can peripheral and gpios
CAN_STATUS can_add_filter(CANHandle *handle, FDCAN_FilterTypeDef *filter);
CAN_STATUS can_enqueue(CANHandle *handle, FDCANTxMessage *message); // adds to software buffer, returns error if full

// pass in a buffer to store the status string
// int can_info(char* );

// alternatively use
// HAL_FDCAN_ConfigGlobalFilter() //important to accept nonmatching frames into
// HAL_FDCAN_ConfigFilter()

#define GPIOx_CLK_ENABLE(GPIOX)                                                                                                                                                                        \
	do {                                                                                                                                                                                           \
		if (GPIOX == GPIOA)                                                                                                                                                                    \
			__HAL_RCC_GPIOA_CLK_ENABLE();                                                                                                                                                  \
		else if (GPIOX == GPIOB)                                                                                                                                                               \
			__HAL_RCC_GPIOB_CLK_ENABLE();                                                                                                                                                  \
		else if (GPIOX == GPIOD)                                                                                                                                                               \
			__HAL_RCC_GPIOD_CLK_ENABLE();                                                                                                                                                  \
		else                                                                                                                                                                                   \
			LOGOMATIC("BAD FDCAN GPIO Port");                                                                                                                                              \
	} while (0)

#define GPIOx_CLK_DISABLE(GPIOX)                                                                                                                                                                       \
	do {                                                                                                                                                                                           \
		if (GPIOX == GPIOA)                                                                                                                                                                    \
			__HAL_RCC_GPIOA_CLK_DISABLE();                                                                                                                                                 \
		else if (GPIOX == GPIOB)                                                                                                                                                               \
			__HAL_RCC_GPIOB_CLK_DISABLE();                                                                                                                                                 \
		else if (GPIOX == GPIOD)                                                                                                                                                               \
			__HAL_RCC_GPIOD_CLK_DISABLE();                                                                                                                                                 \
		else                                                                                                                                                                                   \
			LOGOMATIC("BAD FDCAN GPIO Port");                                                                                                                                              \
	} while (0)

// doesn't need a handle, CAN cores share peripheral clock
void can_set_clksource(uint32_t clksource); // ex. LL_RCC_FDCAN_CLKSOURCE_PCLK1 for STM32G474RE

// default Configuration helpers
int get_cfg(FDCAN_GlobalTypeDef *instance, CAN_RXCallback callback, CANConfig *out_cfg, uint32_t FDCAN_Mode, uint32_t numStdFilters, uint32_t numExtFilters);

// converts CAN FD TxHeader DataLength Field
static const uint8_t CANFD_DLCtoBytes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

// NEW THINGS

void CAN_Timer_Start(void);
void can_tx_dequeue_helper(CANHandle *handle);

#endif
