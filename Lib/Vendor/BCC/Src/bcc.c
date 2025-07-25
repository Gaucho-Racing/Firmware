/*!
 * @file bcc.c
 *
 * Battery cell controller SW driver for MC33771C and MC33772C v2.2.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "bcc_communication.h"
extern void write_LED(bool state);
extern void print_lpuart(char* arr);

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Time after VPWR connection for the IC to be ready for initialization
 *  (t_VPWR(READY), max.) in [ms]. */
#define BCC_T_VPWR_READY_MS             5U

/*! @brief RESET de-glitch filter (t_RESETFLT, typ.) in [us]. */
#define BCC_T_RESETFLT_US               100U

/*! @brief CSB wake-up de-glitch filter, low to high transition
 * (CSB_WU_FLT, max.) in [us].
 * MC33771C: Max 80 us
 * MC33772C: Max 65 us */
#define BCC_CSB_WU_FLT_US               80U

/*! @brief Power up duration (t_WAKE-UP, max.) in [us]. */
#define BCC_T_WAKE_UP_US                440U

/*! @brief CSB_TX LOW period in CSB_TX wake-up pulse sequence in [us]. */
#define BCC_WAKE_PULSE_US               25U

/*! @brief Time between wake pulses (t_WAKE_DELAY, typ.) in [us]. */
#define BCC_T_WAKE_DELAY_US             600U

/*! @brief Time the MCU shall wait after sending first wake-up message
 * per 33771C/33772C IC (t_WU_Wait, min.) in [us]. */
#define BCC_T_WU_WAIT_US                750U

/*! @brief EN LOW to HIGH transition to INTB verification pulse
 * (t_INTB_PULSE_DELAY, max.) in [us]. */
#define BCC_T_INTB_PULSE_DELAY_US       100U

/*! @brief INTB verification pulse duration (t_INTB_PULSE, typ.) in [us]. */
#define BCC_T_INTB_PULSE_US             100U

/*! @brief SOC to data ready (includes post processing of data, ADC_CFG[AVG]=0)
 * (in [us]), typical value. */
#define BCC_T_EOC_TYP_US                520U

/*! @brief Timeout for SOC to data ready (ADC_CFG[AVG]=0) (in [us]).
 * Note: The typical value is 520 us, the maximal one 546 us. */
#define BCC_T_EOC_TIMEOUT_US            650U

/*! @brief Timeout (in [us]) for BCC device to perform a EEPROM read command
 * via I2C (START + 2*(8b + ACK) + START + 2*(8b + ACK) + STOP) and to clear
 * EEPROM_CTRL_BUSY flag.
 *
 * Note: The time measured for KIT33772CTPLEVB was around 420 us. */
#define BCC_EEPROM_READ_TIMEOUT_US      1000

/*! @brief Timeout (in [us]) for BCC device to perform a EEPROM write command
 * via I2C (START + 3*(8b + ACK) + STOP) and to clear EEPROM_CTRL_BUSY flag.
 *
 * Note: The time measured for KIT33772CTPLEVB was around 310 us. */
#define BCC_EEPROM_WRITE_TIMEOUT_US     800

/*! @brief Maximal address of EEPROM data. */
#define BCC_MAX_EEPROM_ADDR             0x7FU

/*! @brief Maximal MC33771C fuse mirror address for read access. */
#define MC33771C_MAX_FUSE_READ_ADDR     0x1AU

/*! @brief Maximal MC33771C fuse mirror address for read access. */
#define MC33772C_MAX_FUSE_READ_ADDR     0x12U

/*! @brief Maximal MC33771C fuse mirror address for read access. */
#define MC33771C_MAX_FUSE_WRITE_ADDR    0x17U

/*! @brief Maximal MC33771C fuse mirror address for read access. */
#define MC33772C_MAX_FUSE_WRITE_ADDR    0x0FU

/*! @brief Fuse address of Traceability 0 in MC33771C. */
#define MC33771C_FUSE_TR_0_OFFSET       0x18U

/*! @brief Fuse address of Traceability 1 in MC33771C. */
#define MC33771C_FUSE_TR_1_OFFSET       0x19U

/*! @brief Fuse address of Traceability 2 in MC33771C. */
#define MC33771C_FUSE_TR_2_OFFSET       0x1AU

/*! @brief Fuse address of Traceability 0 in MC33772C. */
#define MC33772C_FUSE_TR_0_OFFSET       0x10U

/*! @brief Fuse address of Traceability 1 in MC33772C. */
#define MC33772C_FUSE_TR_1_OFFSET       0x11U

/*! @brief Fuse address of Traceability 2 in MC33772C. */
#define MC33772C_FUSE_TR_2_OFFSET       0x12U

/*! @brief Mask of Traceability 0 data. */
#define BCC_FUSE_TR_0_MASK              0xFFFFU

/*! @brief Mask of Traceability 1 data. */
#define BCC_FUSE_TR_1_MASK              0xFFFFU

/*! @brief Mask of Traceability 2 data. */
#define BCC_FUSE_TR_2_MASK              0x001FU

/*******************************************************************************
 * Constant variables
 ******************************************************************************/

/*! @brief Array containing cell maps for different numbers of cells connected
 * to the MC33771C. */
static const uint16_t s_cellMap33771c[MC33771C_MAX_CELLS + 1] = {
    0x0000,   /* Unsupported number of cells. */
    0x0000,   /* Unsupported number of cells. */
    0x0000,   /* Unsupported number of cells. */
    0x0000,   /* Unsupported number of cells. */
    0x0000,   /* Unsupported number of cells. */
    0x0000,   /* Unsupported number of cells. */
    0x0000,   /* Unsupported number of cells. */
    0x380F,   /* 7 cells. */
    0x3C0F,   /* 8 cells. */
    0x3E0F,   /* 9 cells. */
    0x3F0F,   /* 10 cells. */
    0x3F8F,   /* 11 cells. */
    0x3FCF,   /* 12 cells. */
    0x3FEF,   /* 13 cells. */
    0x3FFF    /* 14 cells. */
};

/*! @brief Array containing cell maps for different numbers of cells connected
 * to the MC33772C. */
static const uint16_t s_cellMap33772c[MC33772C_MAX_CELLS + 1] = {
    0x0000,   /* Unsupported number of cells. */
    0x0000,   /* Unsupported number of cells. */
    0x0000,   /* Unsupported number of cells. */
    0x0023,   /* 3 cells. */
    0x0033,   /* 4 cells. */
    0x003B,   /* 5 cells. */
    0x003F    /* 6 cells. */
};

/*******************************************************************************
 * Prototypes of internal functions
 ******************************************************************************/

/*!
 * @brief This function does a transition of CSB from low to high.
 *
 * CSB -> 0 for 80 us
 * CSB -> 1 for 440 us
 *
 * @param drvConfig Pointer to driver instance configuration.
 */
static inline void BCC_WakeUpPatternSpi(const bcc_drv_config_t* const drvConfig);

/*!
 * @brief This function does two consecutive transitions of CSB_TX from low to
 * high.
 *
 * CSB_TX -> 0 for 25 us
 * CSB_TX -> 1 for 600 us
 * CSB_TX -> 0 for 25 us
 * CSB_TX -> 1 for 750*numberOfDevices us
 *
 * @param drvConfig Pointer to driver instance configuration.
 */
static inline void BCC_WakeUpPatternTpl(const bcc_drv_config_t* const drvConfig);

/*!
 * @brief This function assigns CID to a BCC device that has CID equal to zero.
 * It also stores the MsgCntr value for appropriate CID, terminates the RDTX_OUT
 * of the last node if loop-back is not required (in the TPL mode) and checks 
 * if the node with newly set CID replies.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster ID to be set to the BCC device with CID=0.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_AssignCid(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid);

/*!
 * @brief This function wakes device(s) up, resets them (if needed), assigns
 * CIDs and checks the communication.
 *
 * @param drvConfig Pointer to driver instance configuration.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_InitDevices(bcc_drv_config_t* const drvConfig);

/*!
 * @brief This function configures selected GPIO/AN pin as analog input, digital
 * input or digital output by writing the GPIO_CFG1[GPIOx_CFG] bit field.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param gpioSel   Index of pin to be configured. Index starts at 0 (GPIO0).
 * @param mode      Pin mode. The only accepted enum items are:
 *                  BCC_PIN_ANALOG_IN_RATIO, BCC_PIN_ANALOG_IN_ABS,
 *                  BCC_PIN_DIGITAL_IN and BCC_PIN_DIGITAL_OUT.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_SetGpioCfg(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t gpioSel, const bcc_pin_mode_t mode);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_WakeUpPatternSpi
 * Description   : This function does a transition of CSB from low to high.
 *
 *END**************************************************************************/
static inline void BCC_WakeUpPatternSpi(const bcc_drv_config_t* const drvConfig)
{
    /* A transition of CSB from low to high. */
    /* CSB low for 80 us (65 us would be sufficient for MC33772C). */
    BCC_MCU_WriteCsbPin(drvConfig->drvInstance, 0);
    BCC_MCU_WaitUs(BCC_CSB_WU_FLT_US);

    /* CSB high for 440 us. */
    BCC_MCU_WriteCsbPin(drvConfig->drvInstance, 1);
    BCC_MCU_WaitUs(BCC_T_WAKE_UP_US);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_WakeUpPatternTpl
 * Description   : This function does two consecutive transitions of CSB_TX from
 *                 low to high.
 *
 *END**************************************************************************/
static inline void BCC_WakeUpPatternTpl(const bcc_drv_config_t* const drvConfig)
{
    /* CSB_TX low for 25 us. */
    BCC_MCU_WriteCsbPin(drvConfig->drvInstance, 0);
    BCC_MCU_WaitUs(BCC_WAKE_PULSE_US);

    /* CSB_TX high for 600 us. */
    BCC_MCU_WriteCsbPin(drvConfig->drvInstance, 1);
    BCC_MCU_WaitUs(BCC_T_WAKE_DELAY_US);

    /* CSB_TX low for 25 us. */
    BCC_MCU_WriteCsbPin(drvConfig->drvInstance, 0);
    BCC_MCU_WaitUs(BCC_WAKE_PULSE_US);

    /* CSB_TX high. */
    BCC_MCU_WriteCsbPin(drvConfig->drvInstance, 1);
    /* Time to switch Sleep mode to normal mode after TPL bus wake-up. */
    BCC_MCU_WaitUs(BCC_T_WU_WAIT_US * drvConfig->devicesCnt);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_AssignCid
 * Description   : This function assigns CID to a BCC device that has CID equal
 *                 to zero.
 *
 *END**************************************************************************/
static bcc_status_t BCC_AssignCid(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid)
{
    uint16_t writeVal, readVal;
    bcc_status_t status;

    /* Check if unassigned node replies. This is the first reading after device
     * reset. */
    /* Note: In SPI communication mode, the device responds with all bit filed
     * set to zero except message counter and the correct CRC to the very first
     * MCU <-> MC33771C/772C message. */
    status = BCC_Reg_Read(drvConfig, BCC_CID_UNASSIG, MC33771C_INIT_OFFSET, 1U, &readVal);
    if ((status != BCC_STATUS_SUCCESS) && (status != BCC_STATUS_COM_NULL))
    {
        return status;
    }

    /* Assign CID;
     * Terminate RDTX_OUT of the last node in TPL setup without loop-back.
     * Stop forwarding only for MC33772C in TPL setup with one node and no
     * loop-back. RDTX_OUT should not be terminated in this case. */
    writeVal = MC33771C_INIT_CID(cid) |
               MC33771C_INIT_RDTX_IN(MC33771C_INIT_RDTX_IN_DISABLED_ENUM_VAL);
    if ((drvConfig->commMode == BCC_MODE_TPL) &&
        (drvConfig->devicesCnt == 1U) &&
        (!drvConfig->loopBack) &&
        (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33772C))
    {
        writeVal |= MC33772C_INIT_TPL2_TX_TERM(MC33772C_INIT_TPL2_TX_TERM_DISABLED_ENUM_VAL) |
                    MC33772C_INIT_BUS_FW(MC33772C_INIT_BUS_FW_DISABLED_ENUM_VAL);
    }
    else if ((drvConfig->commMode == BCC_MODE_TPL) &&
             ((uint8_t)cid == drvConfig->devicesCnt) &&
             (!drvConfig->loopBack))
    {
        writeVal |= MC33771C_INIT_RDTX_OUT(MC33771C_INIT_RDTX_OUT_ENABLED_ENUM_VAL);

        if (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33772C)
        {
            writeVal |= MC33772C_INIT_BUS_FW(MC33772C_INIT_BUS_FW_ENABLED_ENUM_VAL);
        }
    }
    else
    {
        writeVal |= MC33771C_INIT_RDTX_OUT(MC33771C_INIT_RDTX_OUT_DISABLED_ENUM_VAL);

        if (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33772C)
        {
            writeVal |= MC33772C_INIT_BUS_FW(MC33772C_INIT_BUS_FW_ENABLED_ENUM_VAL);
        }
    }
    status = BCC_Reg_Write(drvConfig, BCC_CID_UNASSIG, MC33771C_INIT_OFFSET, writeVal); 
    if (status == BCC_STATUS_SUCCESS)
    {
        /* Store the last received message counter value from device with CID=0
         * into drvConfig for appropriate (newly assigned) CID.
         * Note: In TPL mode, a response is generated only for read commands,
         * i.e. message counter is incremented only by read commands. In SPI mode,
         * message counter was incremented also by the write command! */
        drvConfig->drvData.msgCntr[(uint8_t)cid] = drvConfig->drvData.msgCntr[0];

        /* Check if assigned node replies. */
        status = BCC_Reg_Read(drvConfig, cid, MC33771C_INIT_OFFSET, 1U, &readVal);

        /* Check the written data. */
        if ((status == BCC_STATUS_SUCCESS) && (writeVal != readVal))
        {
            print_lpuart("bcc.c line 341\n");
            status = BCC_STATUS_SPI_FAIL;
        }
    }
    if (status != BCC_STATUS_SUCCESS)
    {
        /* Wait and try to assign CID once again. */
        BCC_MCU_WaitUs(750U);

        status = BCC_Reg_Write(drvConfig, BCC_CID_UNASSIG, MC33771C_INIT_OFFSET, writeVal);
        if (status == BCC_STATUS_SUCCESS)
        {
            /* Store the last message counter value into drvConfig for appropriate CID.
             * Note: In TPL mode, a response is generated only for read commands. i.e. message
             * counter is incremented only by them. In SPI mode, message counter is
             * incremented by all types of commands! */
            drvConfig->drvData.msgCntr[(uint8_t)cid] = drvConfig->drvData.msgCntr[0];

            status = BCC_Reg_Read(drvConfig, cid, MC33771C_INIT_OFFSET, 1U, &readVal);

            /* Check the written data. */
            if ((status == BCC_STATUS_SUCCESS) && (writeVal != readVal))
            {
                print_lpuart("bcc.c line 364\n");
                status = BCC_STATUS_SPI_FAIL;
            }
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_InitDevices
 * Description   : This function wakes device(s) up, resets them (if needed),
 *                 assigns CIDs and checks the communication.
 *
 *END**************************************************************************/
static bcc_status_t BCC_InitDevices(bcc_drv_config_t* const drvConfig)
{
    uint8_t cid;
    bcc_status_t status;

    /* Wake-up all configured devices (in case they are in SLEEP mode) or
     * move the first device (device closest to MC33664) from IDLE mode to
     * NORMAL mode (in case devices are in IDLE mode). */
    BCC_WakeUp(drvConfig);

    /* Reset all configured devices (in case they are already initialized).
     * If the devices are not initialized (CID is equal to 000000b), a write
     * command is sent via communication interface, but the software reset is
     * not performed as only INIT register of uninitialized devices can be
     * written by the pack controller. */
    (void)BCC_SoftwareReset(drvConfig, (drvConfig->commMode == BCC_MODE_TPL) ? BCC_CID_UNASSIG : BCC_CID_DEV1);
    /* Wait for 5 ms - for the IC to be ready for initialization. */
    BCC_MCU_WaitMs(BCC_T_VPWR_READY_MS);

    /* Assign CID to the first node and terminate its RDTX_OUT if only one
     * device is utilised and if loop-back is not required. */
    status = BCC_AssignCid(drvConfig, BCC_CID_DEV1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Init the rest of devices. */
    for (cid = 2U; cid <= drvConfig->devicesCnt; cid++)
    {
        BCC_MCU_WaitMs(2U);

        /* Move the following device from IDLE to NORMAL mode (in case the
         * devices are in IDLE mode).
         * Note that the WAKE-UP sequence is recognised as two wrong SPI
         * transfers in devices which are already in the NORMAL mode. That will
         * increase their COM_STATUS[COM_ERR_COUNT]. */
        BCC_WakeUpPatternTpl(drvConfig);

        status = BCC_AssignCid(drvConfig, (bcc_cid_t)cid);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }
    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_GPIO_SetMode
 * Description   : This function configures selected GPIO/AN pin as analog
 *                 input, digital input or digital output by writing the
 *                 GPIO_CFG1[GPIOx_CFG] bit field.
 *
 *END**************************************************************************/
static bcc_status_t BCC_SetGpioCfg(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t gpioSel, const bcc_pin_mode_t mode)
{
    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
            (gpioSel >= BCC_GPIO_INPUT_CNT) || (mode > BCC_PIN_DIGITAL_OUT))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Update the content of GPIO_CFG1 register. */
    return BCC_Reg_Update(drvConfig, cid,
                    MC33771C_GPIO_CFG1_OFFSET,
                    (uint16_t)(MC33771C_GPIO_CFG1_GPIO0_CFG_MASK << (gpioSel * 2U)),
                    (uint16_t)(((uint16_t)mode) << (gpioSel * 2U)));
}

/******************************************************************************
 * API
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Init
 * Description   : This function initializes the battery cell controller
 *                 device(s), assigns CID and initializes internal driver data.
 *
 *END**************************************************************************/
bcc_status_t BCC_Init(bcc_drv_config_t* const drvConfig)
{
    uint8_t dev;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    /* Check the drvConfig structure and initialize driver variables. */
    if ((drvConfig->commMode != BCC_MODE_SPI) &&
        (drvConfig->commMode != BCC_MODE_TPL))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if ((drvConfig->devicesCnt == 0) ||
        (drvConfig->devicesCnt > ((drvConfig->commMode == BCC_MODE_SPI) ?
                BCC_DEVICE_CNT_MAX_SPI : BCC_DEVICE_CNT_MAX_TPL)))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    for (dev = 0; dev < drvConfig->devicesCnt; dev++)
    {
        drvConfig->drvData.msgCntr[dev] = 0U;
        if (drvConfig->device[dev] == BCC_DEVICE_MC33771C)
        {
            if (!BCC_IS_IN_RANGE(drvConfig->cellCnt[dev], MC33771C_MIN_CELLS, MC33771C_MAX_CELLS))
            {
                return BCC_STATUS_PARAM_RANGE;
            }
            drvConfig->drvData.cellMap[dev] = s_cellMap33771c[drvConfig->cellCnt[dev]];
        }
        else if (drvConfig->device[dev] == BCC_DEVICE_MC33772C)
        {
            if (!BCC_IS_IN_RANGE(drvConfig->cellCnt[dev], MC33772C_MIN_CELLS, MC33772C_MAX_CELLS))
            {
                return BCC_STATUS_PARAM_RANGE;
            }
            drvConfig->drvData.cellMap[dev] = s_cellMap33772c[drvConfig->cellCnt[dev]];
        }
        else
        {
            return BCC_STATUS_PARAM_RANGE;
        }
    }
    drvConfig->drvData.msgCntr[drvConfig->devicesCnt] = 0U;

    /* Set RESET pin inactive (RESET -> 0). */
    BCC_MCU_WriteRstPin(drvConfig->drvInstance, 0);

    /* Enable MC33664 device in TPL mode. */
    if (drvConfig->commMode == BCC_MODE_TPL)
    {
        if ((status = BCC_TPL_Enable(drvConfig->drvInstance)) != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    /* Wake-up BCCs (in case of idle/sleep mode), reset them, assign CID and
     * check communication with configured devices. */
    return BCC_InitDevices(drvConfig);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_SendNop
 * Description   : This function sends No Operation command the to BCC device.
 *
 *END**************************************************************************/
bcc_status_t BCC_SendNop(bcc_drv_config_t* const drvConfig, const bcc_cid_t cid)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if (drvConfig->commMode == BCC_MODE_SPI)
    {
        return BCC_SendNopSpi(drvConfig, cid);
    }
    else
    {
        return BCC_SendNopTpl(drvConfig, cid);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Sleep
 * Description   : This function sets sleep mode to all battery cell controller
 *                 devices.
 *
 *END**************************************************************************/
bcc_status_t BCC_Sleep(bcc_drv_config_t* const drvConfig)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if (drvConfig->commMode == BCC_MODE_SPI)
    {
        return BCC_Reg_Write(drvConfig, BCC_CID_DEV1, MC33771C_SYS_CFG_GLOBAL_OFFSET,
                MC33771C_SYS_CFG_GLOBAL_GO2SLEEP(MC33771C_SYS_CFG_GLOBAL_GO2SLEEP_ENABLED_ENUM_VAL));
    }
    else
    {
        return BCC_Reg_WriteGlobal(drvConfig, MC33771C_SYS_CFG_GLOBAL_OFFSET,
                MC33771C_SYS_CFG_GLOBAL_GO2SLEEP(MC33771C_SYS_CFG_GLOBAL_GO2SLEEP_ENABLED_ENUM_VAL));
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_WakeUp
 * Description   : This function sets normal mode to all battery cell controller
 *                 devices.
 *
 *END**************************************************************************/
void BCC_WakeUp(const bcc_drv_config_t* const drvConfig)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if (drvConfig->commMode == BCC_MODE_SPI)
    {
        BCC_WakeUpPatternSpi(drvConfig);
    }
    else
    {
        BCC_WakeUpPatternTpl(drvConfig);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_SoftwareReset
 * Description   : This function resets BCC device using software reset. It
 *                 enters reset via SPI or TPL interface.
 *
 *END**************************************************************************/
bcc_status_t BCC_SoftwareReset(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if ((((uint8_t)cid) > drvConfig->devicesCnt) ||
            ((cid == BCC_CID_UNASSIG) && (drvConfig->commMode == BCC_MODE_SPI)))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Note: it is not necessary to read content of SYS_CFG1 register and to
     * change the SOFT_RST bit only, because SYS_CFG1 will be set to POR value
     * after the reset anyway. */
    if (cid == BCC_CID_UNASSIG)
    {
        /* TPL Global reset command. */
        return BCC_Reg_WriteGlobal(drvConfig, MC33771C_SYS_CFG1_OFFSET,
                                   MC33771C_SYS_CFG1_SOFT_RST(MC33771C_SYS_CFG1_SOFT_RST_ACTIVE_ENUM_VAL));
    }
    else
    {
        return BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET,
                             MC33771C_SYS_CFG1_SOFT_RST(MC33771C_SYS_CFG1_SOFT_RST_ACTIVE_ENUM_VAL));
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_HardwareReset
 * Description   : This function resets BCC device using RESET pin.
 *
 *END**************************************************************************/
void BCC_HardwareReset(const bcc_drv_config_t* const drvConfig)
{
    BCC_MCU_Assert(drvConfig != NULL);

    BCC_MCU_WriteRstPin(drvConfig->drvInstance, 1);
    /* Wait at least t_RESETFLT (100 us). */
    BCC_MCU_WaitUs(BCC_T_RESETFLT_US);
    BCC_MCU_WriteRstPin(drvConfig->drvInstance, 0);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_TPL_Enable
 * Description   : This function enables MC33664 TPL device. It uses EN and
 *                 INTB pins. Intended for TPL mode only!
 *
 *END**************************************************************************/
bcc_status_t BCC_TPL_Enable(const uint8_t drvInstance)
{
    int32_t timeout;

    /* Set normal state (transition from low to high). */
    BCC_MCU_WriteEnPin(drvInstance, 0);
    /* Wait at least 100 us. */
    BCC_MCU_WaitUs(150);
    BCC_MCU_WriteEnPin(drvInstance, 1);

    /* Note: MC33664 has time t_Ready/t_INTB_PULSE_DELAY (max. 100 us) to take effect.
     * Wait for INTB transition from high to low (max. 100 us). */
    timeout = BCC_T_INTB_PULSE_DELAY_US;
    while ((BCC_MCU_ReadIntbPin(drvInstance) > 0) && (timeout > 0))
    {
        timeout -= 5;
        BCC_MCU_WaitUs(5U);
    }
    if ((timeout <= 0) && (BCC_MCU_ReadIntbPin(drvInstance) > 0))
    {
        return BCC_STATUS_COM_TIMEOUT;
    }

    /* Wait for INTB transition from low to high (typ. 100 us).
     * Wait for at most 200 us. */
    timeout = BCC_T_INTB_PULSE_US * 2;
    while ((BCC_MCU_ReadIntbPin(drvInstance) == 0) && (timeout > 0))
    {
        timeout -= 10;
        BCC_MCU_WaitUs(10U);
    }
    if ((timeout <= 0) && (BCC_MCU_ReadIntbPin(drvInstance) == 0))
    {
        return BCC_STATUS_COM_TIMEOUT;
    }

    /* Now the device should be in normal mode (i.e. after INTB low to high
    * transition). For sure wait for 150 us. */
    BCC_MCU_WaitUs(150U);

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_TPL_Disable
 * Description   : This function puts MC33664 device into the sleep mode.
 *                 Intended for TPL mode only!
 *
 *END**************************************************************************/
void BCC_TPL_Disable(const uint8_t drvInstance)
{
    BCC_MCU_WriteEnPin(drvInstance, 0);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_Read
 * Description   : This function reads a value from addressed register (or
 *                 desired number of registers) of selected battery cell
 *                 controller device.
 *
 *END**************************************************************************/
bcc_status_t BCC_Reg_Read(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t regAddr, const uint8_t regCnt,
    uint16_t* regVal)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if (drvConfig->commMode == BCC_MODE_SPI)
    {
        return BCC_Reg_ReadSpi(drvConfig, cid, regAddr, regCnt, regVal);
    }
    else
    {
        return BCC_Reg_ReadTpl(drvConfig, cid, regAddr, regCnt, regVal);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_Write
 * Description   : This function writes a value to addressed register of
 *                 selected battery cell controller device.
 *
 *END**************************************************************************/
bcc_status_t BCC_Reg_Write(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t regAddr, const uint16_t regVal)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if (drvConfig->commMode == BCC_MODE_SPI)
    {
        return BCC_Reg_WriteSpi(drvConfig, cid, regAddr, regVal);
    }
    else
    {
        return BCC_Reg_WriteTpl(drvConfig, cid, regAddr, regVal);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_WriteGlobal
 * Description   : This function writes a value to addressed register of all
 *                 configured BCC devices. Intended for TPL mode only.
 *
 *END**************************************************************************/
bcc_status_t BCC_Reg_WriteGlobal(bcc_drv_config_t* const drvConfig,
     const uint8_t regAddr, const uint16_t regVal)
{
    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(drvConfig->commMode == BCC_MODE_TPL);
    return BCC_Reg_WriteGlobalTpl(drvConfig, regAddr, regVal);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Reg_Update
 * Description   : This function updates content of a selected register; affects
 *                 bits specified by a bit mask only.
 *
 *END**************************************************************************/
bcc_status_t BCC_Reg_Update(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t regAddr, const uint16_t regMask,
    const uint16_t regVal)
{
    uint16_t regValTemp;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    if (((uint8_t)cid) > drvConfig->devicesCnt)
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Read(drvConfig, cid, regAddr, 1U, &regValTemp);
    if (status != BCC_STATUS_SUCCESS)
    {
        print_lpuart("BCC_Reg_Update: BCC_Reg_Read: Read failed.\n");
        return status;
    }

    /* Update register value. */
    regValTemp = regValTemp & ~(regMask);
    regValTemp = regValTemp | (regVal & regMask);
    return BCC_Reg_Write(drvConfig, cid, regAddr, regValTemp);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_StartConversion
 * Description   : This function starts ADC conversion in selected BCC device.
 *                 It sets number of samples to be averaged and Start of
 *                 Conversion bit in ADC_CFG register.
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_StartConversion(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_avg_t avg)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
            (avg > BCC_AVG_256))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    return BCC_Reg_Update(drvConfig, cid, MC33771C_ADC_CFG_OFFSET,
                          MC33771C_ADC_CFG_SOC_MASK | MC33771C_ADC_CFG_AVG_MASK,
                          MC33771C_ADC_CFG_SOC(MC33771C_ADC_CFG_SOC_ENABLED_ENUM_VAL) |
                          MC33771C_ADC_CFG_AVG(avg));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_StartConversionGlobal
 * Description   : This function starts ADC conversion for all devices in TPL
 *                 chain. It uses a Global Write command to set ADC_CFG
 *                 register. Intended for TPL mode only!
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_StartConversionGlobal(bcc_drv_config_t* const drvConfig,
    uint16_t adcCfgValue)
{
    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(drvConfig->commMode == BCC_MODE_TPL);

    /* Set Start of Conversion bit in case it is not. */
    adcCfgValue |= MC33771C_ADC_CFG_SOC(MC33771C_ADC_CFG_SOC_ENABLED_ENUM_VAL);

    return BCC_Reg_WriteGlobal(drvConfig, MC33771C_ADC_CFG_OFFSET, adcCfgValue);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_IsConverting
 * Description   : This function checks status of conversion defined by End of
 *                 Conversion bit in ADC_CFG register.
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_IsConverting(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bool* const completed)
{
    uint16_t adcCfgVal;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(completed != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Read(drvConfig, cid, MC33771C_ADC_CFG_OFFSET, 1U, &adcCfgVal);

    *(completed) = ((adcCfgVal & MC33771C_ADC_CFG_EOC_N_MASK) ==
            MC33771C_ADC_CFG_EOC_N(MC33771C_ADC_CFG_EOC_N_COMPLETED_ENUM_VAL));

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_StartAndWait
 * Description   : This function starts an on-demand conversion in selected BCC
 *                 device and waits for completion.
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_StartAndWait(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_avg_t avg)
{
    bool complete;           /* Conversion complete flag. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
            (avg > BCC_AVG_256))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Meas_StartConversion(drvConfig, cid, avg);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Wait for at least 520 us (16-bit conversion) before polling bit EOC_N
     * to avoid any traffic on the communication bus during conversion. */
    BCC_MCU_WaitUs(((uint32_t)BCC_T_EOC_TYP_US) << ((uint8_t)avg));

    status = BCC_MCU_StartTimeout(
            (((uint32_t)BCC_T_EOC_TIMEOUT_US) << ((uint8_t)avg)) -
            (((uint32_t)BCC_T_EOC_TYP_US) << ((uint8_t)avg)));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    do
    {
        status = BCC_Meas_IsConverting(drvConfig, cid, &complete);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    } while ((!complete) && (!BCC_MCU_TimeoutExpired()));

    /* Check once more after timeout expiration because the read command takes
     * several tens/hundreds of microseconds (depends on user code efficiency)
     * and the last read command could be done relatively long before the
     * timeout expiration. */
    if (!complete)
    {
        status = BCC_Meas_IsConverting(drvConfig, cid, &complete);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    return (complete) ? BCC_STATUS_SUCCESS : BCC_STATUS_COM_TIMEOUT;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetRawValues
 * Description   : This function reads the measurement registers and returns raw
 *                 values.
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetRawValues(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint16_t* const measurements)
{
    bcc_status_t status;
    uint8_t i;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(measurements != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Read all the measurement registers.
     * Note: the order and number of registers conforms to the order of measured
     * values in Measurements array, see enumeration bcc_measurements_t. */
    if (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33771C)
    {
        status = BCC_Reg_Read(drvConfig, cid, MC33771C_CC_NB_SAMPLES_OFFSET,
                             BCC_MEAS_CNT, measurements);
    }
    else
    {
        status = BCC_Reg_Read(drvConfig, cid, MC33772C_CC_NB_SAMPLES_OFFSET,
                             (MC33772C_MEAS_STACK_OFFSET - MC33772C_CC_NB_SAMPLES_OFFSET) + 1, measurements);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        /* Skip the reserved registers to speed-up this function. */
        measurements[BCC_MSR_CELL_VOLT14] = 0x0000;
        measurements[BCC_MSR_CELL_VOLT13] = 0x0000;
        measurements[BCC_MSR_CELL_VOLT12] = 0x0000;
        measurements[BCC_MSR_CELL_VOLT11] = 0x0000;
        measurements[BCC_MSR_CELL_VOLT10] = 0x0000;
        measurements[BCC_MSR_CELL_VOLT9] = 0x0000;
        measurements[BCC_MSR_CELL_VOLT8] = 0x0000;
        measurements[BCC_MSR_CELL_VOLT7] = 0x0000;

        status = BCC_Reg_Read(drvConfig, cid, MC33772C_MEAS_CELL6_OFFSET,
                             (MC33772C_MEAS_VBG_DIAG_ADC1B_OFFSET - MC33772C_MEAS_CELL6_OFFSET) + 1,
                             (uint16_t *)(measurements + ((uint8_t)BCC_MSR_CELL_VOLT6)));
    }

    /* Mask the read registers.
     * Note: Nothing to mask in CC_NB_SAMPLES, COULOMB_CNT1 and COULOMB_CNT2
     * registers. */
    measurements[BCC_MSR_ISENSE1] &= MC33771C_MEAS_ISENSE1_MEAS_I_MSB_MASK;
    measurements[BCC_MSR_ISENSE2] &= MC33771C_MEAS_ISENSE2_MEAS_I_LSB_MASK;

    for (i = (uint8_t)BCC_MSR_STACK_VOLT; i < BCC_MEAS_CNT; i++)
    {
        measurements[i] &= MC33771C_MEAS_STACK_MEAS_STACK_MASK;
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetCoulombCounter
 * Description   : This function reads the Coulomb counter registers.
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetCoulombCounter(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_cc_data_t* const cc)
{
    bcc_status_t status;
    uint16_t readVal[3];

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(cc != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Read(drvConfig, cid, MC33771C_CC_NB_SAMPLES_OFFSET, 3U, readVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    cc->nbSamples = readVal[0];
    cc->ccAccumulator = BCC_GET_COULOMB_CNT(readVal[1], readVal[2]);

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetIsenseVoltage
 * Description   : This function reads the ISENSE measurement and converts it to
 *                 [uV].
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetIsenseVoltage(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, int32_t* const isenseVolt)
{
    bcc_status_t status;
    uint16_t readVal[2];

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(isenseVolt != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Read(drvConfig, cid, MC33771C_MEAS_ISENSE1_OFFSET, 2U, readVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    if ((readVal[0] & readVal[1] & MC33771C_MEAS_ISENSE1_DATA_RDY_MASK) == 0U)
    {
        return BCC_STATUS_DATA_RDY;
    }

    *isenseVolt = BCC_GET_ISENSE_VOLT(readVal[0], readVal[1]);

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetStackVoltage
 * Description   : This function reads the stack measurement and converts it to
 *                 [uV].
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetStackVoltage(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint32_t* const stackVolt)
{
    bcc_status_t status;
    uint16_t readVal;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(stackVolt != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Read(drvConfig, cid, MC33771C_MEAS_STACK_OFFSET, 1U, &readVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    if ((readVal & MC33771C_MEAS_STACK_DATA_RDY_MASK) == 0U)
    {
        return BCC_STATUS_DATA_RDY;
    }

    *stackVolt = BCC_GET_STACK_VOLT(readVal);

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetCellVoltages
 * Description   : This function reads the cell measurements and converts them
 *                 to [uV].
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetCellVoltages(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint32_t* const cellVolt)
{
    bcc_status_t status;
    uint16_t readVal[BCC_MAX_CELLS];
    uint8_t i, cellCnt;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(cellVolt != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    cellCnt = BCC_MAX_CELLS_DEV(drvConfig->device[(uint8_t)cid - 1]);

    /* Read the measurement registers. */
    status = BCC_Reg_Read(drvConfig, cid,
                          (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33771C) ?
                               MC33771C_MEAS_CELL14_OFFSET : MC33771C_MEAS_CELL6_OFFSET,
                          cellCnt, readVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Convert measurements to [uV], change the cell order and check the data-ready flag. */
    for (i = 0; i < cellCnt; i++)
    {
        cellVolt[cellCnt - (i + 1)] = BCC_GET_VOLT(readVal[i]);
        readVal[0] &= readVal[i];
    }

    if ((readVal[0] & MC33771C_MEAS_CELL1_DATA_RDY_MASK) == 0U)
    {
        return BCC_STATUS_DATA_RDY;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetCellVoltage
 * Description   : This function reads the voltage measurement of a selected
 *                 cell and converts it to [uV].
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetCellVoltage(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint8_t cellIndex, uint32_t* const cellVolt)
{
    bcc_status_t status;
    uint16_t readVal;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(cellVolt != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
        (cellIndex >= BCC_MAX_CELLS_DEV(drvConfig->device[(uint8_t)cid - 1])))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Read(drvConfig, cid, MC33771C_MEAS_CELL1_OFFSET - cellIndex, 1U, &readVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    if ((readVal & MC33771C_MEAS_CELL1_DATA_RDY_MASK) == 0U)
    {
        return BCC_STATUS_DATA_RDY;
    }

    *cellVolt = BCC_GET_VOLT(readVal);

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetAnVoltages
 * Description   : This function reads the voltage measurement for all ANx
 *                 and converts them to [uV]. Intended for ANx configured for
 *                 absolute measurements only!
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetAnVoltages(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint32_t* const anVolt)
{
    bcc_status_t status;
    uint16_t readVal[BCC_GPIO_INPUT_CNT];
    uint8_t i;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(anVolt != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Read the measurement registers. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_MEAS_AN6_OFFSET,
                          BCC_GPIO_INPUT_CNT, readVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Convert measurements to [uV] and check the data-ready flag. */
    for (i = 0; i < BCC_GPIO_INPUT_CNT; i++)
    {
        anVolt[i] = BCC_GET_VOLT(readVal[i]);
        readVal[0] &= readVal[i];
    }

    if ((readVal[0] & MC33771C_MEAS_AN0_DATA_RDY_MASK) == 0U)
    {
        return BCC_STATUS_DATA_RDY;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetAnVoltage
 * Description   : This function reads the voltage measurement of a selected
 *                 ANx and converts it to [uV]. Intended for ANx configured for
 *                 absolute measurements only!
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetAnVoltage(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint8_t anIndex, uint32_t* const anVolt)
{
    bcc_status_t status;
    uint16_t readVal;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(anVolt != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
        (anIndex >= BCC_GPIO_INPUT_CNT))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Read(drvConfig, cid, MC33771C_MEAS_AN0_OFFSET - anIndex, 1U, &readVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    if ((readVal & MC33771C_MEAS_AN0_DATA_RDY_MASK) == 0U)
    {
        return BCC_STATUS_DATA_RDY;
    }

    *anVolt = BCC_GET_VOLT(readVal);

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Meas_GetIcTemp
 * Description   : This function reads the BCC temperature and converts it to
 *                 the selected unit.
 *
 *END**************************************************************************/
bcc_status_t BCC_Meas_GetIcTemperature(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_temp_unit_t unit, int16_t* const icTemp)
{
    bcc_status_t status;
    uint16_t readVal;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(icTemp != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
            (unit > BCC_TEMP_FAHRENHEIT))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Read(drvConfig, cid, MC33771C_MEAS_IC_TEMP_OFFSET, 1U, &readVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    if ((readVal & MC33771C_MEAS_IC_TEMP_DATA_RDY_MASK) == 0U)
    {
        return BCC_STATUS_DATA_RDY;
    }

    if (unit == BCC_TEMP_CELSIUS)
    {
        *icTemp = BCC_GET_IC_TEMP_C(readVal);
    }
    else if (unit == BCC_TEMP_FAHRENHEIT)
    {
        *icTemp = BCC_GET_IC_TEMP_F(readVal);
    }
    else
    {
        *icTemp = BCC_GET_IC_TEMP_K(readVal);
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Fault_GetStatus
 * Description   : This function reads the fault status registers of the BCC
 *                 device.
 *
 *END**************************************************************************/
bcc_status_t BCC_Fault_GetStatus(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint16_t* const fltStatus)
{
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(fltStatus != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Read CELL_OV_FLT and CELL_UV_FLT. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_CELL_OV_FLT_OFFSET, 2U, &fltStatus[BCC_FS_CELL_OV]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Read CB_OPEN_FLT, CB_SHORT_FLT. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_CB_OPEN_FLT_OFFSET, 2U, &fltStatus[BCC_FS_CB_OPEN]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Read GPIO_STS, AN_OT_UT_FLT, GPIO_SHORT_Anx_OPEN_STS. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_GPIO_STS_OFFSET, 3U, &fltStatus[BCC_FS_GPIO_STATUS]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Read COM_STATUS, FAULT1_STATUS, FAULT2_STATUS and FAULT3_STATUS. */
    return BCC_Reg_Read(drvConfig, cid, MC33771C_COM_STATUS_OFFSET, 4U, &fltStatus[BCC_FS_COMM]);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Fault_ClearStatus
 * Description   : This function clears selected fault status register.
 *
 *END**************************************************************************/
bcc_status_t BCC_Fault_ClearStatus(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_fault_status_t statSel)
{
    /* This array is intended for conversion of bcc_fault_status_t value to
     * a BCC register address. */
    const uint8_t regAddrMap[BCC_STAT_CNT] = {
        MC33771C_CELL_OV_FLT_OFFSET, MC33771C_CELL_UV_FLT_OFFSET,
        MC33771C_CB_OPEN_FLT_OFFSET, MC33771C_CB_SHORT_FLT_OFFSET,
        MC33771C_GPIO_STS_OFFSET, MC33771C_AN_OT_UT_FLT_OFFSET,
        MC33771C_GPIO_SHORT_ANX_OPEN_STS_OFFSET, MC33771C_COM_STATUS_OFFSET,
        MC33771C_FAULT1_STATUS_OFFSET, MC33771C_FAULT2_STATUS_OFFSET,
        MC33771C_FAULT3_STATUS_OFFSET
    };

    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
            ((uint32_t)statSel >= BCC_STAT_CNT))
    {
        return BCC_STATUS_PARAM_RANGE;
    }
    return BCC_Reg_Write(drvConfig, cid, regAddrMap[statSel], 0x0000U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_GPIO_SetMode
 * Description   : This function sets the mode of one BCC GPIOx/ANx pin.
 *
 *END**************************************************************************/
bcc_status_t BCC_GPIO_SetMode(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t gpioSel, const bcc_pin_mode_t mode)
{
    bcc_status_t status = BCC_STATUS_PARAM_RANGE;

    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
            (gpioSel >= BCC_GPIO_INPUT_CNT))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if ((mode == BCC_PIN_WAKE_UP_IN) && (gpioSel == 0U))
    {
        /* Set GPIO0 to digital input and enable the wake-up capability. */
        status = BCC_SetGpioCfg(drvConfig, cid, 0U, BCC_PIN_DIGITAL_IN);
        if (status == BCC_STATUS_SUCCESS)
        {
            status = BCC_Reg_Update(drvConfig, cid,
                    MC33771C_GPIO_CFG2_OFFSET,
                    MC33771C_GPIO_CFG2_GPIO0_WU_MASK,
                    MC33771C_GPIO_CFG2_GPIO0_WU(MC33771C_GPIO_CFG2_GPIO0_WU_WAKEUP_ENUM_VAL));
        }
    }
    else if ((mode == BCC_PIN_CONVERT_TR_IN) && (gpioSel == 2U))
    {
        /* Set GPIO2 to digital input serving as a conversion trigger. */
        status = BCC_SetGpioCfg(drvConfig, cid, 2U, BCC_PIN_DIGITAL_IN);
        if (status == BCC_STATUS_SUCCESS)
        {
            status = BCC_Reg_Update(drvConfig, cid,
                    MC33771C_GPIO_CFG2_OFFSET,
                    MC33771C_GPIO_CFG2_GPIO2_SOC_MASK,
                    MC33771C_GPIO_CFG2_GPIO2_SOC(MC33771C_GPIO_CFG2_GPIO2_SOC_ADC_TRG_ENABLED_ENUM_VAL));
        }
    }
    else if (mode <= BCC_PIN_DIGITAL_OUT)
    {
        status = BCC_STATUS_SUCCESS;
        if (gpioSel == 0U)
        {
            /* Disable the wake-up capability. */
            status = BCC_Reg_Update(drvConfig, cid,
                    MC33771C_GPIO_CFG2_OFFSET,
                    MC33771C_GPIO_CFG2_GPIO0_WU_MASK,
                    MC33771C_GPIO_CFG2_GPIO0_WU(MC33771C_GPIO_CFG2_GPIO0_WU_NO_WAKEUP_ENUM_VAL));
        }
        else if (gpioSel == 2U)
        {
            /* Disable the conversion trigger. */
            status = BCC_Reg_Update(drvConfig, cid,
                    MC33771C_GPIO_CFG2_OFFSET,
                    MC33771C_GPIO_CFG2_GPIO2_SOC_MASK,
                    MC33771C_GPIO_CFG2_GPIO2_SOC(MC33771C_GPIO_CFG2_GPIO2_SOC_ADC_TRG_DISABLED_ENUM_VAL));
        }

        if (status == BCC_STATUS_SUCCESS)
        {
            status = BCC_SetGpioCfg(drvConfig, cid, gpioSel, mode);
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_GPIO_ReadPin
 * Description   : This function reads a value of one BCC GPIO pin.
 *
 *END**************************************************************************/
bcc_status_t BCC_GPIO_ReadPin(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t gpioSel, bool* const val)
{
    bcc_status_t status;
    uint16_t gpioStsVal;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(val != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
            (gpioSel >= BCC_GPIO_INPUT_CNT))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Read and update content of GPIO_CFG2 register. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_GPIO_STS_OFFSET, 1U, &gpioStsVal);
    *val = (gpioStsVal & (1U << gpioSel)) > 0U;

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_GPIO_SetOutput
 * Description   : This function sets output value of one BCC GPIO pin.
 *
 *END**************************************************************************/
bcc_status_t BCC_GPIO_SetOutput(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t gpioSel, const bool val)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt) ||
            (gpioSel >= BCC_GPIO_INPUT_CNT))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Update the content of GPIO_CFG2 register. */
    return BCC_Reg_Update(drvConfig, cid, MC33771C_GPIO_CFG2_OFFSET,
                          (uint16_t)(1U << gpioSel),
                          (uint16_t)((val ? 1U : 0U) << gpioSel));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_CB_Enable
 * Description   : This function enables or disables the cell balancing via
 *                 SYS_CFG1[CB_DRVEN] bit.
 *
 *END**************************************************************************/
bcc_status_t BCC_CB_Enable(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bool enable)
{
    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    return BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET, MC33771C_SYS_CFG1_CB_DRVEN_MASK,
                          enable ? MC33771C_SYS_CFG1_CB_DRVEN(MC33771C_SYS_CFG1_CB_DRVEN_ENABLED_ENUM_VAL) 
                          : MC33771C_SYS_CFG1_CB_DRVEN(MC33771C_SYS_CFG1_CB_DRVEN_DISABLED_ENUM_VAL));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_CB_SetIndividual
 * Description   : This function enables or disables cell balancing for a
 *                 specified cell and sets its timer.
 *
 *END**************************************************************************/
bcc_status_t BCC_CB_SetIndividual(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t cellIndex, const bool enable,
    const uint16_t timer)
{
    uint16_t cbxCfgVal;

    BCC_MCU_Assert(drvConfig != NULL);
    
    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (cellIndex >= BCC_MAX_CELLS_DEV(drvConfig->device[(uint8_t)cid - 1]))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (timer > MC33771C_CB1_CFG_CB_TIMER_MASK)
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    cbxCfgVal = enable ? MC33771C_CB1_CFG_CB_EN(MC33771C_CB1_CFG_CB_EN_ENABLED_ENUM_VAL) 
                       : MC33771C_CB1_CFG_CB_EN(MC33771C_CB1_CFG_CB_EN_DISABLED_ENUM_VAL);
    cbxCfgVal |= MC33771C_CB1_CFG_CB_TIMER(timer);
    return BCC_Reg_Write(drvConfig, cid, MC33771C_CB1_CFG_OFFSET + cellIndex, cbxCfgVal);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_CB_Pause
 * Description   : This function pauses cell balancing.
 *
 *END**************************************************************************/
bcc_status_t BCC_CB_Pause(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bool pause)
{
    BCC_MCU_Assert(drvConfig != NULL);
    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        // print_lpuart("Invalid CID\n");
        return BCC_STATUS_PARAM_RANGE;
    }

    return BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET, MC33771C_SYS_CFG1_CB_MANUAL_PAUSE_MASK,
                          (pause) ? MC33771C_SYS_CFG1_CB_MANUAL_PAUSE(MC33771C_SYS_CFG1_CB_MANUAL_PAUSE_ENABLED_ENUM_VAL) 
                                  : MC33771C_SYS_CFG1_CB_MANUAL_PAUSE(MC33771C_SYS_CFG1_CB_MANUAL_PAUSE_DISABLED_ENUM_VAL));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_FuseMirror_Read
 * Description   : This function reads a fuse mirror register of selected BCC
 *                 device.
 *
 *END**************************************************************************/
bcc_status_t BCC_FuseMirror_Read(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t fuseAddr, uint16_t* const value)
{
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(value != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (fuseAddr > ((drvConfig->device[(uint8_t)cid - 1U] == BCC_DEVICE_MC33771C) ?
            MC33771C_MAX_FUSE_READ_ADDR : MC33772C_MAX_FUSE_READ_ADDR))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FUSE_MIRROR_CNTL_OFFSET,
            MC33771C_FUSE_MIRROR_CNTL_FMR_ADDR(fuseAddr) |
            MC33771C_FUSE_MIRROR_CNTL_FSTM(MC33771C_FUSE_MIRROR_CNTL_FSTM_LOCKED_ENUM_VAL) |
            MC33771C_FUSE_MIRROR_CNTL_FST(MC33771C_FUSE_MIRROR_CNTL_FST_SPI_WRITE_ENABLE_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    return BCC_Reg_Read(drvConfig, cid, MC33771C_FUSE_MIRROR_DATA_OFFSET, 1U, value);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_FuseMirror_Write
 * Description   : This function writes a fuse mirror register of a BCC device
 *                 specified by CID.
 *
 *END**************************************************************************/
bcc_status_t BCC_FuseMirror_Write(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t fuseAddr, const uint16_t value)
{
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (fuseAddr > ((drvConfig->device[(uint8_t)cid - 1U] == BCC_DEVICE_MC33771C) ?
            MC33771C_MAX_FUSE_WRITE_ADDR : MC33772C_MAX_FUSE_WRITE_ADDR))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* FUSE_MIRROR_CNTL to enable writing. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FUSE_MIRROR_CNTL_OFFSET,
            MC33771C_FUSE_MIRROR_CNTL_FMR_ADDR(0U) |
            MC33771C_FUSE_MIRROR_CNTL_FSTM(MC33771C_FUSE_MIRROR_CNTL_FSTM_UNLOCKED_ENUM_VAL) |
            MC33771C_FUSE_MIRROR_CNTL_FST(MC33771C_FUSE_MIRROR_CNTL_FST_SPI_WRITE_ENABLE_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Send the fuse address. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FUSE_MIRROR_CNTL_OFFSET,
            MC33771C_FUSE_MIRROR_CNTL_FMR_ADDR(fuseAddr) |
            MC33771C_FUSE_MIRROR_CNTL_FSTM(MC33771C_FUSE_MIRROR_CNTL_FSTM_UNLOCKED_ENUM_VAL) |
            MC33771C_FUSE_MIRROR_CNTL_FST(MC33771C_FUSE_MIRROR_CNTL_FST_SPI_WRITE_ENABLE_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FUSE_MIRROR_DATA_OFFSET, value);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* FUSE_MIRROR_CNTL to low power. */
    return BCC_Reg_Write(drvConfig, cid, MC33771C_FUSE_MIRROR_CNTL_OFFSET,
            MC33771C_FUSE_MIRROR_CNTL_FMR_ADDR(0U) |
            MC33771C_FUSE_MIRROR_CNTL_FSTM(MC33771C_FUSE_MIRROR_CNTL_FSTM_UNLOCKED_ENUM_VAL) | 
            MC33771C_FUSE_MIRROR_CNTL_FST(MC33771C_FUSE_MIRROR_CNTL_FST_LP_ENUM_VAL));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_GUID_Read
 * Description   : This function reads an unique serial number of the BCC device
 *                 from the content of mirror registers.
 *
 *END**************************************************************************/
bcc_status_t BCC_GUID_Read(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint64_t* const guid)
{
    const uint8_t addr771c[3] = {
            MC33771C_FUSE_TR_0_OFFSET,
            MC33771C_FUSE_TR_1_OFFSET,
            MC33771C_FUSE_TR_2_OFFSET
    };
    const uint8_t addr772c[3] = {
            MC33772C_FUSE_TR_0_OFFSET,
            MC33772C_FUSE_TR_1_OFFSET,
            MC33772C_FUSE_TR_2_OFFSET
    };
    uint8_t const *readAddr;
    uint16_t readData[3];
    uint8_t i;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(guid != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    readAddr = (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33771C) ? addr771c : addr772c;

    for (i = 0; i < 3; i++)
    {
        status = BCC_FuseMirror_Read(drvConfig, cid, readAddr[i], &(readData[i]));
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    *guid = (((uint64_t)(readData[0] & BCC_FUSE_TR_0_MASK)) << 21) |
            (((uint64_t)(readData[1] & BCC_FUSE_TR_1_MASK)) << 5) |
            ((uint64_t)(readData[2] & BCC_FUSE_TR_2_MASK));

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_EEPROM_Read
 * Description   : This function reads a byte from specified address of EEPROM
 *                 memory connected to BCC device via I2C bus.
 *
 *END**************************************************************************/
bcc_status_t BCC_EEPROM_Read(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t addr, uint8_t* const data)
{
    bcc_status_t status;
    uint16_t regVal;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(data != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (addr > BCC_MAX_EEPROM_ADDR)
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* EEPROM Read command. */
    regVal = MC33771C_EEPROM_CTRL_R_W(MC33771C_EEPROM_CTRL_R_W_READ_ENUM_VAL) |
             MC33771C_EEPROM_CTRL_EEPROM_ADD(addr);
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_EEPROM_CTRL_OFFSET, regVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Wait while data is read from EEPROM. */
    status = BCC_MCU_StartTimeout(BCC_EEPROM_READ_TIMEOUT_US);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    do
    {
        status = BCC_Reg_Read(drvConfig, cid, MC33771C_EEPROM_CTRL_OFFSET, 1U, &regVal);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    } while ((regVal & MC33771C_EEPROM_CTRL_BUSY_MASK) && (!BCC_MCU_TimeoutExpired()));

    /* Check once more after timeout expiration because the read command takes
     * several tens/hundreds of microseconds (depends on user code efficiency)
     * and the last read command could be done relatively long before the
     * timeout expiration. */
    if (regVal & MC33771C_EEPROM_CTRL_BUSY_MASK)
    {
        status = BCC_Reg_Read(drvConfig, cid, MC33771C_EEPROM_CTRL_OFFSET, 1U, &regVal);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    if (regVal & MC33771C_EEPROM_CTRL_BUSY_MASK)
    {
        return BCC_STATUS_COM_TIMEOUT;
    }

    if (regVal & MC33771C_EEPROM_CTRL_EE_PRESENT_MASK)
    {
        return BCC_STATUS_EEPROM_PRESENT;
    }

    if (regVal & MC33771C_EEPROM_CTRL_ERROR_MASK)
    {
        return BCC_STATUS_EEPROM_ERROR;
    }

    /* Store read data to memory space defined by the pointer. */
    *data = (uint8_t)(regVal & MC33771C_EEPROM_CTRL_READ_DATA_MASK);

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_EEPROM_Write
 * Description   : This function writes a byte to specified address of EEPROM
 *                 memory connected to BCC device via I2C bus.
 *
 *END**************************************************************************/
bcc_status_t BCC_EEPROM_Write(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint8_t addr, const uint8_t data)
{
    bcc_status_t status;
    uint16_t regVal;

    BCC_MCU_Assert(drvConfig != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (addr > BCC_MAX_EEPROM_ADDR)
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* EEPROM Write command. */
    regVal = MC33771C_EEPROM_CTRL_R_W(MC33771C_EEPROM_CTRL_R_W_WRITE_ENUM_VAL) |
             MC33771C_EEPROM_CTRL_EEPROM_ADD(addr) |
             MC33771C_EEPROM_CTRL_DATA_TO_WRITE(data);
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_EEPROM_CTRL_OFFSET, regVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Wait while BCC sends the write command to EEPROM. */
    status = BCC_MCU_StartTimeout(BCC_EEPROM_WRITE_TIMEOUT_US);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    do
    {
        status = BCC_Reg_Read(drvConfig, cid, MC33771C_EEPROM_CTRL_OFFSET, 1U, &regVal);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    } while ((regVal & MC33771C_EEPROM_CTRL_BUSY_MASK) && (!BCC_MCU_TimeoutExpired()));

    /* Check once more after timeout expiration because the read command takes
     * several tens/hundreds of microseconds (depends on user code efficiency)
     * and the last read command could be done relatively long before the
     * timeout expiration. */
    if (regVal & MC33771C_EEPROM_CTRL_BUSY_MASK)
    {
        status = BCC_Reg_Read(drvConfig, cid, MC33771C_EEPROM_CTRL_OFFSET, 1U, &regVal);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    if (regVal & MC33771C_EEPROM_CTRL_BUSY_MASK)
    {
        return BCC_STATUS_COM_TIMEOUT;
    }

    if (regVal & MC33771C_EEPROM_CTRL_EE_PRESENT_MASK)
    {
        return BCC_STATUS_EEPROM_PRESENT;
    }

    if (regVal & MC33771C_EEPROM_CTRL_ERROR_MASK)
    {
        return BCC_STATUS_EEPROM_ERROR;
    }

    return BCC_STATUS_SUCCESS;
}
