#include "can.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Logomatic.h"

// HAL handles
// #ifdef USECAN1
static FDCAN_HandleTypeDef hal_fdcan1 = {.Instance = FDCAN1};
static CANHandle CAN1 = {.hal_fdcanP = &hal_fdcan1};
// #endif

// #ifdef USECAN2
static FDCAN_HandleTypeDef hal_fdcan2 = {.Instance = FDCAN2};
static CANHandle CAN2 = {.hal_fdcanP = &hal_fdcan2};
// #endif

// #ifdef USECAN3
static FDCAN_HandleTypeDef hal_fdcan3 = {.Instance = FDCAN3};
static CANHandle CAN3 = {.hal_fdcanP = &hal_fdcan3};
// #endif

// macro lore
/*
#define CAT(a,b) a##b
#define CAT3(a, b, c) a##b##c
#define CAT4(a, b,c,d) a##b##c##d
#define CAT5(a, b,c,d,e) a##b##c##d##e

#define ACTIVATE_FDCAN_HELPER(FDCANX, ITY, preirq, subirq)  \
	do { \
	    HAL_NVIC_SetPriority( CAT4(FDCANX##,_,ITY, _IRQn ) , preirq, subirq );  \
	    HAL_NVIC_EnableIRQ( CAT4(FDCANX##,_,ITY, _IRQn ) ); \
	} while(0)

#define HAL_NVIC_ACTIVATE_FDCAN(FDCANX, ITY, preirq, subirq) \
    do { \
	if (FDCANX == ##FDCAN1 && ITY == 0)      { ACTIVATE_FDCAN_HELPER(FDCAN1, IT0, preirq, subirq); } \
	else if (FDCANX == FDCAN1 && ITY == 1) { ACTIVATE_FDCAN_HELPER(FDCAN1, IT1, preirq, subirq); } \
	else if (FDCANX == FDCAN2 && ITY == 0) { ACTIVATE_FDCAN_HELPER(FDCAN2, IT0, preirq, subirq); } \
	else if (FDCANX == FDCAN2 && ITY == 1) { ACTIVATE_FDCAN_HELPER(FDCAN2, IT1, preirq, subirq); } \
	else if (FDCANX == FDCAN3 && ITY == 0) { ACTIVATE_FDCAN_HELPER(FDCAN3, IT0, preirq, subirq); } \
	else if (FDCANX == FDCAN3 && ITY == 1) { ACTIVATE_FDCAN_HELPER(FDCAN3, IT1, preirq, subirq); } \
	else { LOGOMATIC("Unrecognized FDCAN and Interrupt Line combination"); } \
    } while(0)

*/

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

static int fdcan_shared_clock_ref = 0;
static inline void fdcan_enable_shared_clock(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	if (fdcan_shared_clock_ref == 0) {
		__HAL_RCC_FDCAN_CLK_ENABLE();
	}
	fdcan_shared_clock_ref++;

	__set_PRIMASK(primask);
}

static inline void fdcan_disable_shared_clock(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	if (fdcan_shared_clock_ref > 0) {
		fdcan_shared_clock_ref--;
		if (fdcan_shared_clock_ref == 0) {
			__HAL_RCC_FDCAN_CLK_DISABLE();
		}
	}
	__set_PRIMASK(primask);
}

static inline void gpio_clk_enable(GPIO_TypeDef *gpio)
{
	if (gpio == GPIOA) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
	} else if (gpio == GPIOB) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
	} else if (gpio == GPIOD) {
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}
}

static inline void gpio_clk_disable(GPIO_TypeDef *gpio)
{
	if (gpio == GPIOA) {
		__HAL_RCC_GPIOA_CLK_DISABLE();
	} else if (gpio == GPIOB) {
		__HAL_RCC_GPIOB_CLK_DISABLE();
	} else if (gpio == GPIOD) {
		__HAL_RCC_GPIOD_CLK_DISABLE();
	}
}

static int can_get_irqs(FDCAN_GlobalTypeDef *instance, IRQn_Type *it0, IRQn_Type *it1);

static int can_msp_init(CANHandle *handle, CANConfig *config);
CANHandle *can_init(const CANConfig *config)
{
	// config validation?
	// assert(config != 0)

	// #ifdef STM32G474xx
	CANHandle *canHandle = 0;
	// #ifdef USECAN1
	if (config->fdcan_instance == FDCAN1) {
		if (CAN1.init) {
			LOGOMATIC("CAN: CAN1 is already initialized\n");
			return 0;
		} else {
			canHandle = &CAN1;
		}
	}
	// #endif
	// #ifdef USECAN2
	else if (config->fdcan_instance == FDCAN2) {
		if (CAN2.init) {
			LOGOMATIC("CAN: CAN2 is already initialized\n");
			return 0;
		} else {
			canHandle = &CAN2;
		}
	}
	// #ifdef USECAN3
	else if (config->fdcan_instance == FDCAN3) {
		if (CAN3.init) {
			LOGOMATIC("CAN: CAN3 is already initialized\n");
			return 0;
		} else {
			canHandle = &CAN3;
		}
	}
	// #endif
	// #elif defined(STM32L476xx)
	// #else
	// #error "Unsupported STM32 family"
	// #endif
	else {
		LOGOMATIC("CAN: Unrecognized FDCAN instance");
		return 0;
	}
	canHandle->init = false;
	canHandle->started = false;

	// Initialize handle
	assert(config->hal_fdcan_init.TxFifoQueueMode == FDCAN_TX_FIFO_OPERATION);

	canHandle->hal_fdcanP->Init = config->hal_fdcan_init; // copy FDCAN parameters from user
	// canHandle->hal_fdcanP->Instance = config->fdcan_instance //handles initialized with correct base instance addresses

	canHandle->rx_gpio = config->rx_gpio;
	canHandle->tx_gpio = config->tx_gpio;
	canHandle->rx_callback = config->rx_callback;
	canHandle->tx_buffer_length = config->tx_buffer_length;

	// alternately -> have can_msp_init setup state for HAL_FDCAN_MspInit to work correctly
	// have can_msp_deinit setup state for HAL_FDCAN_MspDeInit to work correctly
	// Then call HAL_FDCAN_Init() and HAL_FDCAN_DeInit()

	// Current idea, redefine HAL_FDCAN_MspInit and MspDeInit do nothing at all, do all the work in can_msp_init()
	if (can_msp_init(canHandle, (CANConfig *)config)) {
		LOGOMATIC("CAN_init: could not initialize MSP resources");
		can_release(canHandle);
	}

	// PROBLEM: HAL_FDCAN_Init expects HAL_FDCAN_MspInit() to be defined
	if (HAL_FDCAN_Init(canHandle->hal_fdcanP) != HAL_OK) {
		LOGOMATIC("CAN: HAL Could not initialize FDCAN peripheral");
		return NULL;
		// Error_Handler();
	}

	// Active FDCAN callbacks - rxcalback uses line0, txcallback uses line1
	// uint32_t rxevents = FDCAN_IT_RX_FIFO0_NEW_MESSAGE;
	uint32_t status = 0;
	uint32_t rx_events = FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO0_FULL;
	status |= HAL_FDCAN_ActivateNotification(canHandle->hal_fdcanP, rx_events, 0);
	status |= HAL_FDCAN_ConfigInterruptLines(canHandle->hal_fdcanP, rx_events, FDCAN_INTERRUPT_LINE0);

	// uint32_t txevents = FDCAN_IT_TX_COMPLETE;
	uint32_t destinations = FDCAN_TX_BUFFER0;
	uint32_t tx_events = FDCAN_IT_TX_COMPLETE | FDCAN_IT_TX_FIFO_EMPTY;
	status |= HAL_FDCAN_ActivateNotification(canHandle->hal_fdcanP, tx_events, destinations);
	status |= HAL_FDCAN_ConfigInterruptLines(canHandle->hal_fdcanP, tx_events, FDCAN_INTERRUPT_LINE1);
	// Callbacks redefined later

	if (status & HAL_ERROR) {
		LOGOMATIC("CAN: Could not activate rx and tx interrupts\n");
		return NULL;
	}

	// Circular Buffer
	canHandle->tx_buffer = GR_CircularBuffer_Create(config->tx_buffer_length);
	if (!canHandle->tx_buffer) {
		LOGOMATIC("CAN: Could not allocate circular buffer\n");
		return 0;
	}

	int *ptr = (int *)malloc(sizeof(int));
	*ptr = 42;
	free(ptr);

	canHandle->init = !*ptr;
	canHandle->started = false;

	return canHandle;
}

inline void can_set_clksource(uint32_t clksource)
{
	LL_RCC_SetFDCANClockSource(clksource);
}

// only valid for #STM32G474x, must redefine for each family
static int can_msp_init(CANHandle *canHandle, CANConfig *config)
{
	// MSP Init ------- This could be inside HAL_FDCAN_MspInit() instead
	// FDCAN Clock Select

	fdcan_enable_shared_clock();
	// Clock speed for FDCAN determined by APB1 clock speed and FDCAN prescaler

	// GPIOs init
	GPIOx_CLK_ENABLE(config->rx_gpio);
	GPIOx_CLK_ENABLE(config->tx_gpio);

	HAL_GPIO_Init(config->rx_gpio, &(config->init_rx_gpio));
	HAL_GPIO_Init(config->tx_gpio, &(config->init_tx_gpio));

	IRQn_Type rxit = -1;
	IRQn_Type txit = -1;
	can_get_irqs(canHandle->hal_fdcanP->Instance, &rxit, &txit);

	// rxfifo0
	HAL_NVIC_SetPriority(rxit, config->rx_interrupt_priority, 0);
	HAL_NVIC_EnableIRQ(rxit);

	// tx
	HAL_NVIC_SetPriority(txit, config->tx_interrupt_priority, 0);
	HAL_NVIC_EnableIRQ(txit);
	// End MSP Init --------------

	return 0;
}

// valid only for STM32G4
static int can_get_irqs(FDCAN_GlobalTypeDef *instance, IRQn_Type *it0, IRQn_Type *it1)
{
	if (instance == FDCAN1) {
		*it0 = FDCAN1_IT0_IRQn;
		*it1 = FDCAN1_IT1_IRQn;
		return 0;
	}
	if (instance == FDCAN2) {
		*it0 = FDCAN2_IT0_IRQn;
		*it1 = FDCAN2_IT1_IRQn;
		return 0;
	}
	if (instance == FDCAN3) {
		*it0 = FDCAN3_IT0_IRQn;
		*it1 = FDCAN3_IT1_IRQn;
		return 0;
	}

	return -1; // invalid instance
}

// valid only for STM32G4
// static const char *can_get_instance_name(FDCAN_GlobalTypeDef *instance)
// {
// 	if (instance == FDCAN1) {
// 		return "FDCAN1";
// 	} else if (instance == FDCAN2) {
// 		return "FDCAN2";
// 	} else if (instance == FDCAN3) {
// 		return "FDCAN3";
// 	}
//  return "UNKNOWN";
// }

// valid only for STM32G4
static CANHandle *can_get_buffer_handle(FDCAN_HandleTypeDef *hfdcan)
{
	// #ifdef STM32G474xx
	if (hfdcan->Instance == FDCAN1) {
		return &CAN1;
	} else if (hfdcan->Instance == FDCAN2) {
		return &CAN2;
	} else if (hfdcan->Instance == FDCAN3) {
		return &CAN3;
	} else {
		LOGOMATIC("CAN_get_buffer_handle: was given invalid FDCAN instance\n");
		return 0;
	}
}

static void can_tx_buffer_helper(CANHandle *handle)
{
	while (HAL_FDCAN_GetTxFifoFreeLevel(handle->hal_fdcanP) && !GR_CircularBuffer_IsEmpty(handle->tx_buffer)) {
		FDCANTxMessage *msg = GR_CircularBuffer_Pop(handle->tx_buffer);

		if (!msg) {
			break;
		}

		HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(handle->hal_fdcanP, &msg->tx_header, msg->data);

		if (status != HAL_OK) {
			LOGOMATIC("CAN_tx_helper: failed to add message to FIFO\n");
			free(msg); // Free the message we couldn't send
			break;	   // Stop trying to send more
		}

		free(msg); // Successfully sent, free the memory
	}
}

void FDCAN1_IT0_IRQHandler(void)
{
	HAL_FDCAN_IRQHandler(&hal_fdcan1);
}
void FDCAN1_IT1_IRQHandler(void)
{
	HAL_FDCAN_IRQHandler(&hal_fdcan1);
}

void FDCAN2_IT0_IRQHandler(void)
{
	HAL_FDCAN_IRQHandler(&hal_fdcan2);
}
void FDCAN2_IT1_IRQHandler(void)
{
	HAL_FDCAN_IRQHandler(&hal_fdcan2);
}

void FDCAN3_IT0_IRQHandler(void)
{
	HAL_FDCAN_IRQHandler(&hal_fdcan3);
}
void FDCAN3_IT1_IRQHandler(void)
{
	HAL_FDCAN_IRQHandler(&hal_fdcan3);
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
	UNUSED(BufferIndexes);
	// If circular buffer has elements, send to queue
	// Otherwise do nothing
	// #ifdef USECAN1
	CANHandle *handle = can_get_buffer_handle(hfdcan);

	if (!handle || !handle->tx_buffer) {
		return;
	}

	if (GR_CircularBuffer_IsEmpty(handle->tx_buffer)) {
		return;
	}

	// see if you can pop any more from the buffer
	can_tx_buffer_helper(handle);
}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	CANHandle *handle = can_get_buffer_handle(hfdcan);

	if (!handle || !handle->init || !handle->rx_callback) {
		return;
	}

	/*if (!handle->rx_buffer) {
	    LOGOMATIC("CAN: RX Complete, but %s Buffer was released\n", can_get_instance_name(hfdcan->Instance));
	    return;
	} */ // no rx buffer at the moment

	/*if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) {
	    lost_rx++;
	}*/

	if (!(RxFifo0ITs & ~FDCAN_IT_RX_FIFO0_MESSAGE_LOST)) {
		return;
	}

	// if (GR_CircularBuffer_IsFull(handle->rx_buffer)) return;
	FDCAN_RxHeaderTypeDef rx_header;

	// TODO: Stack allocation may be unsafe
	uint8_t rx_data[64] = {0};

	// TODO: maybe also use a timer for this?
	while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0) {
		HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data);

		// stack allocation should be fine? Callback needs to terminate first before stack is popped
		// should switch this over to malloc at some point to avoid double copies?
		// GR_OLD_NODE_ID sendingID = (rx_header.Identifier & (0xFF << 20)) >> 20;
		// GR_OLD_MSG_ID messageID = (rx_header.Identifier & (0xFFF << 8)) >> 8;
		handle->rx_callback(rx_header.Identifier, rx_data, rx_header.DataLength);
	}

	/* whoopsie, don't need the rx buffer yet
	while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) & !GR_CircularBuffer_IsFull(handle->rx_buffer)) {
	    FDCAN_RxHeaderTypeDef rx_header;
	    uint8_t rx_data[64] = {0};
	    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, &rx_data);

	    if (GR_CircularBuffer_IsEmpty(handle->rx_buffer)) handle->rx_callback(rx_data, rx_header.DataLength);
	    else {
		GR_CircularBuffer_Push(handle->rx_buffer, rx_data, rx_header.DataLength);
	    }
	}*/
}

/*
void can_read_rx_buffer(CANHandle* canHandle) {
    //User can call this at their leisure to pop the rx_buffer

    //read rx_buffer one element at a time?
    //or read until empty
}*/

/*void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef * hfdcan, uint32_t RxFifo0ITs) {

}*/

// Just alternatively just use the HAL_FDCAN_ConfigFilter directly with the canHandle->hal_fdcan
int can_add_filter(CANHandle *canHandle, FDCAN_FilterTypeDef *filter)
{
	if (!canHandle) {
		LOGOMATIC("CAN_add_filter: handle is null");
		return -1;
	}

	if (!canHandle->init || canHandle->started) {
		LOGOMATIC("CAN_add_filter: can instance is not initialized or already started");
		return -1;
	}

	if (HAL_FDCAN_ConfigFilter(canHandle->hal_fdcanP, filter) != HAL_OK) {
		LOGOMATIC("CAN_add_filter: failed to configure filter");
		return -1;
	}
	return 0;
	// check that # of filters isn't exceeding max value
}

// Need to
int can_start(CANHandle *canHandle)
{
	if (!canHandle || !canHandle->init) {
		return -1;
	}

	GPIOx_CLK_ENABLE(canHandle->rx_gpio);
	GPIOx_CLK_ENABLE(canHandle->tx_gpio);

	HAL_FDCAN_Start(canHandle->hal_fdcanP);
	canHandle->started = true;

	return 0;
}

int can_stop(CANHandle *canHandle)
{
	if (!canHandle || !canHandle->init) {
		return -1;
	}
	if (!canHandle->started) {
		return 0;
	}

	HAL_FDCAN_Stop(canHandle->hal_fdcanP);

	GPIOx_CLK_DISABLE(canHandle->rx_gpio);
	GPIOx_CLK_DISABLE(canHandle->tx_gpio);

	canHandle->started = false;

	return 0;
}

// Valid only for STM32G474xE
/*int can_msp_deinit(CANHandle* canHandle) {
    //MSP DeInit
    //turn off gpio clocks - can only turn off GPIOs if no other instances are using them


    return 0;
}*/

static void FDCAN_InstanceDeInit(FDCAN_HandleTypeDef *hfdcan)
{
	// Enter INIT mode
	hfdcan->Instance->CCCR |= FDCAN_CCCR_INIT;
	while (!(hfdcan->Instance->CCCR & FDCAN_CCCR_INIT))
		;

	// Clear filters
	// TODO: fix magic numbers
	memset((void *)hfdcan->msgRam.StandardFilterSA, 0, 0x0070);
	memset((void *)hfdcan->msgRam.ExtendedFilterSA, 0, 0x0050);

	// Optionally reset FIFOs / buffers

	// Disable interrupts
	__HAL_FDCAN_DISABLE_IT(hfdcan, FDCAN_IT_LIST_RX_FIFO0 | FDCAN_IT_LIST_RX_FIFO1 | FDCAN_IT_LIST_SMSG | FDCAN_IT_LIST_TX_FIFO_ERROR | FDCAN_IT_LIST_MISC | FDCAN_IT_LIST_BIT_LINE_ERROR |
					   FDCAN_IT_LIST_PROTOCOL_ERROR);

	// Exit INIT mode
	hfdcan->Instance->CCCR &= ~FDCAN_CCCR_INIT;
	while (hfdcan->Instance->CCCR & FDCAN_CCCR_INIT)
		;

	// Update handle state
	hfdcan->State = HAL_FDCAN_STATE_RESET;
}

int can_release(CANHandle *canHandle)
{
	if (!canHandle) {
		LOGOMATIC("CAN: Tried to release a null handle");
		return -1;
	}

	if (!canHandle->init) {
		LOGOMATIC("CAN_release: can instance is already deinitialized");
		return -1;
	}
	can_stop(canHandle); // try to prevent more interrupts from firing

	// must disable NVIC IRQs before freeing circular buffer

	// turn off NVIC resources
	IRQn_Type rx0it = -1;
	IRQn_Type txit = -1;
	can_get_irqs(canHandle->hal_fdcanP->Instance, &rx0it, &txit);
	HAL_NVIC_DisableIRQ(rx0it);
	HAL_NVIC_DisableIRQ(txit);

	// need to check if other pins are using before disabling - do this after mvp
	// GPIOx_CLK_DISABLE(canHandle->rx_gpio);
	// GPIOx_CLK_DISABLE(canHandle->tx_gpio);

	// reset FDCANx instance and message RAM and filters, clear interrupts
	// HAL_FDCAN_DeInit(canHandle->hal_fdcanP); resets a little too hard
	FDCAN_InstanceDeInit(canHandle->hal_fdcanP);

	__DSB(); // Data Synchronization Barrier
	__ISB(); // Instruction Synchronization Barrier

	// free circular buffer contents
	GR_CircularBuffer_Free(&(canHandle->tx_buffer));

	// reset can instance
	FDCAN_HandleTypeDef *temp = canHandle->hal_fdcanP;
	memset(canHandle, 0, sizeof(*canHandle));
	canHandle->hal_fdcanP = temp;

	fdcan_disable_shared_clock(); // only turns off clock if no other instances are running.

	return 0;
}

int can_send(CANHandle *canHandle, FDCANTxMessage *message)
{
	if (!canHandle || !message) {
		LOGOMATIC("CAN_send: received null pointer\n");
		return -1;
	}

	if (!canHandle->init || !canHandle->started) {
		LOGOMATIC("CAN_send: CAN not initialized or started\n");
		return -1;
	}

	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	// IF TX Fifos are not full, send directly to them
	// If TX Fifos are full, append to circular buffer
	// If circular buffer is full, return an error code
	uint32_t free = HAL_FDCAN_GetTxFifoFreeLevel(canHandle->hal_fdcanP);

	if (free > 0) {
		HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(canHandle->hal_fdcanP, &(message->tx_header),
									 message->data // Not &message->data if data is array
		);

		__set_PRIMASK(primask);

		if (status != HAL_OK) {
			LOGOMATIC("CAN_send: failed to add to HW FIFO\n");
			return -1;
		}
		return 0;
	}

	// Hardware FIFO full, try software buffer
	if (!GR_CircularBuffer_IsFull(canHandle->tx_buffer)) {
		int result = GR_CircularBuffer_Push(canHandle->tx_buffer, message, sizeof(FDCANTxMessage));

		__set_PRIMASK(primask);

		if (result != 0) {
			LOGOMATIC("CAN_send: buffer push failed\n");
			return -1;
		}
		return 0;
	}

	// Both buffers full
	__set_PRIMASK(primask);
	LOGOMATIC("CAN_send: all buffers full\n");
	return -1;
}
