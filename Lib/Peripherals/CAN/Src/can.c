#include "can.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Logomatic.h"

// CAN CONFIGURATION HEADER
#include "can_cfg.h"
#ifndef CAN_CFG_H
#error "can.c: Please define CAN_CFG_H and define at least one USECANx and TX_BUFFER_X_SIZE"
#endif
// ===============================

// EXAMPLE Configuration in "can_cfg.h"
// #ifndef CAN_CFG_H
// #define CAN_CFG_H

// #define USECAN1
// #define TX_BUFFER_1_SIZE 10

// #endif

// HAL handles
#ifdef USECAN1
#ifndef TX_BUFFER_1_SIZE
#error "Please Define TX_BUFFER_1_SIZE"
#endif
static FDCAN_HandleTypeDef hal_fdcan1 = {.Instance = FDCAN1};
FDCANTxMessage tx_buffer_1[TX_BUFFER_1_SIZE] = {0};
static CANHandle CAN1 = {.hal_fdcanP = &hal_fdcan1, .tx_buffer = tx_buffer_1};
#endif

#ifdef USECAN2
#ifndef TX_BUFFER_2_SIZE
#error "Please Define TX_BUFFER_2_SIZE"
#endif
static FDCAN_HandleTypeDef hal_fdcan2 = {.Instance = FDCAN2};
FDCANTxMessage tx_buffer_2[TX_BUFFER_2_SIZE] = {0};
static CANHandle CAN2 = {.hal_fdcanP = &hal_fdcan2, .tx_buffer = tx_buffer_2};
#endif

#ifdef USECAN3
#ifndef TX_BUFFER_3_SIZE
#error "Please Define TX_BUFFER_3_SIZE"
#endif
static FDCAN_HandleTypeDef hal_fdcan3 = {.Instance = FDCAN3};
FDCANTxMessage tx_buffer_3[TX_BUFFER_3_SIZE] = {0};
static CANHandle CAN3 = {.hal_fdcanP = &hal_fdcan3, .tx_buffer = tx_buffer_3};
#endif

#define MIN(A, B) ((A < B) ? A : B)

// bool hardwareEnabled = false;

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

// TODO: Modify helpers to work across families
// helpers =================
static int fdcan_shared_clock_ref = 0;
static inline void fdcan_enable_shared_clock(void);
static inline void fdcan_disable_shared_clock(void);
static CANHandle *can_get_handle(FDCAN_HandleTypeDef *hfdcan);
static CAN_STATUS can_get_irqs(FDCAN_GlobalTypeDef *instance, IRQn_Type *it0, IRQn_Type *it1);
static CAN_STATUS validate_can_handle(CANHandle *canHandle);

inline void can_set_clksource(uint32_t clksource)
{
	LL_RCC_SetFDCANClockSource(clksource);
}
// static const char *can_get_instance_name(FDCAN_GlobalTypeDef *instance)
// static inline void gpio_clk_enable(GPIO_TypeDef *gpio)
// static inline void gpio_clk_disable(GPIO_TypeDef *gpio)

static CAN_STATUS can_msp_init(CANHandle *canHandle, CANConfig *config);
static CAN_STATUS can_msp_deinit(CANHandle *canHandle);
static void FDCAN_InstanceDeInit(FDCAN_HandleTypeDef *hfdcan);

//================================================= API ========================================
CANHandle *can_init(const CANConfig *config)
{
	// config validation?
	// assert(config != 0)
	CANHandle *canHandle = NULL;

#ifdef STM32G474xx
#ifdef USECAN1
	if (config->fdcan_instance == FDCAN1) {
		if (CAN1.init) {
			LOGOMATIC("CAN: CAN1 is already initialized\n");
			return CAN_SUCCESS;
		} else {
			canHandle = &CAN1;
			canHandle->tx_capacity = TX_BUFFER_1_SIZE;
		}
	}
#endif
#ifdef USECAN2
	if (config->fdcan_instance == FDCAN2) {
		if (CAN2.init) {
			LOGOMATIC("CAN: CAN2 is already initialized\n");
			return CAN_SUCCESS;
		} else {
			canHandle = &CAN2;
			canHandle->tx_capacity = TX_BUFFER_2_SIZE;
			LOGOMATIC("CAN: CAN2 selected with tx capacity %lu\n", canHandle->tx_capacity);
		}
	}
#endif
#ifdef USECAN3
	if (config->fdcan_instance == FDCAN3) {
		if (CAN3.init) {
			LOGOMATIC("CAN: CAN3 is already initialized\n");
			return CAN_SUCCESS;
		} else {
			canHandle = &CAN3;
			canHandle->tx_capacity = TX_BUFFER_3_SIZE;
		}
	}
#endif
#endif

	// #elif defined(STM32L476xx)
	// #else
	// #error "Unsupported STM32 family"
	// #endif

	if (canHandle == NULL) {
		LOGOMATIC("CAN: Unrecognized FDCAN instance");
		return NULL;
	}

	canHandle->init = false;
	canHandle->started = false;

	// Initialize handle
	assert(config->hal_fdcan_init.TxFifoQueueMode == FDCAN_TX_FIFO_OPERATION);

	canHandle->hal_fdcanP->Init = config->hal_fdcan_init; // copy FDCAN parameters from user
	// canHandle->hal_fdcanP->Instance = config->fdcan_instance //handles initialized with correct base instance addresses

	canHandle->rx_gpio = config->rx_gpio;
	canHandle->tx_gpio = config->tx_gpio;
	canHandle->rx_pin = config->init_rx_gpio.Pin;
	canHandle->tx_pin = config->init_tx_gpio.Pin;

	canHandle->rx_interrupt_priority = config->rx_interrupt_priority;
	canHandle->tx_interrupt_priority = config->tx_interrupt_priority;

	canHandle->rx_callback = config->rx_callback;

	// tx buffer
	// canHandle->tx_capacity = TX_BUFFER_SIZE_1; //dependent on can instance
	canHandle->tx_tail = 0;
	canHandle->tx_elements = 0;

	// alternately -> have can_msp_init setup state for HAL_FDCAN_MspInit to work correctly
	// have can_msp_deinit setup state for HAL_FDCAN_MspDeInit to work correctly
	// Then call HAL_FDCAN_Init() and HAL_FDCAN_DeInit()

	// Current idea, redefine HAL_FDCAN_MspInit and MspDeInit to do nothing at all, do all the work in can_msp_init()
	uint32_t failure = 0;
	if (failure |= (can_msp_init(canHandle, (CANConfig *)config) != CAN_SUCCESS)) {
		LOGOMATIC("CAN_init: could not initialize MSP resources");
		return NULL;
	}

	// PROBLEM: HAL_FDCAN_Init expects HAL_FDCAN_MspInit() to be defined
	if (HAL_FDCAN_Init(canHandle->hal_fdcanP) != HAL_OK) {
		failure |= HAL_ERROR;
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
		failure |= status;
	}

	// Circular Buffer
	// canHandle->tx_buffer = GR_CircularBuffer_Create(config->tx_buffer_length);
	// canHandle->tx_buffer = malloc(sizeof(FDCANTxMessage)*canHandle->tx_buffer_length);
	if (!canHandle->tx_buffer) {
		LOGOMATIC("tx_buffer isn't valid?");
		failure |= 1;
	}

	if (failure) {
		can_msp_deinit(canHandle);
		FDCAN_InstanceDeInit(canHandle->hal_fdcanP);

		FDCAN_HandleTypeDef *temp = canHandle->hal_fdcanP;
		memset(canHandle, 0, sizeof(*canHandle)); // FIXME: Make sure instance is not being overwritten (FDCANx)
		canHandle->hal_fdcanP = temp;

		return NULL;
	}

	canHandle->init = true;
	canHandle->started = false;

	return canHandle;
}

CAN_STATUS can_release(CANHandle *canHandle)
{
	if (validate_can_handle(canHandle) != CAN_SUCCESS) {
		return CAN_ERROR;
	}

	if (!canHandle->init) {
		LOGOMATIC("CAN_release: can instance is already deinitialized");
		return CAN_ERROR;
	}

	if (can_stop(canHandle)) { // try to prevent more interrupts from firing
		LOGOMATIC("CAN_release: could not stop instance");
		return CAN_ERROR;
	}

	// No more interrupts should be firing that modify canHandle
	if (can_msp_deinit(canHandle) != CAN_SUCCESS) {
		LOGOMATIC("CAN_release: could not stop instance");
		return CAN_ERROR;
	}

	// reset FDCANx instance and message RAM and filters, clear interrupts
	// HAL_FDCAN_DeInit(canHandle->hal_fdcanP); resets a little too hard
	FDCAN_InstanceDeInit(canHandle->hal_fdcanP);

	__DSB(); // Data Synchronization Barrier
	__ISB(); // Instruction Synchronization Barrier

	// free circular buffer contents
	// GR_CircularBuffer_Free(&(canHandle->tx_buffer));
	memset((void *)canHandle->tx_buffer, 0, canHandle->tx_capacity * sizeof(canHandle->tx_buffer[0]));
	canHandle->tx_elements = 0;
	canHandle->tx_tail = 0;

	// reset can handle
	// FDCAN_HandleTypeDef *temp = canHandle->hal_fdcanP;
	// uint32_t capacity = canHandle->tx_capacity;
	// FDCANTxMessage* buff = canHandle->tx_buffer;

	canHandle->init = 0;
	canHandle->started = 0;
	canHandle->rx_callback = 0;
	canHandle->tx_elements = 0;
	canHandle->rx_gpio = 0;
	canHandle->rx_interrupt_priority = 0;
	canHandle->tx_interrupt_priority = 0;
	canHandle->tx_gpio = 0;
	canHandle->rx_pin = 0;
	canHandle->tx_pin = 0;
	canHandle->tx_tail = 0;

	// memset(canHandle, 0, sizeof(*canHandle));
	// canHandle->hal_fdcanP = temp;
	// canHandle->tx_capacity = capacity;
	// canHandle->tx_buffer = buff;

	return CAN_SUCCESS;
}
// TODO: Implement timer
// lock access to Circular Buffer when sending and dequeuing
static void can_tx_dequeue_helper(CANHandle *handle)
{
	// TODO: validate buffer
	if (!handle->tx_buffer) {
		LOGOMATIC("can_tx_buffer_helper: buffer is invalid");
		return;
	}

	// use interrupt masking in case any other ISRs need to lock the circular buffer
	uint32_t basepri = __get_BASEPRI();
	__set_BASEPRI(handle->tx_interrupt_priority << 4);
	// single consumer shouldn't affect state of circular buffer
	if (handle->tx_elements == 0) {
		__set_BASEPRI(basepri << 4);
		return;
	}

	// Can Add to Fifo Q
	if (HAL_FDCAN_GetTxFifoFreeLevel(handle->hal_fdcanP)) {
		const FDCANTxMessage *msg = &(handle->tx_buffer[handle->tx_tail]);

		// should call Tx Buffer Callback once complete
		HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(handle->hal_fdcanP, &msg->tx_header, msg->data);

		if (status != HAL_OK) {
			// LOGOMATIC("CAN_tx_helper: failed to add message to FIFO\n"); //FIXME: Logomatic may not work with interrupts disabled
			__set_BASEPRI(basepri << 4);
			return; // Stop trying to send more
		}
		// free(msg); // Successfully sent, free the entry in the circular buffer (which is pointed to by tail)
		handle->tx_tail = ++handle->tx_tail % handle->tx_capacity;
		handle->tx_elements--;

	} else { // FIXME: call can_tx_dequeue_helper later with a timer, if this gets implemented, need to mask timer interrupts to allow atomic access

	} // alternatively, if fifo is full, tx_dequeue should get called anyways, and we don't need the else statement

	__set_BASEPRI(basepri << 4);
}

CAN_STATUS can_send(CANHandle *canHandle, FDCANTxMessage *message)
{
	if (validate_can_handle(canHandle) != CAN_SUCCESS) {
		return CAN_ERROR;
	}

	if (!canHandle->started) {
		LOGOMATIC("CAN_send: instance is not started\n");
		return CAN_ERROR;
	}

	// TODO: validate message??
	if (message == NULL) {
		LOGOMATIC("CAN_send: received null pointer for <message>\n");
		return CAN_ERROR;
	}

	if (!canHandle->init || !canHandle->started) {
		LOGOMATIC("CAN_send: CAN not initialized or started\n");
		return CAN_ERROR;
	}

	// IF TX Fifos are not full, send directly to them
	// If TX Fifos are full, append to circular buffer
	// If circular buffer is full, return an error code

	// stop can_tx_dequeue_helper from from interleaving
	uint32_t basepri = __get_BASEPRI();
	__set_BASEPRI((canHandle->tx_interrupt_priority) << 4);

	uint32_t free = 0;
	if ((free = HAL_FDCAN_GetTxFifoFreeLevel(canHandle->hal_fdcanP)) > 0) {
		HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(canHandle->hal_fdcanP, &(message->tx_header), message->data);

		uint32_t val = 0;
		if (status != HAL_OK) {
			LOGOMATIC("CAN_send: failed to add to HW FIFO\n");
			val = CAN_ERROR;
		} else {
			val = CAN_SUCCESS;
		}
		__set_BASEPRI(basepri << 4);
		return val;
	}
	//}

	// Hardware FIFO full, try software buffer
	if (canHandle->tx_elements < canHandle->tx_capacity) {
		// int result = GR_CircularBuffer_Push(canHandle->tx_buffer, message, sizeof(FDCANTxMessage));

		uint32_t idx = (canHandle->tx_tail + canHandle->tx_elements) % canHandle->tx_capacity;
		canHandle->tx_buffer[idx] = *message;
		canHandle->tx_elements++;
		// memcpy(&canHandle->tx_buffer[idx], message , sizeof(FDCANTxMessage) );

		__set_BASEPRI(basepri << 4);
		return CAN_SUCCESS; // added to software buffer

		/*if (result != 0) {
		    LOGOMATIC("CAN_send: buffer push failed\n");
		    return CAN_ERROR;
		} else {
		    return CAN_SUCCESS;
		}*/
	} else {
		LOGOMATIC("CAN_send: all buffers full\n"); // p
	}
	__set_BASEPRI(basepri << 4);
	// Both buffers full
	return CAN_ERROR;
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
	UNUSED(BufferIndexes);
	// If circular buffer has elements, send to queue
	// Otherwise do nothing
	// #ifdef USECAN1
	CANHandle *handle = can_get_handle(hfdcan);
	// see if you can pop any more from the buffer
	can_tx_dequeue_helper(handle);
}

void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan)
{
	CANHandle *handle = can_get_handle(hfdcan);
	can_tx_dequeue_helper(handle);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	CANHandle *handle = can_get_handle(hfdcan);

	if (!handle || !handle->init || !handle->rx_callback) {
		return;
	}

	/*if (!handle->rx_buffer) {
	    LOGOMATIC("CAN: RX Complete, but %s Buffer was released\n", can_get_instance_name(hfdcan->Instance));
	    return;
	} */ // no rx buffer at the moment

	if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) {
		// TODO: Implement lost message counters
		//  lost_rx++;
	}

	if (!(RxFifo0ITs & ~FDCAN_IT_RX_FIFO0_MESSAGE_LOST)) {
		return;
	}

	// if (GR_CircularBuffer_IsFull(handle->rx_buffer)) return;
	FDCAN_RxHeaderTypeDef rx_header;

	// TODO: Stack allocation may be unsafe, (but not unsafer than heap)
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
	/*
	    if (GR_CircularBuffer_IsEmpty(handle->rx_buffer)) handle->rx_callback(rx_data, rx_header.DataLength);
	    else {
	    GR_CircularBuffer_Push(handle->rx_buffer, rx_data, rx_header.DataLength);
	    }
	}*/
	//__set_BASEPRI(prev_priority);
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
CAN_STATUS can_add_filter(CANHandle *canHandle, FDCAN_FilterTypeDef *filter)
{
	if (validate_can_handle(canHandle) != CAN_SUCCESS) {
		return CAN_ERROR;
	}

	if (!canHandle->init || canHandle->started) {
		LOGOMATIC("CAN_add_filter: can instance is not initialized or already started\n");
		return CAN_ERROR;
	}

	if (HAL_FDCAN_ConfigFilter(canHandle->hal_fdcanP, filter) != HAL_OK) {
		LOGOMATIC("CAN_add_filter: failed to configure filter\n");
		return CAN_ERROR;
	}
	return CAN_SUCCESS;
	// check that # of filters isn't exceeding max value
}

CAN_STATUS can_start(CANHandle *canHandle)
{
	if (validate_can_handle(canHandle) != CAN_SUCCESS) {
		return CAN_ERROR;
	}

	if (!canHandle->init) {
		LOGOMATIC("CAN_start: can instance is not initialized\n");
		return CAN_ERROR;
	}

	if (canHandle->started) {
		return CAN_SUCCESS;
	}

	IRQn_Type rx0it, txit;
	rx0it = txit = -1; // TOOD: Check that this is a valid way to initialize an invalid value
	if (can_get_irqs(canHandle->hal_fdcanP->Instance, &rx0it, &txit) != CAN_SUCCESS) {
		LOGOMATIC("can_start: could not obtain irq #s\n");
		return CAN_ERROR;
	}

	HAL_NVIC_ClearPendingIRQ(rx0it); // prevent a spurious interrupt
	HAL_NVIC_ClearPendingIRQ(txit);

	GPIOx_CLK_ENABLE(canHandle->rx_gpio);
	GPIOx_CLK_ENABLE(canHandle->tx_gpio);

	HAL_FDCAN_Start(canHandle->hal_fdcanP);

	canHandle->started = true;

	HAL_NVIC_EnableIRQ(rx0it);
	HAL_NVIC_EnableIRQ(txit);

	return CAN_SUCCESS;
}

CAN_STATUS can_stop(CANHandle *canHandle)
{
	if (validate_can_handle(canHandle) != CAN_SUCCESS) {
		return CAN_ERROR;
	}

	if (!canHandle->init) {
		LOGOMATIC("CAN_stop: can instance is not initialized\n");
		return CAN_ERROR;
	}

	if (!canHandle->started) {
		return CAN_SUCCESS;
	}

	// stop can interrupts from activating
	uint32_t prev_priority = __get_BASEPRI();
	__set_BASEPRI(MIN(canHandle->rx_interrupt_priority, canHandle->tx_interrupt_priority) << 4);

	if (HAL_FDCAN_Stop(canHandle->hal_fdcanP) != HAL_OK) {
		return CAN_ERROR;
	}

	IRQn_Type rx0it, txit;
	rx0it = txit = -1;
	if (can_get_irqs(canHandle->hal_fdcanP->Instance, &rx0it, &txit) != CAN_SUCCESS) {
		return CAN_ERROR;
	}

	HAL_NVIC_DisableIRQ(rx0it);
	HAL_NVIC_DisableIRQ(txit);
	HAL_NVIC_ClearPendingIRQ(rx0it);
	HAL_NVIC_ClearPendingIRQ(txit);

	__set_BASEPRI(prev_priority << 4);

	GPIOx_CLK_DISABLE(canHandle->rx_gpio);
	GPIOx_CLK_DISABLE(canHandle->tx_gpio);

	canHandle->started = false;

	return CAN_SUCCESS;
}

// ==================================== HELPER FUNCTIONS ===============================================
// TODO: Abstract across families

static inline void fdcan_enable_shared_clock(void)
{
	if (fdcan_shared_clock_ref == 0) {
		__HAL_RCC_FDCAN_CLK_ENABLE();
	}
	fdcan_shared_clock_ref++;
}

static inline void fdcan_disable_shared_clock(void)
{
	if (fdcan_shared_clock_ref > 0) {
		fdcan_shared_clock_ref--;
		if (fdcan_shared_clock_ref == 0) {
			__HAL_RCC_FDCAN_CLK_DISABLE();
		}
	}
}

// valid only for STM32G4
static CAN_STATUS can_get_irqs(FDCAN_GlobalTypeDef *instance, IRQn_Type *it0, IRQn_Type *it1)
{
#ifdef STM32G4
	if (instance == FDCAN1) {
		*it0 = FDCAN1_IT0_IRQn;
		*it1 = FDCAN1_IT1_IRQn;
		return CAN_SUCCESS;
	}
	if (instance == FDCAN2) {
		*it0 = FDCAN2_IT0_IRQn;
		*it1 = FDCAN2_IT1_IRQn;
		return CAN_SUCCESS;
	}
	if (instance == FDCAN3) {
		*it0 = FDCAN3_IT0_IRQn;
		*it1 = FDCAN3_IT1_IRQn;
		return CAN_SUCCESS;
	}
#endif

	LOGOMATIC("can_get_irqs: could not obtain irq #s\n");
	return CAN_ERROR; // invalid instance
}

static CANHandle *can_get_handle(FDCAN_HandleTypeDef *hfdcan)
{
#ifdef STM32G4
#ifdef USECAN1
	if (hfdcan->Instance == FDCAN1) {
		return &CAN1;
	}
#endif
#ifdef USECAN2
	if (hfdcan->Instance == FDCAN2) {
		return &CAN2;
	}
#endif
#ifdef USECAN3
	if (hfdcan->Instance == FDCAN3) {
		return &CAN3;
	}
#endif
#endif

	LOGOMATIC("CAN_get_handle: was given invalid FDCAN instance\n");
	UNUSED(hfdcan);
	return NULL;
}

static CAN_STATUS validate_can_handle(CANHandle *canHandle)
{

	if (canHandle == NULL) {
		LOGOMATIC("can.c: handle is null\n");
		return CAN_ERROR;
	}

#ifdef STM32G4
#ifdef USECAN1
	if (canHandle == &CAN1) {
		return CAN_SUCCESS;
	}
#endif
#ifdef USECAN2
	if (canHandle == &CAN2) {
		return CAN_SUCCESS;
	}
#endif
#ifdef USECAN3
	if (canHandle == &CAN3) {
		return CAN_SUCCESS;
	}
#endif
#endif

	LOGOMATIC("can.c: invalid can handle\n");
	return CAN_ERROR;
}

/*
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
}*/

// only valid for #STM32G474x, must redefine for each family
static CAN_STATUS can_msp_init(CANHandle *canHandle, CANConfig *config)
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
	if (can_get_irqs(canHandle->hal_fdcanP->Instance, &rxit, &txit) != CAN_SUCCESS) {
		return CAN_ERROR;
	}

	// rxfifo0
	HAL_NVIC_SetPriority(rxit, config->rx_interrupt_priority, 0);

	// tx
	HAL_NVIC_SetPriority(txit, config->tx_interrupt_priority, 0);
	// End MSP Init --------------

	// Call can_start() to enable interrupts

	return CAN_SUCCESS;
}

// Valid only for STM32G474xE
static CAN_STATUS can_msp_deinit(CANHandle *canHandle)
{
	// MSP DeInit
	//  must disable NVIC IRQs before freeing circular buffer

	// NVIC
	IRQn_Type rx0it = -1;
	IRQn_Type txit = -1;
	if (can_get_irqs(canHandle->hal_fdcanP->Instance, &rx0it, &txit) != CAN_SUCCESS) {
		return CAN_ERROR;
	}

	HAL_NVIC_DisableIRQ(rx0it);
	HAL_NVIC_DisableIRQ(txit);

	// TODO: turn off gpio clocks if no other peripherals are using them??? Could implement a shared GPIO layer
	// HAL_GPIO_DeInit(canHandle->rx_gpio, canHandle->rx_pin);
	// HAL_GPIO_DeInit(canHandle->tx_gpio, canHandle->tx_pin);

	// MSP shared layer for GPIOs
	// TODO: used to disable GPIOs clocks, but that might affect other peripherals

	// RCC
	// can only disable clock after resetting all FDCAN instances
	return CAN_SUCCESS;
}

static void FDCAN_InstanceDeInit(FDCAN_HandleTypeDef *hfdcan)
{
	// Enter INIT mode
	hfdcan->Instance->CCCR |= FDCAN_CCCR_INIT;
	while (!(hfdcan->Instance->CCCR & FDCAN_CCCR_INIT))
		;
	hfdcan->Instance->CCCR |= FDCAN_CCCR_CCE;

	// Disable interrupts
	//__HAL_FDCAN_DISABLE_IT(hfdcan, FDCAN_IT_LIST_RX_FIFO0 | FDCAN_IT_LIST_RX_FIFO1 | FDCAN_IT_LIST_SMSG | FDCAN_IT_LIST_TX_FIFO_ERROR | FDCAN_IT_LIST_MISC | //FDCAN_IT_LIST_BIT_LINE_ERROR |
	// FDCAN_IT_LIST_PROTOCOL_ERROR);

	//
	CLEAR_BIT(hfdcan->Instance->ILE, (FDCAN_INTERRUPT_LINE0 | FDCAN_INTERRUPT_LINE1));

	// Clear filters
	// TODO: fix magic numbers
	memset((void *)hfdcan->msgRam.StandardFilterSA, 0, 0x0070);
	memset((void *)hfdcan->msgRam.ExtendedFilterSA, 0, 0x0050);

	// Optionally clear FIFOs / buffers

	// Exit INIT mode
	hfdcan->Instance->CCCR &= ~FDCAN_CCCR_INIT;
	while (hfdcan->Instance->CCCR & FDCAN_CCCR_INIT)
		;

	// Update handle state
	hfdcan->State = HAL_FDCAN_STATE_RESET;

	fdcan_disable_shared_clock();
}

// valid only for STM32G4
/*static const char *can_get_instance_name(FDCAN_GlobalTypeDef *instance)
{
 if (instance == FDCAN1) {
     return "FDCAN1";
 } else if (instance == FDCAN2) {
     return "FDCAN2";
 } else if (instance == FDCAN3) {
     return "FDCAN3";
 }
 return "UNKNOWN";
}*/

// ===================================== HAL Callbacks ================================
// TODO: Implement Family Checks
// Probably is safe from races
void FDCAN1_IT0_IRQHandler(void)
{
#ifdef USECAN1
	HAL_FDCAN_IRQHandler(&hal_fdcan1);
#endif
}
void FDCAN1_IT1_IRQHandler(void)
{
#ifdef USECAN1
	HAL_FDCAN_IRQHandler(&hal_fdcan1);
#endif
}

void FDCAN2_IT0_IRQHandler(void)
{
#ifdef USECAN2
	HAL_FDCAN_IRQHandler(&hal_fdcan2);
#endif
}
void FDCAN2_IT1_IRQHandler(void)
{
#ifdef USECAN2
	HAL_FDCAN_IRQHandler(&hal_fdcan2);
#endif
}

void FDCAN3_IT0_IRQHandler(void)
{
#ifdef USECAN3
	HAL_FDCAN_IRQHandler(&hal_fdcan3);
#endif
}
void FDCAN3_IT1_IRQHandler(void)
{
#ifdef USECAN3
	HAL_FDCAN_IRQHandler(&hal_fdcan3);
#endif
}
