/*!
 * @file bcc_communication.c
 *
 * This file implements the basic low-level functions for both SPI and TPL
 * communication with MC33771C and MC33772C BCC devices.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "bcc_communication.h"
#include <stdio.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Size of CRC table. */
#define BCC_CRC_TBL_SIZE          256U

/** BCC Commands. */
/*! @brief No operation command. */
#define BCC_CMD_NOOP              0x00U
/*! @brief Read command. */
#define BCC_CMD_READ              0x01U
/*! @brief Write command. */
#define BCC_CMD_WRITE             0x02U
/*! @brief Global write command. */
#define BCC_CMD_GLOB_WRITE        0x03U

/*!
 * @brief Returns data field of the communication frame.
 *
 * @param msg Pointer to the frame.
 * @return Data field.
 */
#define BCC_GET_MSG_DATA(msg) \
    (((uint16_t)*((msg) + BCC_MSG_IDX_DATA_H) << 8U) | \
      (uint16_t)*((msg) + BCC_MSG_IDX_DATA_L))

/*! @brief Mask for address field of frame. */
#define BCC_MSG_ADDR_MASK         0x7FU
/*! @brief Mask of the message counter bit field within the BCC_MSG_IDX_CNT_CMD
 *  byte. */
#define BCC_MSG_MSG_CNT_MASK      0xF0U
/*! @brief Shift of the message counter bit field within the BCC_MSG_IDX_CNT_CMD
 *  byte. */
#define BCC_MSG_MSG_CNT_SHIFT     4U

/*!
 * @brief Increments message counter value and executes modulo 16.
 *
 * @param msgCntr Message counter to be incremented.
 * @return Incremented value.
 */
#define BCC_INC_MSG_CNTR(msgCntr) \
    (((msgCntr) + 1U) & 0x0FU)

/*!
 * @brief Returns true if all bit fields except Message counter and CRC field
 * are zero.
 *
 * @param resp Response message to be checked.
 * @return True when the response is zero (except CRC and MSG_CNTR), false
 *         otherwise.
 */
#define BCC_IS_NULL_RESP(resp) \
    (((resp)[BCC_MSG_IDX_DATA_H] == 0U) && \
     ((resp)[BCC_MSG_IDX_DATA_L] == 0U) && \
     ((resp)[BCC_MSG_IDX_ADDR] == 0U) && \
     ((resp)[BCC_MSG_IDX_CID] == 0U) && \
     (((resp)[BCC_MSG_IDX_CNT_CMD] & (~BCC_MSG_MSG_CNT_MASK)) == 0U))

/*! @brief Address of the last register. */
#define BCC_MAX_REG_ADDR       0x7FU

/*******************************************************************************
 * Constant variables
 ******************************************************************************/

/* Table with precalculated CRC values. */
static const uint8_t s_crcTable[BCC_CRC_TBL_SIZE] = {
    0x00U, 0x2fU, 0x5eU, 0x71U, 0xbcU, 0x93U, 0xe2U, 0xcdU,
    0x57U, 0x78U, 0x09U, 0x26U, 0xebU, 0xc4U, 0xb5U, 0x9aU,
    0xaeU, 0x81U, 0xf0U, 0xdfU, 0x12U, 0x3dU, 0x4cU, 0x63U,
    0xf9U, 0xd6U, 0xa7U, 0x88U, 0x45U, 0x6aU, 0x1bU, 0x34U,
    0x73U, 0x5cU, 0x2dU, 0x02U, 0xcfU, 0xe0U, 0x91U, 0xbeU,
    0x24U, 0x0bU, 0x7aU, 0x55U, 0x98U, 0xb7U, 0xc6U, 0xe9U,
    0xddU, 0xf2U, 0x83U, 0xacU, 0x61U, 0x4eU, 0x3fU, 0x10U,
    0x8aU, 0xa5U, 0xd4U, 0xfbU, 0x36U, 0x19U, 0x68U, 0x47U,
    0xe6U, 0xc9U, 0xb8U, 0x97U, 0x5aU, 0x75U, 0x04U, 0x2bU,
    0xb1U, 0x9eU, 0xefU, 0xc0U, 0x0dU, 0x22U, 0x53U, 0x7cU,
    0x48U, 0x67U, 0x16U, 0x39U, 0xf4U, 0xdbU, 0xaaU, 0x85U,
    0x1fU, 0x30U, 0x41U, 0x6eU, 0xa3U, 0x8cU, 0xfdU, 0xd2U,
    0x95U, 0xbaU, 0xcbU, 0xe4U, 0x29U, 0x06U, 0x77U, 0x58U,
    0xc2U, 0xedU, 0x9cU, 0xb3U, 0x7eU, 0x51U, 0x20U, 0x0fU,
    0x3bU, 0x14U, 0x65U, 0x4aU, 0x87U, 0xa8U, 0xd9U, 0xf6U,
    0x6cU, 0x43U, 0x32U, 0x1dU, 0xd0U, 0xffU, 0x8eU, 0xa1U,
    0xe3U, 0xccU, 0xbdU, 0x92U, 0x5fU, 0x70U, 0x01U, 0x2eU,
    0xb4U, 0x9bU, 0xeaU, 0xc5U, 0x08U, 0x27U, 0x56U, 0x79U,
    0x4dU, 0x62U, 0x13U, 0x3cU, 0xf1U, 0xdeU, 0xafU, 0x80U,
    0x1aU, 0x35U, 0x44U, 0x6bU, 0xa6U, 0x89U, 0xf8U, 0xd7U,
    0x90U, 0xbfU, 0xceU, 0xe1U, 0x2cU, 0x03U, 0x72U, 0x5dU,
    0xc7U, 0xe8U, 0x99U, 0xb6U, 0x7bU, 0x54U, 0x25U, 0x0aU,
    0x3eU, 0x11U, 0x60U, 0x4fU, 0x82U, 0xadU, 0xdcU, 0xf3U,
    0x69U, 0x46U, 0x37U, 0x18U, 0xd5U, 0xfaU, 0x8bU, 0xa4U,
    0x05U, 0x2aU, 0x5bU, 0x74U, 0xb9U, 0x96U, 0xe7U, 0xc8U,
    0x52U, 0x7dU, 0x0cU, 0x23U, 0xeeU, 0xc1U, 0xb0U, 0x9fU,
    0xabU, 0x84U, 0xf5U, 0xdaU, 0x17U, 0x38U, 0x49U, 0x66U,
    0xfcU, 0xd3U, 0xa2U, 0x8dU, 0x40U, 0x6fU, 0x1eU, 0x31U,
    0x76U, 0x59U, 0x28U, 0x07U, 0xcaU, 0xe5U, 0x94U, 0xbbU,
    0x21U, 0x0eU, 0x7fU, 0x50U, 0x9dU, 0xb2U, 0xc3U, 0xecU,
    0xd8U, 0xf7U, 0x86U, 0xa9U, 0x64U, 0x4bU, 0x3aU, 0x15U,
    0x8fU, 0xa0U, 0xd1U, 0xfeU, 0x33U, 0x1cU, 0x6dU, 0x42U
};

/*******************************************************************************
 * Prototypes of internal functions
 ******************************************************************************/

/*!
 * @brief This function calculates CRC value for the SPI frame.
 *
 * @param data Pointer to the SPI frame.
 *
 * @return Computed CRC value.
 */
static inline uint8_t BCC_CalcCRC(const uint8_t* const data);

/*!
 * @brief This function calculates CRC of a received frame and compares
 * it with CRC field of this frame.
 *
 * @param resp Pointer to memory that contains a response (frame) to be checked.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_CheckCRC(const uint8_t* const resp);

/*!
 * @brief This function checks value of the Message counter field of a frame.
 * It is done by comparing a value parsed from the frame with a value from
 * previously received frame, which is internally stored in the driver
 * configuration structure.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of communicating BCC device.
 * @param resp      Pointer to memory that contains a response (frame) to be
 *                  checked.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_CheckMsgCntr(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t* const resp);

/*!
 * @brief This function checks content of the received echo frame. It should be
 * equal to the sent frame.
 *
 * @param txBuf Pointer to memory that contains the sent frame.
 * @param resp  Pointer to memory that contains a response (frame) to be
 *              checked.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_CheckEchoFrame(const uint8_t* const txBuf,
    const uint8_t* const resp);

/*!
 * @brief This function packs all the parameters into a frame according to
 * the BCC frame format (see BCC datasheet).
 *
 * @param data   16 bit Register Data field of the BCC frame.
 * @param addr   7 bit Register Address field of the BCC frame.
 * @param cid    6 bit Physical Address field of the BCC frame.
 * @param cntCmd 4 bit Message Counter and 2 bit Command field of the BCC frame.
 *               -----------------------------------------
 *               | Message counter | Reserved |  Command |
 *               |    Bit[7:4]     | Bit[3:2] | Bit[1:0] |
 *               -----------------------------------------
 * @param frame  Pointer to a BCC_MSG_SIZE-byte array where all the fields and
 *               computed CRC will be stored. Note that fields are stored into
 *               the array according to BCC_MSG_IDX_* macros.
 */
static void BCC_PackFrame(const uint16_t data, const uint8_t addr,
    const bcc_cid_t cid, const uint8_t cmdCnt, uint8_t* const frame);

/*******************************************************************************
 * Internal function
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_CalcCRC
 * Description   : This function calculates CRC value of passed data array.
 *
 *END**************************************************************************/
static inline uint8_t BCC_CalcCRC(const uint8_t* const data)
{
    uint8_t crc;      /* Result. */
    uint8_t tableIdx; /* Index to the CRC table. */

    BCC_MCU_Assert(data != NULL);

    /* Expanding value. */
    crc = 0x42U;

    tableIdx = crc ^ data[BCC_MSG_IDX_DATA_H];
    crc = s_crcTable[tableIdx];
    tableIdx = crc ^ data[BCC_MSG_IDX_DATA_L];
    crc = s_crcTable[tableIdx];
    tableIdx = crc ^ data[BCC_MSG_IDX_ADDR];
    crc = s_crcTable[tableIdx];
    tableIdx = crc ^ data[BCC_MSG_IDX_CID];
    crc = s_crcTable[tableIdx];
    tableIdx = crc ^ data[BCC_MSG_IDX_CNT_CMD];
    crc = s_crcTable[tableIdx];

    return crc;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_CheckCRC
 * Description   : This function calculates CRC of a received frame and compares
 *                 it with CRC field of the frame.
 *
 *END**************************************************************************/
static bcc_status_t BCC_CheckCRC(const uint8_t* const resp)
{
    uint8_t frameCrc;  /* CRC value from the response frame. */
    uint8_t compCrc;   /* Computed (expected) CRC value. */

    BCC_MCU_Assert(resp != NULL);

    frameCrc = resp[BCC_MSG_IDX_CRC];
    compCrc = BCC_CalcCRC(resp);

    return (compCrc != frameCrc) ? BCC_STATUS_COM_CRC : BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_CheckMsgCnt
 * Description   : This function checks value of the Message counter field of
 *                 a frame.
 *
 *END**************************************************************************/
static bcc_status_t BCC_CheckMsgCntr(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t* const resp)
{
    uint8_t msgCntPrev;  /* Previously received message counter value. */
    uint8_t msgCntRcv;   /* Currently received message counter value. */

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(resp != NULL);

    msgCntPrev = drvConfig->drvData.msgCntr[(uint8_t)cid];
    msgCntRcv = (resp[BCC_MSG_IDX_CNT_CMD] & BCC_MSG_MSG_CNT_MASK) >> BCC_MSG_MSG_CNT_SHIFT;

    /* Store the Message counter value. */
    drvConfig->drvData.msgCntr[(uint8_t)cid] = msgCntRcv;

    /* Check the Message counter value.
     * Note: Do not perform a check for CID=0. */
    if ((cid != BCC_CID_UNASSIG) && (msgCntRcv != BCC_INC_MSG_CNTR(msgCntPrev)))
    {
        return BCC_STATUS_COM_MSG_CNT;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_CheckEchoFrame
 * Description   : This function checks content of the echo frame.
 *
 *END**************************************************************************/
static bcc_status_t BCC_CheckEchoFrame(const uint8_t* const txBuf,
    const uint8_t* const resp)
{
    BCC_MCU_Assert(resp != NULL);
    BCC_MCU_Assert(txBuf != NULL);

    if ((txBuf[BCC_MSG_IDX_DATA_H] == resp[BCC_MSG_IDX_DATA_H]) &&
        (txBuf[BCC_MSG_IDX_DATA_L] == resp[BCC_MSG_IDX_DATA_L]) &&
        (txBuf[BCC_MSG_IDX_ADDR] == resp[BCC_MSG_IDX_ADDR]) &&
        (txBuf[BCC_MSG_IDX_CID] == resp[BCC_MSG_IDX_CID]) &&
        (txBuf[BCC_MSG_IDX_CNT_CMD] == resp[BCC_MSG_IDX_CNT_CMD]) &&
        (txBuf[BCC_MSG_IDX_CRC] == resp[BCC_MSG_IDX_CRC]))
    {
        return BCC_STATUS_SUCCESS;
    }
    else
    {
        return BCC_STATUS_COM_ECHO;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_PackFrame
 * Description   : This function packs all the parameters into a frame according
 *                 to the BCC frame format (see BCC datasheet).
 *
 *END**************************************************************************/
static void BCC_PackFrame(const uint16_t data, const uint8_t addr,
    const bcc_cid_t cid, const uint8_t cmdCnt, uint8_t* const frame)
{
    BCC_MCU_Assert(frame != NULL);

    /* Register Data field. */
    frame[BCC_MSG_IDX_DATA_H] = (uint8_t)(data >> 8U);
    frame[BCC_MSG_IDX_DATA_L] = (uint8_t)(data & 0xFFU);

    /* Register Address field. Master/Slave field is always 0 for sending. */
    frame[BCC_MSG_IDX_ADDR] = (addr & BCC_MSG_ADDR_MASK);

    /* Device address (Cluster ID) field. */
    frame[BCC_MSG_IDX_CID] = ((uint8_t)cid & 0x3FU);

    /* Message counter and Command fields. */
    frame[BCC_MSG_IDX_CNT_CMD] = (cmdCnt & 0xF3U);

    /* CRC field. */
    frame[BCC_MSG_IDX_CRC] = BCC_CalcCRC(frame);
}

/******************************************************************************
 * API
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_ReadTpl
 * Description   : This function reads desired number of registers of the BCC
 *                 device. Intended for TPL mode only.
 *
 *END**************************************************************************/
extern volatile uint8_t TPL_RxBufferLevel;
bcc_status_t BCC_Reg_ReadTpl(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t regAddr, const uint8_t regCnt,
    uint16_t* regVal)
{
    uint8_t txBuf[BCC_MSG_SIZE]; /* Transmission buffer. */
    uint8_t *rxBuf;              /* Pointer to received data. */
    uint8_t regIdx;              /* Index of a received register. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(regVal != NULL);

    if (((uint8_t)cid > drvConfig->devicesCnt) || (regAddr > BCC_MAX_REG_ADDR) ||
        (regCnt == 0U) || ((regAddr + regCnt - 1U) > BCC_MAX_REG_ADDR))
    {
        return BCC_STATUS_PARAM_RANGE;
    }
    /* Create frame for request. */
    BCC_PackFrame((uint16_t)regCnt, regAddr, cid, BCC_CMD_READ, txBuf);
    /* Pointer to beginning of the received frame. */
    rxBuf = (uint8_t *)(drvConfig->drvData.rxBuf);

    status = BCC_MCU_TransferTpl(drvConfig->drvInstance, txBuf, rxBuf, regCnt + 1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check the echo frame. */
    status = BCC_CheckEchoFrame(txBuf, rxBuf);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check and store responses. */
    for (regIdx = 0U; regIdx < regCnt; regIdx++)
    {
        rxBuf += BCC_MSG_SIZE;

        /* Check CRC. */
        if ((status = BCC_CheckCRC(rxBuf)) != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        /* Check the Message counter value. */
        if ((status = BCC_CheckMsgCntr(drvConfig, cid, rxBuf)) != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        /* Store data. */
        *regVal++ = BCC_GET_MSG_DATA(rxBuf);
    }
    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_ReadSpi
 * Description   : This function reads desired number of registers of the BCC
 *                 device. Intended for SPI mode only.
 *
 *END**************************************************************************/
bcc_status_t BCC_Reg_ReadSpi(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint8_t regAddr, const uint8_t regCnt,
    uint16_t* regVal)
{
    uint8_t txBuf[BCC_MSG_SIZE]; /* Transmission buffer. */
    uint8_t rxBuf[BCC_MSG_SIZE]; /* Buffer for receiving. */
    uint8_t regIdx;              /* Index of a received register. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(regVal != NULL);

    if (((uint8_t)cid > drvConfig->devicesCnt) || (regAddr > BCC_MAX_REG_ADDR) ||
        (regCnt == 0U) || ((regAddr + regCnt - 1U) > BCC_MAX_REG_ADDR))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Create frame for request. */
    BCC_PackFrame(0x0001U, regAddr, cid, BCC_CMD_READ, txBuf);

    /* Send request for data. Required data are returned with the following transfer. */
    status = BCC_MCU_TransferSpi(drvConfig->drvInstance, txBuf, rxBuf);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check CRC, message counter, a null response (all field except CRC and
     * message counter are zero) and discard the response. */
    if ((status = BCC_CheckCRC(rxBuf)) != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    if ((status = BCC_CheckMsgCntr(drvConfig, cid, rxBuf)) != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    if (BCC_IS_NULL_RESP(rxBuf))
    {
        return BCC_STATUS_COM_NULL;
    }

    /* Read required data. */
    for (regIdx = 0U; regIdx < regCnt; regIdx++)
    {
        /* Increment address of the register to be read. */
        regAddr++;
        if (regAddr > 0x7FU)
        {
            regAddr = 0x00U;
        }

        BCC_PackFrame(0x0001U, regAddr, cid, BCC_CMD_READ, txBuf);

        /* Send request for data. Required data are returned with the following transfer. */
        status = BCC_MCU_TransferSpi(drvConfig->drvInstance, txBuf, rxBuf);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        /* Check CRC. */
        if ((status = BCC_CheckCRC(rxBuf)) != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        /* Check the Message counter value. */
        if ((status = BCC_CheckMsgCntr(drvConfig, cid, rxBuf)) != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        if (BCC_IS_NULL_RESP(rxBuf))
        {
            return BCC_STATUS_COM_NULL;
        }

        /* Store data. */
        *regVal++ = BCC_GET_MSG_DATA(rxBuf);
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_WriteTpl
 * Description   : This function writes a value to addressed register of the
 *                 BCC device. Intended for TPL mode only.
 *
 *END**************************************************************************/
bcc_status_t BCC_Reg_WriteTpl(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t regAddr, const uint16_t regVal)
{
    uint8_t txBuf[BCC_MSG_SIZE]; /* Transmission buffer. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    if (((uint8_t)cid > drvConfig->devicesCnt) || (regAddr > BCC_MAX_REG_ADDR))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Create frame for writing. */
    BCC_PackFrame(regVal, regAddr, cid, BCC_CMD_WRITE, txBuf);

    status = BCC_MCU_TransferTpl(drvConfig->drvInstance, txBuf, drvConfig->drvData.rxBuf, 1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check the echo frame. */
    return BCC_CheckEchoFrame(txBuf, drvConfig->drvData.rxBuf);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_WriteSpi
 * Description   : This function writes a value to addressed register of the
 *                 BCC device. Intended for SPI mode only.
 *
 *END**************************************************************************/
bcc_status_t BCC_Reg_WriteSpi(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t regAddr, const uint16_t regVal)
{
    uint8_t txBuf[BCC_MSG_SIZE]; /* Transmission buffer. */
    uint8_t rxBuf[BCC_MSG_SIZE]; /* Buffer for receiving. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    if (((uint8_t)cid > drvConfig->devicesCnt) || (regAddr > BCC_MAX_REG_ADDR))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Create frame for writing. */
    BCC_PackFrame(regVal, regAddr, cid, BCC_CMD_WRITE, txBuf);

    status = BCC_MCU_TransferSpi(drvConfig->drvInstance, txBuf, rxBuf);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check CRC. */
    if ((status = BCC_CheckCRC(rxBuf)) != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check message counter. */
    if ((status = BCC_CheckMsgCntr(drvConfig, cid, rxBuf)) != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check whether all field except CRC and message counter are zero. */
    if (BCC_IS_NULL_RESP(rxBuf))
    {
        return BCC_STATUS_COM_NULL;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_WriteGlobalTpl
 * Description   : This function writes a value to addressed register of all
 *                 configured BCC devices in the chain. Intended for TPL mode
 *                 only.
 *
 *END**************************************************************************/
bcc_status_t BCC_Reg_WriteGlobalTpl(bcc_drv_config_t* const drvConfig,
    const uint8_t regAddr, const uint16_t regVal)
{
    uint8_t txBuf[BCC_MSG_SIZE]; /* Transmission buffer. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    /* Check input parameters. */
    if (regAddr > BCC_MAX_REG_ADDR)
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Create frame for writing. */
    BCC_PackFrame(regVal, regAddr, BCC_CID_UNASSIG, BCC_CMD_GLOB_WRITE, txBuf);

    status = BCC_MCU_TransferTpl(drvConfig->drvInstance, txBuf, drvConfig->drvData.rxBuf, 1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check the echo frame. */
    status = BCC_CheckEchoFrame(txBuf, drvConfig->drvData.rxBuf);
    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_SendNopTpl
 * Description   : This function sends a No Operation command (NOP) to the
 *                 BCC device. Intended for TPL mode only.
 *
 *END**************************************************************************/
bcc_status_t BCC_SendNopTpl(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid)
{
    uint8_t txBuf[BCC_MSG_SIZE]; /* Transmission buffer. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || ((uint8_t)cid > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Create frame for writing.
    * Note: Register Data, Register Address and Message counter fields can
    * contain any value. */
    BCC_PackFrame(0x0000U, 0x00U, cid, BCC_CMD_NOOP, txBuf);

    status = BCC_MCU_TransferTpl(drvConfig->drvInstance, txBuf, drvConfig->drvData.rxBuf, 1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check the echo frame. */
    return BCC_CheckEchoFrame(txBuf, drvConfig->drvData.rxBuf);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_SendNopSpi
 * Description   : This function sends a No Operation command (NOP) to the
 *                 BCC device. Intended for SPI mode only.
 *
 *END**************************************************************************/
bcc_status_t BCC_SendNopSpi(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid)
{
    uint8_t txBuf[BCC_MSG_SIZE]; /* Transmission buffer. */
    uint8_t rxBuf[BCC_MSG_SIZE]; /* Buffer for receiving. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || ((uint8_t)cid > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Create frame for writing.
    * Note: Register Data, Register Address and Message counter fields can
    * contain any value. */
    BCC_PackFrame(0x0000U, 0x00U, cid, BCC_CMD_NOOP, txBuf);

    status = BCC_MCU_TransferSpi(drvConfig->drvInstance, txBuf, rxBuf);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check CRC. */
    if ((status = BCC_CheckCRC(rxBuf)) != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check message counter. */
    if ((status = BCC_CheckMsgCntr(drvConfig, cid, rxBuf)) != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check whether all field except CRC and message counter are zero. */
    if (BCC_IS_NULL_RESP(rxBuf))
    {
        return BCC_STATUS_COM_NULL;
    }

    return BCC_STATUS_SUCCESS;
}
