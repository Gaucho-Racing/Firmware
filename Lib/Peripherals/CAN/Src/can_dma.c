
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_bus.h"
#include <stdbool.h>

#include "can_dma.h"

//#include "can_platform_deps.h"
#include "STM32G4_hal_fdcan_defines.h"
#include "Logomatic.h"

//#define DMA_INTERRUPT //can't even think about trying this yet

//static void DMA_M2M_BlockingTransfer(uint8_t *src, uint8_t *dst, uint32_t byte_count);
static void DMA_M2M_Transfer(uint8_t *src, uint8_t *dst, uint32_t byte_count);

/*
typedef struct dma_transfer_t {
	uint32_t can_id;
    uint8_t data[64] __attribute__((aligned(4)));
	uint32_t size;

	uint32_t RxLocation;
	FDCAN_HandleTypeDef* hfdcan;
	uint32_t GetIndex;

	volatile bool unconsumed;

} dma_transfer_t;
*/

//static volatile dma_transfer_t dma1_ch1 = {0};

static uint32_t msg_count = 0;
HAL_StatusTypeDef FDCAN_GetRxMessage_DMA(FDCAN_HandleTypeDef* hfdcan, uint32_t RxLocation, FDCAN_RxHeaderTypeDef *pRxHeader, uint8_t *pRxData)
{
	uint32_t *RxAddress;
	uint8_t *pData;
	uint32_t ByteCounter; UNUSED(ByteCounter);
	uint32_t GetIndex;

	//FDCAN_HandleTypeDef* hfdcan = handle->hal_fdcanP;
	HAL_FDCAN_StateTypeDef state = hfdcan->State;

	/* Check function parameters */
	assert_param(IS_FDCAN_RX_FIFO(RxLocation));

	if (state == HAL_FDCAN_STATE_BUSY) {
		if (RxLocation == FDCAN_RX_FIFO0) /* Rx element is assigned to the Rx FIFO 0 */
		{
			/* Check that the Rx FIFO 0 is not empty */
			if ((hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0FL) == 0U) {
				/* Update error code */
				hfdcan->ErrorCode |= HAL_FDCAN_ERROR_FIFO_EMPTY;

				return HAL_ERROR;
			} else {
				/* Calculate Rx FIFO 0 element index */
				GetIndex = ((hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos);

				/* Check that the Rx FIFO 0 is full & overwrite mode is on */
				if (((hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0F) >> FDCAN_RXF0S_F0F_Pos) == 1U) {
					if (((hfdcan->Instance->RXGFC & FDCAN_RXGFC_F0OM) >> FDCAN_RXGFC_F0OM_Pos) == FDCAN_RX_FIFO_OVERWRITE) {
						/* When overwrite status is on discard first message in FIFO */
						/* GetIndex is incremented by one and wraps to 0 in case it overflows the FIFO size */
						GetIndex = (GetIndex + 1U) & SRAMCAN_RF0_NBR;
					}
				}

				/* Calculate Rx FIFO 0 element address */
				RxAddress = (uint32_t *)(hfdcan->msgRam.RxFIFO0SA + (GetIndex * SRAMCAN_RF0_SIZE));
			}
		} else /* Rx element is assigned to the Rx FIFO 1 */
		{
			/* Check that the Rx FIFO 1 is not empty */
			if ((hfdcan->Instance->RXF1S & FDCAN_RXF1S_F1FL) == 0U) {
				/* Update error code */
				hfdcan->ErrorCode |= HAL_FDCAN_ERROR_FIFO_EMPTY;

				return HAL_ERROR;
			} else {
				/* Calculate Rx FIFO 1 element index */
				GetIndex = ((hfdcan->Instance->RXF1S & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos);

				/* Check that the Rx FIFO 1 is full & overwrite mode is on */
				if (((hfdcan->Instance->RXF1S & FDCAN_RXF1S_F1F) >> FDCAN_RXF1S_F1F_Pos) == 1U) {
					if (((hfdcan->Instance->RXGFC & FDCAN_RXGFC_F1OM) >> FDCAN_RXGFC_F1OM_Pos) == FDCAN_RX_FIFO_OVERWRITE) {
						/* When overwrite status is on discard first message in FIFO */
						/* GetIndex is incremented by one and wraps to 0 in case it overflows the FIFO size */
						GetIndex = (GetIndex + 1U) & SRAMCAN_RF1_NBR;
					}
				}

				/* Calculate Rx FIFO 1 element address */
				RxAddress = (uint32_t *)(hfdcan->msgRam.RxFIFO1SA + (GetIndex * SRAMCAN_RF1_SIZE));
			}
		}

		msg_count++;



		/* Retrieve IdType */
		pRxHeader->IdType = *RxAddress & FDCAN_ELEMENT_MASK_XTD;

		/* Retrieve Identifier */
		if (pRxHeader->IdType == FDCAN_STANDARD_ID) /* Standard ID element */
		{
			pRxHeader->Identifier = ((*RxAddress & FDCAN_ELEMENT_MASK_STDID) >> 18U);
		} else /* Extended ID element */
		{
			pRxHeader->Identifier = (*RxAddress & FDCAN_ELEMENT_MASK_EXTID);
		}

		/* Retrieve RxFrameType */
		pRxHeader->RxFrameType = (*RxAddress & FDCAN_ELEMENT_MASK_RTR);

		/* Retrieve ErrorStateIndicator */
		pRxHeader->ErrorStateIndicator = (*RxAddress & FDCAN_ELEMENT_MASK_ESI);

		/* Increment RxAddress pointer to second word of Rx FIFO element */
		RxAddress++;

		/* Retrieve RxTimestamp */
		pRxHeader->RxTimestamp = (*RxAddress & FDCAN_ELEMENT_MASK_TS);

		/* Retrieve DataLength */
		pRxHeader->DataLength = ((*RxAddress & FDCAN_ELEMENT_MASK_DLC) >> 16U);

		/* Retrieve BitRateSwitch */
		pRxHeader->BitRateSwitch = (*RxAddress & FDCAN_ELEMENT_MASK_BRS);

		/* Retrieve FDFormat */
		pRxHeader->FDFormat = (*RxAddress & FDCAN_ELEMENT_MASK_FDF);

		/* Retrieve FilterIndex */
		pRxHeader->FilterIndex = ((*RxAddress & FDCAN_ELEMENT_MASK_FIDX) >> 24U);

		/* Retrieve NonMatchingFrame */
		pRxHeader->IsFilterMatchingFrame = ((*RxAddress & FDCAN_ELEMENT_MASK_ANMF) >> 31U);

		/* Increment RxAddress pointer to payload of Rx FIFO element */
		RxAddress++;


		/*LOGOMATIC("msg#%lu F0GI=%lu GetIndex=%lu RxFIFO0SA=0x%08lx PayloadAddr=0x%08lx\n",
		msg_count,
		(hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos,
		GetIndex,
		hfdcan->msgRam.RxFIFO0SA,
		(uint32_t)RxAddress);  // after the two RxAddress++ increments


		uint8_t *dbg = (uint8_t*)RxAddress;  // after the two increments
		LOGOMATIC("src byte48=0x%02x\n", dbg[48]);
		*/

		/* Retrieve Rx payload with DMA*/
		pData = (uint8_t *)RxAddress;
		uint32_t bytes = DLCtoBytes[pRxHeader->DataLength];
		//DMA_M2M_BlockingTransfer(pData, pRxData, bytes);


		// ====================== THIS SECTION IS DIFFERENT FROM HAL_FDCAN_GetRxMessage()===========================
		#ifdef DMA_INTERRUPT
		while (dma1_ch1.unconsumed);

		dma1_ch1.unconsumed = true;
		dma1_ch1.can_id = pRxHeader->Identifier;
		dma1_ch1.size = DLCtoBytes[pRxHeader->DataLength];
		dma1_ch1.RxLocation = RxLocation;
		dma1_ch1.hfdcan = hfdcan;
		dma1_ch1.GetIndex = GetIndex;

		DMA_M2M_Transfer(pData, dma1_ch1.data, bytes);

		#else
		DMA_M2M_Transfer(pData, pRxData, bytes);

		//===============================================================================

		if (RxLocation == FDCAN_RX_FIFO0) /* Rx element is assigned to the Rx FIFO 0 */
		{
			/* Acknowledge the Rx FIFO 0 that the oldest element is read so that it increments the GetIndex */
			hfdcan->Instance->RXF0A = GetIndex;
		} else /* Rx element is assigned to the Rx FIFO 1 */
		{
			/* Acknowledge the Rx FIFO 1 that the oldest element is read so that it increments the GetIndex */
			hfdcan->Instance->RXF1A = GetIndex;
		}
		#endif

		/*for (ByteCounter = 0; ByteCounter < DLCtoBytes[pRxHeader->DataLength]; ByteCounter++) {
			pRxData[ByteCounter] = pData[ByteCounter];
		}*/

		/* Return function status */
		return HAL_OK;


	} else {
		/* Update error code */
		hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_STARTED;

		return HAL_ERROR;
	}
}



CAN_RXCallback DMA1_rx_callback = 0;
void DMA_M2M_Init(uint32_t preempt, uint32_t subpriority, CAN_RXCallback callback)
{
    //Enable DMA1 clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);

    LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_1,
        LL_DMA_DIRECTION_MEMORY_TO_MEMORY |
        LL_DMA_MODE_NORMAL                |
        LL_DMA_PERIPH_INCREMENT           |   // src increment
        LL_DMA_MEMORY_INCREMENT           |   // dst increment
        LL_DMA_PDATAALIGN_WORD            |   // src word (32-bit)
        LL_DMA_MDATAALIGN_WORD            |   // dst word (32-bit)
        LL_DMA_PRIORITY_HIGH);

    //For M2M, DMAMUX must be set to a software request line (0)
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_MEM2MEM);

	#ifndef DMA_INTERRUPT
	UNUSED(preempt);
	UNUSED(subpriority);
	UNUSED(callback);
	#else

	DMA1_rx_callback = callback;

	LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_TE1(DMA1);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_1);

	NVIC_SetPriority(DMA1_Channel1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), preempt, subpriority));
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	#endif
}



/*
#define RxFifoElementsNumber 3
bool is_valid_rxfifo0_address(FDCAN_HandleTypeDef *hfdcan, uint32_t *RxAddress)
{
    uint32_t fifo_start = hfdcan->msgRam.RxFIFO0SA;
    uint32_t fifo_end   = fifo_start + (RxFifoElementsNumber * SRAMCAN_RF0_SIZE);

    uint32_t addr = (uint32_t)RxAddress;

    return (addr >= fifo_start) && (addr + SRAMCAN_RF0_SIZE <= fifo_end);
}

bool is_valid_rxfifo1_address(FDCAN_HandleTypeDef *hfdcan, uint32_t *RxAddress) {
	uint32_t fifo_start = hfdcan->msgRam.RxFIFO0SA;
    uint32_t fifo_end   = fifo_start + (RxFifoElementsNumber * SRAMCAN_RF0_SIZE);

    uint32_t addr = (uint32_t)RxAddress;

    return (addr >= fifo_start) && (addr + SRAMCAN_RF0_SIZE <= fifo_end);
	return false;
}*/


void DMA_M2M_Transfer(uint8_t *src, uint8_t *dst, uint32_t byte_count)
{

	if (byte_count == 0) return;

    /* Disable channel before reconfiguring */
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);

    LL_DMA_SetMemoryAddress(DMA1,  LL_DMA_CHANNEL_1, (uint32_t)dst);

	LL_DMA_SetPeriphAddress(DMA1,  LL_DMA_CHANNEL_1, (uint32_t)src);
	//dma_src_start = src;

	//access FDDCAN Message RAM in 32-bit words, not in bytes
	LL_DMA_SetDataLength(DMA1,     LL_DMA_CHANNEL_1, byte_count >> 2);

	#ifdef DMA_INTERRUPT
    /* Clear any pending flags before enabling */
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_TE1(DMA1);
	#endif

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

	#ifndef DMA_INTERRUPT
    /* Poll for transfer complete */
    while (!LL_DMA_IsActiveFlag_TC1(DMA1)) {}
	__DSB();
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
	#endif
}




/*
//TODO: Abstract out to handle other FDCAN instances besides FDCAN1 and FDCAN2
void FDCAN1_DMA_TC(uint8_t *src_addr) {
	if (!is_valid_rxfifo0_address(&hal_fdcan1, (uint32_t*)src_addr)) {
		LOGOMATIC("DMA src addr des not correspond to rx fifo 0");
		return;
	}

	uint32_t GetIndex = ((FDCAN1->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos);

	// Acknowledge the Rx FIFO 0 that the oldest element is read so that it increments the GetIndex
	//hal_fdcan1.Instance->RXF0A = GetIndex;
	FDCAN1->RXF0A = GetIndex;

	//HAL_FDCAN_RxFifo0Callback()

	//else  Rx element is assigned to the Rx FIFO 1
	//{
		// Acknowledge the Rx FIFO 1 that the oldest element is read so that it increments the GetIndex
	//	hfdcan->Instance->RXF1A = GetIndex;
	//}

}

uint32_t transfer_errors = 0;
void DMA1_Channel1_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC1(DMA1))
    {
        LL_DMA_ClearFlag_TC1(DMA1);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);

        // transfer complete - notify application
		//FDCAN1_DMA_TC(dma_src_start);
		if (dma1_ch1.RxLocation == FDCAN_RX_FIFO0) // Rx element is assigned to the Rx FIFO 0
		{
			// Acknowledge the Rx FIFO 0 that the oldest element is read so that it increments the GetIndex
			dma1_ch1.hfdcan->Instance->RXF0A = dma1_ch1.GetIndex;
		} else // Rx element is assigned to the Rx FIFO 1
		{
			// Acknowledge the Rx FIFO 1 that the oldest element is read so that it increments the GetIndex
			dma1_ch1.hfdcan->Instance->RXF1A = dma1_ch1.GetIndex;
		}

		//
		DMA1_rx_callback(dma1_ch1.can_id, dma1_ch1.data,  dma1_ch1.size);

		dma1_ch1.unconsumed = false;
    }

    if (LL_DMA_IsActiveFlag_TE1(DMA1))
    {
        LL_DMA_ClearFlag_TE1(DMA1);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);

        // handle error
		//TODO: Handle DMA Transfer error
		transfer_errors++;

		dma1_ch1.unconsumed = false;
    }
}*/
