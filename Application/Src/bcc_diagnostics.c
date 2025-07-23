/*!
 * @file bcc_diagnostics.c
 *
 * Diagnostics part of Battery cell controller SW driver for MC33771C and
 * MC33772C v2.2.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "bcc_diagnostics.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Time delay t_delay (in [us]) used in Cell balance fault diagnostics.
 */
#define BCC_DIAG_T_DELAY_US          100U

/*! @brief Time (in [us]) to perform auto-zero procedure after enabling the
 * current channel. */
#define BCC_DIAG_T_AZC_SETTLE_US     200U

/*! @brief Waiting time (in [us]) in Cell Terminal Leakage diagnostics
 *  containing the cell balance delay and cell terminal settling time. */
#define BCC_DIAG_TCT_SETTLE_CTL_US   710U

/*! @brief Waiting time (in [us]) in OV/UV detection diagnostics. */
#define BCC_DIAG_TCT_SETTLE_OV_US    100U

/*! @brief Bit map of odd cells (differs according to number of used cells) */
#define BCC_ODD_CELL_MASK(drvConfig, cid)                                        \
    (((drvConfig->device[(uint8_t)(cid) - 1] == BCC_DEVICE_MC33771C) ?           \
            ((drvConfig->cellCnt[(uint8_t)(cid) - 1] % 2) ? 0x2AA5 : 0x1555) :   \
            ((drvConfig->cellCnt[(uint8_t)(cid) - 1] % 2) ? 0x0029 : 0x0015)) &  \
            drvConfig->drvData.cellMap[(uint8_t)(cid) - 1])

/*! @brief Bit map of even cells (differs according to number of used cells) */
#define BCC_EVEN_CELL_MASK(drvConfig, cid)                                       \
    (((drvConfig->device[(uint8_t)(cid) - 1] == BCC_DEVICE_MC33771C) ?           \
            ((drvConfig->cellCnt[(uint8_t)(cid) - 1] % 2) ? 0x155A : 0x2AAA) :   \
            ((drvConfig->cellCnt[(uint8_t)(cid) - 1] % 2) ? 0x0016 : 0x002A)) &  \
            drvConfig->drvData.cellMap[(uint8_t)(cid) - 1])

/*! @brief Bit mask of all ANx UT bits in AN_OT_UT_FLT register. */
#define BCC_AN_UT_ALL_MASK                                                       \
    (MC33771C_AN_OT_UT_FLT_AN0_UT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN1_UT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN2_UT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN3_UT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN4_UT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN5_UT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN6_UT_MASK)

/*! @brief Bit mask of all ANx OT bits in AN_OT_UT_FLT register. */
#define BCC_AN_OT_ALL_MASK                                                       \
    (MC33771C_AN_OT_UT_FLT_AN0_OT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN1_OT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN2_OT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN3_OT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN4_OT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN5_OT_MASK |                                         \
     MC33771C_AN_OT_UT_FLT_AN6_OT_MASK)

/*! @brief Bit mask of all ANx OPEN bits in GPIO_SHORT_ANX_OPEN_STS register. */
#define BCC_AN_OPEN_STS_ALL_MASK                                                \
    (MC33771C_GPIO_SHORT_ANX_OPEN_STS_AN0_OPEN_MASK |                           \
     MC33771C_GPIO_SHORT_ANX_OPEN_STS_AN1_OPEN_MASK |                           \
     MC33771C_GPIO_SHORT_ANX_OPEN_STS_AN2_OPEN_MASK |                           \
     MC33771C_GPIO_SHORT_ANX_OPEN_STS_AN3_OPEN_MASK |                           \
     MC33771C_GPIO_SHORT_ANX_OPEN_STS_AN4_OPEN_MASK |                           \
     MC33771C_GPIO_SHORT_ANX_OPEN_STS_AN5_OPEN_MASK |                           \
     MC33771C_GPIO_SHORT_ANX_OPEN_STS_AN6_OPEN_MASK)

/*******************************************************************************
 * Prototypes of internal functions
 ******************************************************************************/

/*!
 * @brief This function fills SYS_CFG2[NUMB_ODD] according to the number of
 * cells.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_SetNumbOdd(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid);

/*!
 * @brief This function enters diagnostic mode.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_EnterDiagnostics(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid);

/*!
 * @brief This function exits diagnostic mode.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_ExitDiagnostics(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid);

/*!
 * @brief This function commands CT or CB diagnostic switches.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param switches  Selection of CT or CB switches.
 * @param odd       State of odd diagnostic switches.
 * @param even      State of even diagnostic switches.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_CommandSwitches(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_switch_sel_t switches,
    const bcc_diag_switch_pos_t odd, const bcc_diag_switch_pos_t even);

/*!
 * @brief This function is part of the overvoltage and undervoltage functional
 * verification. It commands CT switches and measures cell voltages.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param odd       State of odd switches.
 * @param even      State of even switches.
 * @param sm01twait Diagnostic time Twait for SM01 (in [us]).
 * @param fltOvrv   Result of the function containing CELL_OV_FLT register value.
 * @param fltUndv   Result of the function containing CELL_UV_FLT register value.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_DiagOvuvPart(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_switch_pos_t odd,
    const bcc_diag_switch_pos_t even, const uint32_t sm01twait,
    uint16_t* const fltOvrv, uint16_t* const fltUndv);

/*!
 * @brief This function is part of the CTx open detect verification. It clear
 * OV and UV fault registers, commands CT switches, measures cell voltages opens
 * switches and waits for the recovery time.
 *
 * @param drvConfig     Pointer to driver instance configuration.
 * @param cid           Cluster Identification Address of the BCC device.
 * @param odd           State of odd switches.
 * @param even          State of even switches.
 * @param diagTimeConst Diagnostic time constants.
 * @param measVal       Result of the function containing measured values for
 *                      CT1 - CT14/CT6 (in [uV]).
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_DiagCtxopenPart(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_switch_pos_t odd,
    const bcc_diag_switch_pos_t even,
    const bcc_diag_const_t* const diagTimeConst,
    uint32_t* const measVal);

/*!
 * @brief This function is part of the GPIO OT and UT functional verification.
 *
 * Writes value given in parameter to GPIO_CFG2 register, initiates conversion,
 * reads AN_OT_UT_FLT register and clears OT/UT faults.
 *
 * @param drvConfig   Pointer to driver instance configuration.
 * @param cid         Cluster Identification Address of the BCC device.
 * @param gpioCFG2Val Value of GPIO_CFG2 GPIO configuration register.
 * @param otUtStat    Over/Under-temperature status for GPIOs (expected for all
 *                    GPIOs). It contains value of AN_OT_UT_FLT register.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_GpioOtUtPart(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint16_t gpioCfg2Val, uint16_t* const otUtStat);

/*******************************************************************************
 * Internal function
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_SetNumbOdd
 * Description   : This function fills SYS_CFG2[NUMB_ODD] according to the
 *                 number of cells.
 *
 *END**************************************************************************/
static bcc_status_t BCC_SetNumbOdd(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid)
{
    /* If the number of cells on the cluster is odd, then write the bit
     * SYS_CFG2[NUMB_ODD] to logic 1, else write it to logic 0. */
    return BCC_Reg_Update(drvConfig, cid,
            MC33771C_SYS_CFG2_OFFSET,
            MC33771C_SYS_CFG2_NUMB_ODD_MASK,
            (drvConfig->cellCnt[(uint8_t)cid - 1] & 0x01U) ?
              MC33771C_SYS_CFG2_NUMB_ODD(MC33771C_SYS_CFG2_NUMB_ODD_ODD_ENUM_VAL) :
              MC33771C_SYS_CFG2_NUMB_ODD(MC33771C_SYS_CFG2_NUMB_ODD_EVEN_ENUM_VAL));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_EnterDiagnostics
 * Description   : This function enters diagnostic mode.
 *
 *END**************************************************************************/
static bcc_status_t BCC_EnterDiagnostics(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid)
{
    uint16_t sysCfg1Val;
    bcc_status_t status;

    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET,
                MC33771C_SYS_CFG1_GO2DIAG_MASK,
                MC33771C_SYS_CFG1_GO2DIAG(MC33771C_SYS_CFG1_GO2DIAG_ENTER_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Check that the device is in diagnostic mode. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET, 1U, &sysCfg1Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    if (!(sysCfg1Val & MC33771C_SYS_CFG1_DIAG_ST_MASK))
    {
        return BCC_STATUS_DIAG_FAIL;
    }

    return BCC_STATUS_SUCCESS;
}  

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_ExitDiagnostics
 * Description   : This function exits diagnostic mode.
 *
 *END**************************************************************************/
static bcc_status_t BCC_ExitDiagnostics(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid)
{
    return BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET,
            MC33771C_SYS_CFG1_GO2DIAG_MASK,
            MC33771C_SYS_CFG1_GO2DIAG(MC33771C_SYS_CFG1_GO2DIAG_EXIT_ENUM_VAL));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_CommandSwitches
 * Description   : This function commands CT or CB diagnostic switches.
 *
 *END**************************************************************************/
static bcc_status_t BCC_CommandSwitches(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_switch_sel_t switches,
    const bcc_diag_switch_pos_t odd, const bcc_diag_switch_pos_t even)
{
    uint16_t mask;     /* SYS_DIAG register mask. */
    uint16_t value;    /* New value. */

    /* Create content of a frame to command CB outputs */
    if (switches == BCC_SWITCH_SEL_CT)
    {
        mask = MC33771C_SYS_DIAG_CT_OL_ODD_MASK | MC33771C_SYS_DIAG_CT_OL_EVEN_MASK;
        value = (even == BCC_SWITCH_POS_OPEN) ?
                MC33771C_SYS_DIAG_CT_OL_EVEN(MC33771C_SYS_DIAG_CT_OL_EVEN_OPEN_ENUM_VAL) :
                MC33771C_SYS_DIAG_CT_OL_EVEN(MC33771C_SYS_DIAG_CT_OL_EVEN_CLOSED_ENUM_VAL);
        value |= (odd == BCC_SWITCH_POS_OPEN) ?
                MC33771C_SYS_DIAG_CT_OL_ODD(MC33771C_SYS_DIAG_CT_OL_ODD_OPEN_ENUM_VAL) :
                MC33771C_SYS_DIAG_CT_OL_ODD(MC33771C_SYS_DIAG_CT_OL_ODD_CLOSED_ENUM_VAL);
    }
    else
    {
        mask = MC33771C_SYS_DIAG_CB_OL_ODD_MASK | MC33771C_SYS_DIAG_CB_OL_EVEN_MASK;
        value = (even == BCC_SWITCH_POS_OPEN) ?
                MC33771C_SYS_DIAG_CB_OL_EVEN(MC33771C_SYS_DIAG_CB_OL_EVEN_OPEN_ENUM_VAL) :
                MC33771C_SYS_DIAG_CB_OL_EVEN(MC33771C_SYS_DIAG_CB_OL_EVEN_CLOSED_ENUM_VAL);
        value |= (odd == BCC_SWITCH_POS_OPEN) ?
                MC33771C_SYS_DIAG_CB_OL_ODD(MC33771C_SYS_DIAG_CB_OL_ODD_OPEN_ENUM_VAL) :
                MC33771C_SYS_DIAG_CB_OL_ODD(MC33771C_SYS_DIAG_CB_OL_ODD_CLOSED_ENUM_VAL);
    }

    return BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET, mask, value);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_DiagOvuvPart
 * Description   : This function is part of the overvoltage and undervoltage
 *                 functional verification. It commands CT switches and measures
 *                 cell voltages.
 *
 *END**************************************************************************/
static bcc_status_t BCC_DiagOvuvPart(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_switch_pos_t odd,
    const bcc_diag_switch_pos_t even, const uint32_t sm01twait,
    uint16_t* const fltOvrv, uint16_t* const fltUndv)
{
    uint16_t fault[2];   /* Value of OV and UV fault registers. */
    bcc_status_t status;

    /* Clear OV, UV faults. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_OV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_UV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5. Command CTx switches. */
    status = BCC_CommandSwitches(drvConfig, cid, BCC_SWITCH_SEL_CT, odd, even);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 6. Wait for the diagnostic time T_wait = K_DIAG*tau_DIAG,n. */
    BCC_MCU_WaitUs(sm01twait);

    /* 7. - 8. Initiate conversion with ADC_CFG[AVG]=0000b. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 9. - 10. Read OV & UV flags. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_CELL_OV_FLT_OFFSET, 2U, fault);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    (*fltOvrv) = fault[0];
    (*fltUndv) = fault[1];

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_DiagCtxopenPart
 * Description   : This function is part of the CTx open detect verification.
 *                 It clear OV and UV fault registers, commands CT switches,
 *                 measures cell voltages opens switches and waits for recovery
 *                 time.
 *
 *END**************************************************************************/
static bcc_status_t BCC_DiagCtxopenPart(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_switch_pos_t odd,
    const bcc_diag_switch_pos_t even,
    const bcc_diag_const_t* const diagTimeConst,
    uint32_t* const measVal)
{
    bcc_status_t status;

    /* Clear OV, UV faults. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_OV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_UV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Command fault detection switches. */
    status = BCC_CommandSwitches(drvConfig, cid, BCC_SWITCH_SEL_CT, odd, even);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 3. Wait for the diagnostic time T_wait = K_DIAG * tau_DIAG. */
    BCC_MCU_WaitUs(diagTimeConst->sm02twait);

    /* 4. - 5. Initiate conversion with ADC_CFG[SOC]=1. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 6. Read conversion results. */
    status = BCC_Meas_GetCellVoltages(drvConfig, cid, measVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 8. Open all switches. */
    status = BCC_CommandSwitches(drvConfig, cid, BCC_SWITCH_SEL_CT,
                                 BCC_SWITCH_POS_OPEN, BCC_SWITCH_POS_OPEN);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 10. Wait for the recovery time T_recv = K * tau. */
    BCC_MCU_WaitUs(diagTimeConst->sm02trecv);

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_GpioOtUtPart
 * Description   : This function is part of the GPIO OT and UT functional
 *                 verification.
 *
 *END**************************************************************************/
static bcc_status_t BCC_GpioOtUtPart(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const uint16_t gpioCfg2Val, uint16_t* const otUtStat)
{
    bcc_status_t status;

    /* 3. Set GPIO_CFG2[GPIOx_DR] register to drive output for overtemperature
     * or undertemperature. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_GPIO_CFG2_OFFSET, gpioCfg2Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5. Initiate conversion. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 6. Conversions below the TH_ANx_OT threshold trigger the ANx_OT fault bit.
     * Conversions above the TH_ANx_UT threshold trigger the ANx_UT fault bit. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_AN_OT_UT_FLT_OFFSET, 1U, otUtStat);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Clear OT/UT faults before exit. */
    return BCC_Reg_Write(drvConfig, cid, MC33771C_AN_OT_UT_FLT_OFFSET, 0U);
}

/******************************************************************************
 * API
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_ADC1
 * Description   : This function implements the ADC1-A and ADC1-B functional
 *                 verification (SM07).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_ADC1(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_adc1x_res_t* const results)
{
    uint16_t fault2MaskVal;       /* Original value of FAULT_MASK2 register. */
    uint16_t wakeUp2MaskVal;      /* Original value of WAKEUP_MASK2 register. */
    uint16_t adcCfgVal;           /* Original value of ADC_CFG register. */
    uint16_t regVal[2];           /* Read values of MEAS_VBG_DIAG_ADC1A and ADC1B registers. */
    uint32_t thMinUv, thMaxUv;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(results != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33771C)
    {
        thMinUv = BCC_DIAG_V_BGP - MC33771C_DIAG_ADC1X_FV;
        thMaxUv = BCC_DIAG_V_BGP + MC33771C_DIAG_ADC1X_FV;
    }
    else
    {
        thMinUv = BCC_DIAG_V_BGP - MC33772C_DIAG_ADC1X_FV;
        thMaxUv = BCC_DIAG_V_BGP + MC33772C_DIAG_ADC1X_FV;
    }

    /* Disable the ADC1_A and ADC1_B fault detection by setting FAULT_MASK2
     * register, MASK_11_F = MASK_10_F = 1. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_FAULT_MASK2_OFFSET, 1U, &fault2MaskVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FAULT_MASK2_OFFSET,
            fault2MaskVal |
            MC33771C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F(MC33771C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F_MASKED_ENUM_VAL) | 
            MC33771C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F(MC33771C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F_MASKED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Disable the wake-up, due to ADC1_A and ADC1_B faults, by setting
     * WAKEUP_MASK2 register, MASK_11_F = MASK_10_F = 1. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_WAKEUP_MASK2_OFFSET, 1U, &wakeUp2MaskVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_WAKEUP_MASK2_OFFSET,
            wakeUp2MaskVal |
            MC33771C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F(MC33771C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F_MASKED_ENUM_VAL) |
            MC33771C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F(MC33771C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F_MASKED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Detection performance can be guaranteed only if ADC_CFG[ADC1_A_DEF] =
     * ADC_CFG[ADC1_B_DEF] = 11 (16 bit resolution). */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_ADC_CFG_OFFSET, 1U, &adcCfgVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_ADC_CFG_OFFSET,
            (adcCfgVal & ~(MC33771C_ADC_CFG_ADC1_A_DEF_MASK | MC33771C_ADC_CFG_ADC1_B_DEF_MASK))
                    | MC33771C_ADC_CFG_ADC1_A_DEF(MC33771C_ADC_CFG_ADC1_A_DEF_16_BIT_ENUM_VAL)
                    | MC33771C_ADC_CFG_ADC1_B_DEF(MC33771C_ADC_CFG_ADC1_B_DEF_16_BIT_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Measurement values of the diagnostic band-gap may be obtained by reading
     * the MEAS_VBG_DIAG_ADC1A and MEAS_VBG_DIAG_ADC1B after having sent a SOC
     * and waited for the necessary time to get the averaged result. It is
     * recommended the pack controller to use ADC_CFG[AVG]=0011b. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_8);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Read conversion results. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_MEAS_VBG_DIAG_ADC1A_OFFSET, 2U, regVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Write back the original values of FAULT_MASK2, WAKEUP_MASK2 and
     * ADC_CFG registers. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FAULT_MASK2_OFFSET, fault2MaskVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_WAKEUP_MASK2_OFFSET, wakeUp2MaskVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_ADC_CFG_OFFSET, adcCfgVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Evaluate results. */
    results->adc1aAvg = BCC_GET_VOLT(regVal[0]);
    results->adc1bAvg = BCC_GET_VOLT(regVal[1]);

    if (BCC_IS_IN_RANGE(results->adc1aAvg, thMinUv, thMaxUv) &&
        BCC_IS_IN_RANGE(results->adc1bAvg, thMinUv, thMaxUv))
    {
        results->error = false;
    }
    else
    {
        results->error = true;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_OvUvVer
 * Description   : This function implements the OV/UV functional verification
 *                 (SM01).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_OvUvVer(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_const_t* const diagConst,
    bcc_diag_ov_uv_ver_res_t* const results)
{
    uint16_t fault1MaskVal;   /* Original value of FAULT_MASK1 register. */
    uint16_t ovUvEnVal;       /* Original value of OV_UV_EN register. */
    uint16_t thAllCtVal;      /* Original value of TH_ALL_CT register. */
    uint16_t regVal;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(diagConst != NULL);
    BCC_MCU_Assert(results != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* a. If the number of cells on the cluster is odd, then write the bit
     * SYS_CFG2[NUMB_ODD] to logic 1, else write it to logic 0. */
    status = BCC_SetNumbOdd(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Mask simulated OV/UV faults - do not activate FAULT pin for these faults. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_FAULT_MASK1_OFFSET, 1U, &fault1MaskVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FAULT_MASK1_OFFSET,
            fault1MaskVal |
            MC33771C_FAULT_MASK1_CT_UV_FLT_MASK_0_F(MC33771C_FAULT_MASK1_CT_UV_FLT_MASK_0_F_MASKED_ENUM_VAL) |
            MC33771C_FAULT_MASK1_CT_OV_FLT_MASK_1_F(MC33771C_FAULT_MASK1_CT_OV_FLT_MASK_1_F_MASKED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 1. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Write OV_UV_EN[CTx_OVUV_EN] for x = 1..6/14 to enable OV/UV. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_OV_UV_EN_OFFSET, 1U, &ovUvEnVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_OV_UV_EN_OFFSET,
            (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33771C) ? 0xFFFFU : 0xC03FU);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 3. Set the OV and the UV thresholds to diagnostic values (see CTx_OV_TH
     * and CTx_UV_TH parameters in reference document) */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_TH_ALL_CT_OFFSET, 1U, &thAllCtVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    regVal = MC33771C_TH_ALL_CT_ALL_CT_OV_TH(BCC_GET_TH_CTX(diagConst->sm01ovTh)) |
             MC33771C_TH_ALL_CT_ALL_CT_UV_TH(BCC_GET_TH_CTX(diagConst->sm01uvTh));
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_TH_ALL_CT_OFFSET, regVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 4. Enter OV and UV functional verification. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
            MC33771C_SYS_DIAG_CT_OV_UV_MASK,
            MC33771C_SYS_DIAG_CT_OV_UV(MC33771C_SYS_DIAG_CT_OV_UV_ENABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5. - 10. Command switches, run measurement and read results (odd switches opened,
     * even closed). */
    status = BCC_DiagOvuvPart(drvConfig, cid, BCC_SWITCH_POS_OPEN, BCC_SWITCH_POS_CLOSED,
                              diagConst->sm01twait, &(results->ovOdd), &(results->uvEven));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 11. - 16. Command switches, run measurement and read results (odd switches closed,
     * even opened). */
    status = BCC_DiagOvuvPart(drvConfig, cid, BCC_SWITCH_POS_CLOSED, BCC_SWITCH_POS_OPEN,
                              diagConst->sm01twait, &(results->ovEven), &(results->uvOdd));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 17. Open all switches and leave OV and UV functional verification.
     * (Write to SYS_DIAG[CT_OL_ODD,CT_OL_EVEN] field to the 00 configuration
     * and write SYS_DIAG[CT_OV_UV] bit to logic 0 to exit OV and UV functional
     * verification.) */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_CT_OL_ODD_MASK | MC33771C_SYS_DIAG_CT_OL_EVEN_MASK | MC33771C_SYS_DIAG_CT_OV_UV_MASK,
                            MC33771C_SYS_DIAG_CT_OL_ODD(MC33771C_SYS_DIAG_CT_OL_ODD_OPEN_ENUM_VAL) |
                            MC33771C_SYS_DIAG_CT_OL_EVEN(MC33771C_SYS_DIAG_CT_OL_EVEN_OPEN_ENUM_VAL) |
                            MC33771C_SYS_DIAG_CT_OV_UV(MC33771C_SYS_DIAG_CT_OV_UV_DISABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 18. Wait for the recovery time T_recv = K * tau. */
    BCC_MCU_WaitUs(diagConst->sm01trecv);

    /* 19. Restore normal functional values for the OV and UV thresholds. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_TH_ALL_CT_OFFSET, thAllCtVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_OV_UV_EN_OFFSET, ovUvEnVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 20. Clear CELL_OV_FLT and CELL_UV_FLT fault registers, as well as
     * FAULT1_STATUS[CT_OV_FLT, CT_UV_FLT] bits. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_OV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_UV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FAULT_MASK1_OFFSET, fault1MaskVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Update(drvConfig, cid,
            MC33771C_FAULT1_STATUS_OFFSET,
            MC33771C_FAULT1_STATUS_CT_OV_FLT_MASK |
            MC33771C_FAULT1_STATUS_CT_UV_FLT_MASK,
            MC33771C_FAULT1_STATUS_CT_OV_FLT(MC33771C_FAULT1_STATUS_CT_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL) |
            MC33771C_FAULT1_STATUS_CT_UV_FLT(MC33771C_FAULT1_STATUS_CT_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 21. Exit diagnostic mode. */
    status = BCC_ExitDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Evaluate diagnostics. */
    if ((results->ovOdd & drvConfig->drvData.cellMap[(uint8_t)cid - 1]) != BCC_ODD_CELL_MASK(drvConfig, cid))
    {
        results->error = true;
    }
    else if ((results->ovEven & drvConfig->drvData.cellMap[(uint8_t)cid - 1]) != BCC_EVEN_CELL_MASK(drvConfig, cid))
    {
        results->error = true;
    }
    else if ((results->uvOdd & drvConfig->drvData.cellMap[(uint8_t)cid - 1]) != BCC_ODD_CELL_MASK(drvConfig, cid))
    {
        results->error = true;
    }
    else if ((results->uvEven & drvConfig->drvData.cellMap[(uint8_t)cid - 1]) != BCC_EVEN_CELL_MASK(drvConfig, cid))
    {
        results->error = true;
    }
    else
    {
        results->error = false;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_OvUvDet
 * Description   : This function implements OV/UV detection in the pack
 *                 controller (SM34).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_OvUvDet(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_const_t* const diagConst,
    bcc_diag_ov_uv_det_res_t* const results)
{
    uint16_t cbxCfgRegs[BCC_MAX_CELLS];   /* Array for read registers. */
    bool cbOn[BCC_MAX_CELLS];             /* State of CB (true: on). */
    uint16_t sysCfg1Val;                  /* Original value of SYS_CFG1. */
    uint8_t i;
    bcc_device_t deviceType;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(diagConst != NULL);
    BCC_MCU_Assert(results != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    deviceType = drvConfig->device[(uint8_t)cid - 1];

    /* Read the SYS_CFG1 register. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET, 1U, &sysCfg1Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Note: steps 1 through 5 and step 8 of the procedure may be skipped
     * by using the normally measured cell voltage values in steps 6 and 7,
     * to detect OV/UV errors; while steps 9 through 17 are always needed. */

    /* 1. Pause cell balancing. */
    status = BCC_CB_Pause(drvConfig, cid, true);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Wait for the time interval T_CTsettle. */
    BCC_MCU_WaitUs(BCC_DIAG_TCT_SETTLE_OV_US);

    /* 3. Initiate conversion. */
    /* 4. Wait for conversion time. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5. Read the cell voltage measurements. */
    status = BCC_Meas_GetCellVoltages(drvConfig, cid, results->cellVoltCbOff);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 6. If a cell voltage is lower than the Vdet_UV detection threshold,
     * then an error is detected.
     * 7. If a cell voltage is greater than the Vdet_OV detection threshold,
     * then an error is detected. */
    results->error = false;
    for (i = 0; i < BCC_MAX_CELLS_DEV(deviceType); i++)
    {
        if (BCC_IS_CELL_CONN(drvConfig, (uint8_t)cid, i + 1))
        {
            if ((results->cellVoltCbOff[i] < diagConst->sm34uvTh) ||
                (results->cellVoltCbOff[i] > diagConst->sm34ovTh))
            {
                results->error = true;
                break;
            }
        }
    }

    /* 8. Unpause cell balancing. */
    status = BCC_CB_Pause(drvConfig, cid, false);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 9. Write the list of all the cells having the cell balance switch in the
     * off state. */
    status = BCC_CB_Enable(drvConfig, cid, true);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Read(drvConfig, cid, MC33771C_CB1_CFG_OFFSET,
                          BCC_MAX_CELLS_DEV(deviceType), cbxCfgRegs);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    for (i = 0; i < BCC_MAX_CELLS_DEV(deviceType); i++)
    {
        cbOn[i] = ((cbxCfgRegs[i] & MC33771C_CB1_CFG_CB_STS_MASK) > 0);
    }

    /* 10. Turn on the CB switch of all cells included in the list. */
    for (i = 0; i < BCC_MAX_CELLS_DEV(deviceType); i++)
    {
        if (!cbOn[i])
        {
            status = BCC_CB_SetIndividual(drvConfig, cid, i, true, 0);
            if (status != BCC_STATUS_SUCCESS)
            {
                return status;
            }
        }
    }

    /* 11. Wait for the time interval T_CTsettle. */
    BCC_MCU_WaitUs(BCC_DIAG_TCT_SETTLE_OV_US);

    /* 12. Initiate conversion. */
    /* 13. Wait for conversion time. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 14. Read the cell voltage measurements. */
    status = BCC_Meas_GetCellVoltages(drvConfig, cid, results->cellVoltCbOn);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 15. If a cell voltage is lower than the Vdet_UV detection threshold,
     * then an error is detected.
     * 16. If a cell voltage is greater than the Vdet_OV detection threshold,
     * then an error is detected. */
    for (i = 0; i < BCC_MAX_CELLS_DEV(deviceType); i++)
    {
        if (BCC_IS_CELL_CONN(drvConfig, (uint8_t)cid, i + 1))
        {
            if ((results->cellVoltCbOn[i] < diagConst->sm34uvTh) ||
                (results->cellVoltCbOn[i] > diagConst->sm34ovTh))
            {
                results->error = true;
                break;
            }
        }
    }

    /* 17. Restore the CB switch to off for all the cell voltages belonging
     * to the list. */
    for (i = 0; i < BCC_MAX_CELLS_DEV(deviceType); i++)
    {
        if (!cbOn[i])
        {
            status = BCC_CB_SetIndividual(drvConfig, cid, i, false, 0);
            if (status != BCC_STATUS_SUCCESS)
            {
                return status;
            }
        }
    }

    /* Restore the content of SYS_CFG1[CB_DRVEN]. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET, sysCfg1Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_CTxOpen
 * Description   : This function implements CTx open detection and functional
 *                 verification (SM02).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_CTxOpen(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_const_t* const diagConst,
    bcc_diag_ctx_open_res_t* const results)
{
    uint32_t vSwOpen, vSwShort;
    uint16_t faultMask1Val;              /* Value of FAULT_MASK1 register. */
    uint8_t i;                           /* Cell index for iteration. */
    bcc_device_t deviceType;
    bool odd;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(diagConst != NULL);
    BCC_MCU_Assert(results != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    deviceType = drvConfig->device[(uint8_t)cid - 1];

    /* If the number of cells on the cluster is odd, then write the bit
     * SYS_CFG2[NUMB_ODD] to logic 1, else write it to logic 0. */
    status = BCC_SetNumbOdd(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Mask simulated faults (OV & UV). */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_FAULT_MASK1_OFFSET, 1U, &faultMask1Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FAULT_MASK1_OFFSET,
                           faultMask1Val |
                           MC33771C_FAULT_MASK1_CT_UV_FLT_MASK_0_F(MC33771C_FAULT_MASK1_CT_UV_FLT_MASK_0_F_MASKED_ENUM_VAL) |
                           MC33771C_FAULT_MASK1_CT_OV_FLT_MASK_1_F(MC33771C_FAULT_MASK1_CT_OV_FLT_MASK_1_F_MASKED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 1. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. - 6., 8., 10. Command switches, run measurement, read results and open switches
     * with SYS_DIAG[CT_OL_ODD,CT_OL_EVEN] = 10 (ODD fault detect switches closed). */
    status = BCC_DiagCtxopenPart(drvConfig, cid, BCC_SWITCH_POS_CLOSED, BCC_SWITCH_POS_OPEN,
                                 diagConst, results->measOddClosed);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. - 6., 8., 10. Command switches, run measurement, read results and open switches
     * with SYS_DIAG[CT_OL_ODD,CT_OL_EVEN] = 01 (EVEN fault detect switches closed). */
    status = BCC_DiagCtxopenPart(drvConfig, cid, BCC_SWITCH_POS_OPEN, BCC_SWITCH_POS_CLOSED,
                                 diagConst, results->measEvenClosed);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 7. Determine fault mode.
     * Open line condition during test execution:
     *   SWx OFF: No decision
     *   SWx ON: V < VOL_DETECT
     */
    results->ctxOpen = 0U;
    results->swxOpen = 0U;
    results->swxShort = 0U;

    odd = true;
    for (i = 0U; i < BCC_MAX_CELLS_DEV(deviceType); i++)
    {
        if (BCC_IS_CELL_CONN(drvConfig, (uint8_t)cid, i + 1U))
        {
            vSwShort = (odd) ? results->measOddClosed[i] : results->measEvenClosed[i];
            vSwOpen = (odd) ? results->measEvenClosed[i] : results->measOddClosed[i];

            /* Open line detection: V(SWx commanded ON) < VOL_DETECT. */
            if (vSwShort < diagConst->sm02voldetect)
            {
                results->ctxOpen |= (uint16_t)(1U << i);
            }
            /* Open SWx detection: V(SWx commanded ON) >= min(Vcell). */
            if (vSwShort >= diagConst->sm02vcellMin)
            {
                results->swxOpen |= (uint16_t)(1U << i);
            }
            /* Short SWx detection: V(SWx commanded OFF) < CTx_UV_TH. */
            if (vSwOpen < diagConst->sm02uvTh)
            {
                results->swxShort |= (uint16_t)(1U << i);
            }

            odd = !odd;
        }
    }

    /* 11. Clear CELL_OV_FLT and CELL_UV_FLT fault registers. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_OV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_UV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Update(drvConfig, cid,
            MC33771C_FAULT1_STATUS_OFFSET,
            MC33771C_FAULT1_STATUS_CT_OV_FLT_MASK |
            MC33771C_FAULT1_STATUS_CT_UV_FLT_MASK,
            MC33771C_FAULT1_STATUS_CT_OV_FLT(MC33771C_FAULT1_STATUS_CT_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL) |
            MC33771C_FAULT1_STATUS_CT_UV_FLT(MC33771C_FAULT1_STATUS_CT_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Restore the previous content of FAULT_MASK1 register. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FAULT_MASK1_OFFSET, faultMask1Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 12. Exit diagnostic mode. */
    return BCC_ExitDiagnostics(drvConfig, cid);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_CellVolt
 * Description   : This function implements Cell Voltage Channel functional
 *                 verification (SM03).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_CellVolt(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_cell_volt_res_t* const results)
{
    uint16_t adcCfgVal;                 /* Original value of ADC_CFG register. */
    uint16_t regVal;                    /* Value of ADC_CFG register. */
    uint32_t measVoltUv[BCC_MAX_CELLS]; /* Measured cell voltages in [uV]. */
    uint8_t i;
    bcc_device_t deviceType;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(results != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    deviceType = drvConfig->device[(uint8_t)cid - 1];

    /* 1. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Configure 16 bit resolution of ADC1-A, B
     * (ADC_CFG[ADC1_A_DEF] = ADC_CFG[ADC1_B_DEF] = 11). */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_ADC_CFG_OFFSET, 1U, &adcCfgVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    regVal = adcCfgVal & ~(MC33771C_ADC_CFG_ADC1_A_DEF_MASK | MC33771C_ADC_CFG_ADC1_B_DEF_MASK);
    regVal |= MC33771C_ADC_CFG_ADC1_A_DEF(MC33771C_ADC_CFG_ADC1_A_DEF_16_BIT_ENUM_VAL) |
              MC33771C_ADC_CFG_ADC1_B_DEF(MC33771C_ADC_CFG_ADC1_B_DEF_16_BIT_ENUM_VAL);

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_ADC_CFG_OFFSET, regVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Isolate CTx inputs and places reference at amplifier input. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_DA_DIAG_MASK,
                            MC33771C_SYS_DIAG_DA_DIAG(MC33771C_SYS_DIAG_DA_DIAG_ENABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 3. Initiate conversion with ADC_CFG[AVG]= 0011b. */
    /* 4. Wait for conversion time. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_8);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5a. For x= 1 to 6/14, read MEAS_CELLx registers */
    status = BCC_Meas_GetCellVoltages(drvConfig, cid, measVoltUv);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    results->result = 0U;
    if (deviceType == BCC_DEVICE_MC33771C)
    {
        for (i = 0; i < MC33771C_MAX_CELLS; i++)
        {
            /* 5b. Compute V_err_x = (MEAS_CELLx - MEAS_CELL2) errors converted in mV. */
            results->vErrX[i] = (int32_t)measVoltUv[i] - (int32_t)measVoltUv[1];

            /* 6. For any x, check min(V_CVFV) <= V_err_x <= max(V_CVFV). If true,
             * result ok, else error is detected and exit.
             * Note: Checking all channels is necessary only if all 14 cells are
             * used; otherwise, unused cell voltage channels may be skipped. */
            if (BCC_IS_CELL_CONN(drvConfig, (uint8_t)cid, i + 1U))
            {
                if ((results->vErrX[i] < BCC_DIAG_VCVFV_MIN) || (results->vErrX[i] > BCC_DIAG_VCVFV_MAX))
                {
                    results->result |= (uint16_t)(1U << i);
                }
            }
        }
    }
    else
    {
        /* 5b. Compute errors
         * V_err_x = (MEAS_CELLx + MEAS_CELLx+1)/2 - (MEAS_CELL1 + MEAS_CELL2)/2,
         * for x = {3,5}. */
        if (drvConfig->cellCnt[(uint8_t)cid - 1] <= 4U)
        {
            /* NOTE: Checking all channels from 3 to 6 is necessary only if all
             * 6 cells are used; otherwise, unused cell voltage channels may be skipped. */
            results->vErrX[0] = 0;
        }
        else if (drvConfig->cellCnt[(uint8_t)cid - 1] == 5U)
        {
            /* V_err_3 = (MEAS_CELL4) - (MEAS_CELL1 + MEAS_CELL2)/2 */
            results->vErrX[0] = ((int32_t)(measVoltUv[3])) -
                      ((int32_t)(measVoltUv[0] + measVoltUv[1]))/2;

        }
        else
        {
            /* V_err_3 = (MEAS_CELL3 + MEAS_CELL4)/2 - (MEAS_CELL1 + MEAS_CELL2)/2 */
            results->vErrX[0] =
                    (((int32_t)(measVoltUv[2] + measVoltUv[3])) -
                     ((int32_t)(measVoltUv[0] + measVoltUv[1])))/2;

        }

        if (drvConfig->cellCnt[(uint8_t)cid - 1] == 3U)
        {
            /* V_err_5 = (MEAS_CELL6) - (MEAS_CELL1 + MEAS_CELL2)/2 */
            results->vErrX[1] = ((int32_t)(measVoltUv[5])) -
                                 ((int32_t)(measVoltUv[0] + measVoltUv[1]))/2;
        }
        else
        {
            /* V_err_5 = (MEAS_CELL5 + MEAS_CELL6)/2 - (MEAS_CELL1 + MEAS_CELL2)/2 */
            results->vErrX[1] = (((int32_t)(measVoltUv[4] + measVoltUv[5])) -
                                 ((int32_t)(measVoltUv[0] + measVoltUv[1])))/2;

        }

        /* 6. The system controller checks, for any x = {3,5},
         * if min(V_CVFV) <= V_err_x <= max(V_CVFV) are true.
         * If yes, the result is ok, else an error is detected. */
        for (i = 0; i < 2; i++)
        {
            if (!BCC_IS_IN_RANGE(results->vErrX[i], BCC_DIAG_VCVFV_MIN, BCC_DIAG_VCVFV_MAX))
            {
                results->result |= (uint16_t)(1U << i);
            }
        }
    }

    if ((deviceType == BCC_DEVICE_MC33771C) &&
        (results->result == 0))
    {
        /* If no error was detected. */

        /* 7. Initiate conversion with ADC_CFG[AVG]= 0000b. */
        /* 8. Wait for conversion time. */
        status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        /* 9a. For x= 1 to 14, read MEAS_CELLx registers */
        status = BCC_Meas_GetCellVoltages(drvConfig, cid, measVoltUv);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        results->result = 0U;
        for (i = 0; i < MC33771C_MAX_CELLS; i++)
        {
            /* 9b. Compute V_x = (MEAS_CELLx - MEAS_CELL2) errors converted in mV. */
            results->vX[i] = (int32_t)measVoltUv[i] - (int32_t)measVoltUv[1];

            results->vDiffX[i] = results->vErrX[i] - results->vX[i];

            /* 10. For any x, check |V_err_x - V_x| <= |min(V_CVFV)|. If true,
             * result ok, else error is detected and exit.
             * Note: Checking all channels is necessary only if all 14 cells are
             * used; otherwise, unused cell voltage channels may be skipped. */
            if (BCC_IS_CELL_CONN(drvConfig, (uint8_t)cid, i + 1U))
            {
                if (!BCC_IS_IN_RANGE(results->vDiffX[i], BCC_DIAG_VCVFV_MIN, -(BCC_DIAG_VCVFV_MIN)))
                {
                    results->result |= (uint16_t)(1U << i);
                }
            }
        }
    }

    /* 11. Clear cell OV faults in both the FAULT1_STATUS[CT_OV_FLT] bit and the CELL_OV_FLT register.
     * Note: FAULT1_STATUS[CT_OV_FLT] bit is automatically cleared by reset the CELL_OV_FLT. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_OV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Restore previous ADC settings. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_ADC_CFG_OFFSET, adcCfgVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 12. Exit diagnostic mode. */
    status = BCC_ExitDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_ConnResistance
 * Description   : This function detects a connector having an abnormally high
 *                 contact resistance. It is a part of cell terminals and cell
 *                 balancing terminals leakage diagnostics (SM04).
 *
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_ConnResistance(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_conn_res_res_t* const result)
{
    uint16_t measRaw[3][BCC_MAX_CELLS]; /* Measured voltages for three configurations (raw values). */
    int32_t dvThUv;
    uint8_t i, deviceCellsMax, firstMeasRegAddr;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(result != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33771C)
    {
        deviceCellsMax = MC33771C_MAX_CELLS;
        firstMeasRegAddr = MC33771C_MEAS_CELL14_OFFSET;
        dvThUv = MC33771C_DIAG_DV;
    }
    else
    {
        deviceCellsMax = MC33772C_MAX_CELLS;
        firstMeasRegAddr = MC33772C_MEAS_CELL6_OFFSET;
        dvThUv = MC33772C_DIAG_DV;
    }

    result->result = 0x0000;

    /* 1. Set all CBx drivers to OFF.
     * Note: Can be skipped if all CBx are already OFF. */
    for (i = 0; i < deviceCellsMax; i++)
    {
        status = BCC_CB_SetIndividual(drvConfig, cid, i, false, 0);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    status = BCC_CB_Enable(drvConfig, cid, true);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Wait for the time interval T_CTsettle.
     * Note: Can be skipped if all CBx are already OFF. */
    BCC_MCU_WaitUs(BCC_DIAG_TCT_SETTLE_CTL_US);

    /* 3. Measure the cell voltage Vcell_meas_x(CB_OFF) (for x=1 to 6/14) */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Read(drvConfig, cid, firstMeasRegAddr,
                          deviceCellsMax, &(measRaw[0][0]));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 4. Set CBx drivers to ON (for all odd x) - CB1, CB3, etc. */
    for (i = 0; i < deviceCellsMax; i += 2)
    {
        status = BCC_CB_SetIndividual(drvConfig, cid, i, true, 1);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    /* 5. Wait for the time interval T_CTsettle (cell balance delay + cell
     * terminal settling time). */
    BCC_MCU_WaitUs(BCC_DIAG_TCT_SETTLE_CTL_US);

    /* 6. Measure the cell voltage Vcell_meas_x(CB_ON) (for all odd x). */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* MEAS_CELL13/MEAS_CELL5 -> MEAS_CELL1 are read out. */
    status = BCC_Reg_Read(drvConfig, cid, firstMeasRegAddr + 1,
                          deviceCellsMax - 1, &(measRaw[1][1]));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 7. Set CBx drivers to OFF (for all odd x) - CB1, CB3, etc. */
    for (i = 0; i < deviceCellsMax; i += 2)
    {
        status = BCC_CB_SetIndividual(drvConfig, cid, i, false, 1);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    /* 8. Set CBx drivers to ON (for all even x) - CB2, CB4, etc. */
    for (i = 1; i < deviceCellsMax; i += 2)
    {
        status = BCC_CB_SetIndividual(drvConfig, cid, i, true, 1);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    /* 9. Wait for the time interval T_CTsettle (cell balance delay + cell
     * terminal settling time). */
    BCC_MCU_WaitUs(BCC_DIAG_TCT_SETTLE_CTL_US);

    /* 10. Measure the cell voltage Vcell_meas_x(CB_ON) (for all even x). */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* MEAS_CELL14/MEAS_CELL6 -> MEAS_CELL2 are read out. */
    status = BCC_Reg_Read(drvConfig, cid, firstMeasRegAddr,
                          deviceCellsMax - 1, &(measRaw[2][0]));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 11. Set CBx drivers to OFF (for all even x). */
    for (i = 1; i < deviceCellsMax; i += 2)
    {
        status = BCC_CB_SetIndividual(drvConfig, cid, i, false, 1);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }
    }

    /* Evaluate results.
       If |Vcell_meas_x(CB_ON) - Vcell_meas_x(CB_OFF)| > DV_diag, then error is detected. */
    for (i = 0; i < deviceCellsMax; i++)
    {
        /* For odd cells, measRaw[1] is used. For even cells, measRaw[2] is used. */
        result->diff[i] =
                ((int32_t)(BCC_GET_VOLT(measRaw[1 + (i % 2)][deviceCellsMax - (i + 1)]))) -
                ((int32_t)(BCC_GET_VOLT(measRaw[0][deviceCellsMax - (i + 1)])));

        if (!BCC_IS_IN_RANGE(result->diff[i], -dvThUv, dvThUv))
        {
            result->result |= (uint16_t)(1U << i);
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_CTxLeak
 * Description   : This function detects a leakage current. It is a part of cell
 *                 terminals and cell balancing terminals leakage diagnostics
 *                 (SM04).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_CTxLeak(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_ctx_leak_res_t* const result)
{
    uint32_t measUv[2][BCC_MAX_CELLS + 1];  /* Measured voltages in [uV] for two configurations. */
    uint32_t vLeakThUv;
    uint8_t i, deviceCellsMax;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(result != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33771C)
    {
        deviceCellsMax = MC33771C_MAX_CELLS;
        vLeakThUv = MC33771C_DIAG_VLEAK;
    }
    else
    {
        deviceCellsMax = MC33772C_MAX_CELLS;
        vLeakThUv = MC33772C_DIAG_VLEAK;
    }

    /* 1. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Pause cell balancing. */
    status = BCC_CB_Pause(drvConfig, cid, true);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 3. Write SYS_DIAG[CT_LEAK_DIAG,POLARITY] = 10 to route cell terminal
     * and balancing pins according to the logic of the routing table. */
    status = BCC_Reg_Update(drvConfig, cid,
                            MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_CT_LEAK_DIAG_MASK |
                            MC33771C_SYS_DIAG_POLARITY_MASK,
                            MC33771C_SYS_DIAG_CT_LEAK_DIAG(MC33771C_SYS_DIAG_CT_LEAK_DIAG_DIFF_ENUM_VAL) |
                            MC33771C_SYS_DIAG_POLARITY(MC33771C_SYS_DIAG_POLARITY_NONINVERTED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 4. Write ADC_CFG[AVG]= 0010b and ADC_CFG[SOC] to initiate a conversion. */
    /* 5. Wait for conversion time. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_4);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 6. Read measurement results (MEAS_CELLx and MEAS_STACK registers). */
    /* 7. Compute leakage indices IND0_x = MEAS_CELLx, for x = 1 to 6/14. */
    /* 8. Compute leakage index IND0_7/15 = MEAS_STACK.
     * Note: measUv contains IND0_1 at [0][0] and IND0_7/15 at [0][6/14] */
    status = BCC_Meas_GetCellVoltages(drvConfig, cid, &(measUv[0][0]));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Meas_GetStackVoltage(drvConfig, cid, &(measUv[0][deviceCellsMax]));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 9. Write SYS_DIAG[CT_LEAK_DIAG,POLARITY] = 11 to route cell terminal
     * and balancing pins according to the logic of the routing table. */
    status = BCC_Reg_Update(drvConfig, cid,
                            MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_CT_LEAK_DIAG_MASK |
                            MC33771C_SYS_DIAG_POLARITY_MASK,
                            MC33771C_SYS_DIAG_CT_LEAK_DIAG(MC33771C_SYS_DIAG_CT_LEAK_DIAG_DIFF_ENUM_VAL) |
                            MC33771C_SYS_DIAG_POLARITY(MC33771C_SYS_DIAG_POLARITY_INVERTED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 10. Write ADC_CFG[AVG]= 0010b and ADC_CFG[SOC] to initiate a conversion. */
    /* 11. Wait for conversion time. */
    status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_4);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 12. Read measurement results (MEAS_CELLx and MEAS_STACK registers). */
    /* 13. Compute leakage indices IND1_x = MEAS_CELLx, for x = 1 to 14. */
    /* 14. Compute leakage index IND1_15 = MEAS_STACK.
     * Note: measUv contains IND1_1 at [1][0] and IND1_7/15 at [1][6/14] */
    status = BCC_Meas_GetCellVoltages(drvConfig, cid, &(measUv[1][0]));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Meas_GetStackVoltage(drvConfig, cid, &(measUv[1][deviceCellsMax]));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    result->result = 0U;
    for (i = 0U; i <= deviceCellsMax; i++)
    {
        /* 15. Compute V_leakx = MAX(IND0_x, IND1_x), for x=1 to 7/15. */
        result->vLeakX[i] =
                (measUv[0][i] > measUv[1][i]) ? measUv[0][i] : measUv[1][i];

        /* 16. Evaluate the decision criterion, for x=1 to 15:
         * If V_leakx >= V_LEAK then Cell x is leaky, else Cell x is not leaky. */
        if (result->vLeakX[i] >= vLeakThUv)
        {
            result->result |= (uint16_t)((1U) << i);
        }
    }

    /* 17. Clear OV and UV faults. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_OV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CELL_UV_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 18. Clear SYS_DIAG[CT_LEAK_DIAG, POLARITY]. */
    status = BCC_Reg_Update(drvConfig, cid,
                            MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_CT_LEAK_DIAG_MASK |
                            MC33771C_SYS_DIAG_POLARITY_MASK,
                            MC33771C_SYS_DIAG_CT_LEAK_DIAG(MC33771C_SYS_DIAG_CT_LEAK_DIAG_NORMAL_ENUM_VAL) |
                            MC33771C_SYS_DIAG_POLARITY(MC33771C_SYS_DIAG_POLARITY_NONINVERTED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 19. Unpause cell balancing. */
    status = BCC_CB_Pause(drvConfig, cid, false);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 20. Exit diagnostic mode. */
    return BCC_ExitDiagnostics(drvConfig, cid);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_CurrentMeas
 * Description   : This function implements diagnostics of IC internal resources
 *                 for current measurement (SM37, SM38).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_CurrentMeas(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_current_meas_t sel,
    int32_t* const current, bool* const fault)
{
    uint16_t measCurrentRaw[2];  /* Value of MEAS_ISENSE 1 and 2 registers. */
    uint16_t regVal[4];          /* Original value of $03 - $06 registers. */
    /* PGA gain and ADC2 resolution for each bcc_diag_current_meas_t diagnostic
     * measurement. */
    const uint16_t adcCfg[2] = {
            MC33771C_ADC_CFG_ADC2_DEF(MC33771C_ADC_CFG_ADC2_DEF_16_BIT_ENUM_VAL) |
            MC33771C_ADC_CFG_PGA_GAIN(MC33771C_ADC_CFG_PGA_GAIN_256_ENUM_VAL),
            MC33771C_ADC_CFG_ADC2_DEF(MC33771C_ADC_CFG_ADC2_DEF_16_BIT_ENUM_VAL) |
            MC33771C_ADC_CFG_PGA_GAIN(MC33771C_ADC_CFG_PGA_GAIN_4_ENUM_VAL)
    };
    /* Inputs to PGA for each bcc_diag_current_meas_t diagnostic measurement. */
    const uint16_t adcMux[2] = {
            MC33771C_SYS_DIAG_I_MUX(MC33771C_SYS_DIAG_I_MUX_PGA_ZERO_ENUM_VAL),
            MC33771C_SYS_DIAG_I_MUX(MC33771C_SYS_DIAG_I_MUX_VREF_DIAG_ENUM_VAL),
    };
    bcc_status_t status;
    uint8_t i;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(current != NULL);
    BCC_MCU_Assert(fault != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    if ((sel != BCC_DCM_PGA_SHORTED) && (sel != BCC_DCM_VREF_GAIN4))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Store original values of the following registers:
     * [0] SYS_CFG1 ($03),
     * [2] SYS_DIAG ($05)
     * [3] ADC_CFG ($06). */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET, 4U, regVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 1. Disable the current measurement by setting SYS_CFG1[I_MEAS_EN] = 0. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET,
                            (regVal[0] & ~(MC33771C_SYS_CFG1_I_MEAS_EN_MASK)) |
                            MC33771C_SYS_CFG1_I_MEAS_EN(MC33771C_SYS_CFG1_I_MEAS_EN_DISABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Read the Coulomb counter register COULOMB_CNT to retain count information.
     * *** up to user *** */

    /* 3. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Set ADC2 measurement resolution and PGA gain */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_ADC_CFG_OFFSET,
                           (regVal[3] & ~(MC33771C_ADC_CFG_ADC2_DEF_MASK | MC33771C_ADC_CFG_PGA_GAIN_MASK)) | adcCfg[sel]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 4. Configure the current measurement chain for the specific diagnostic
     * source by writing to the SYS_DIAG[I_MUX] bits. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                           (regVal[2] & ~(MC33771C_SYS_DIAG_I_MUX_MASK)) | adcMux[sel]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5. Enable the current measurement by setting SYS_CFG1[I_MEAS_EN] = 1.
     * Note: In order to optimize the speed, BCC_Reg_Write was used instead of
     * BCC_Reg_Update. However, regVal[0] can differ now to SYS_CFG1 register
     * besides the I_MEAS_ENABLED bit also in DIAG_MODE bit, which was earlier
     * set by the BCC_EnterDiagnostics function! */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET,
                           (regVal[0] & ~(MC33771C_SYS_CFG1_I_MEAS_EN_MASK | MC33771C_SYS_CFG1_GO2DIAG_MASK)) |
                           MC33771C_SYS_CFG1_I_MEAS_EN(MC33771C_SYS_CFG1_I_MEAS_EN_ENABLED_ENUM_VAL) |
                           MC33771C_SYS_CFG1_GO2DIAG(MC33771C_SYS_CFG1_GO2DIAG_ENTER_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 6. Wait for the time to perform auto-zero procedure t_AZC_SETTLE. */
    BCC_MCU_WaitUs(BCC_DIAG_T_AZC_SETTLE_US);

    /* To reduce the effect of the noise, it is recommended to cycle N times
     * through steps 7, 8 and 9, to calculate the average of N results.
     * N can be a few units. Alternatively, the coulomb counter can be used
     * to get a filtered value to be compared with the diagnostic threshold,
     * with no need to repeat steps 7, 8, and 9. */
    *current = 0;
    for (i = 0U; i < BCC_DIAG_CURR_MEAS_AVG; i++)
    {
        /* 7. Write ADC_CFG[CC_RST] = 1 and ADC_CFG[SOC] = 1 to reset the coulomb
         * counter COULOMB_CNT and initiate a conversion.
         * Note: This implementation does not use CC => No need to reset it. */
        /* 8. Wait for the conversion time (16 bit resolution). */
        status = BCC_Meas_StartAndWait(drvConfig, cid, BCC_AVG_1);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        /* 9. Read conversion results. */
        status = BCC_Reg_Read(drvConfig, cid, MC33771C_MEAS_ISENSE1_OFFSET, 2U, measCurrentRaw);
        if (status != BCC_STATUS_SUCCESS)
        {
            return status;
        }

        /* Extracts current from measured raw value. */
        *current += BCC_GET_ISENSE_VOLT(measCurrentRaw[0], measCurrentRaw[1]);
    }
    *current /= BCC_DIAG_CURR_MEAS_AVG;

    /* 10. Compare data with the expected result. */
    if (sel == BCC_DCM_PGA_SHORTED)
    {
        /* When the measured value <= Voff_diag -> success. */
       (*fault) = ((*current) > (int32_t)BCC_DIAG_VOFF_MAX);
    }
    else
    {
        /* sel == BCC_DCM_VREF_GAIN4 */
        /* When the measured value is in the range defined by Vref_diag then success. */
        if (drvConfig->device[(uint8_t)cid - 1] == BCC_DEVICE_MC33771C)
        {
            *fault = !BCC_IS_IN_RANGE(*current, MC33771C_DIAG_VREF_MIN, MC33771C_DIAG_VREF_MAX);
        }
        else
        {
            *fault = !BCC_IS_IN_RANGE(*current, MC33772C_DIAG_VREF_MIN, MC33772C_DIAG_VREF_MAX);
        }
    }

    /* 11. Exit diagnostic mode. */
    status = BCC_ExitDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Restore original values of registers. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET, regVal[0]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET, regVal[2]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 12. Reset Coulomb counter and restore previous ADC2 settings. */
    return BCC_Reg_Write(drvConfig, cid, MC33771C_ADC_CFG_OFFSET,
            regVal[3] | MC33771C_ADC_CFG_CC_RST(MC33771C_ADC_CFG_CC_RST_RESET_ENUM_VAL));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_ShuntConn
 * Description   : This function verifies whether the shunt resistor is properly
 *                 connected to the current channel low-pass filter (SM36).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_ShuntConn(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_const_t* const diagTimeConst,
    bool* const shuntConn)
{
    uint16_t fault1stVal;      /* Read value of FAULT1_STATUS register. */
    uint16_t sysCfg1Val;
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(diagTimeConst != NULL);
    BCC_MCU_Assert(shuntConn != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* 1. Disable current measurement. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET, 1U, &sysCfg1Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET,
                           (sysCfg1Val & ~(MC33771C_SYS_CFG1_I_MEAS_EN_MASK)) |
                           MC33771C_SYS_CFG1_I_MEAS_EN(MC33771C_SYS_CFG1_I_MEAS_EN_DISABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Clear faults. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_FAULT1_STATUS_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Read the Coulomb counter COULOMB_CNT to retain count information
     * *** up to user *** */

    /* 3. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 4. Configure current measurement chain for the open detection check. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_ISENSE_OL_DIAG_MASK,
                            MC33771C_SYS_DIAG_ISENSE_OL_DIAG(MC33771C_SYS_DIAG_ISENSE_OL_DIAG_ENABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5. Wait for the diagnostic time constant. */
    BCC_MCU_WaitUs(diagTimeConst->sm36tdiag);

    /* 6. Read FAULT1_STATUS[IS_OL_FLT] flag. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_FAULT1_STATUS_OFFSET, 1U, &fault1stVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 7. Configure current measurement chain for the open detection check
     * by setting SYS_DIAG[I_SENSE_OL_DIAG] to logic 0. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_ISENSE_OL_DIAG_MASK,
                            MC33771C_SYS_DIAG_ISENSE_OL_DIAG(MC33771C_SYS_DIAG_ISENSE_OL_DIAG_DISABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 8. Wait for K times the current measurement time constant tau_I. */
    BCC_MCU_WaitUs(diagTimeConst->sm36trecv);

    /* 9. Return state of the current measurement (enabled/disabled) to the previous state
     * and exit diagnostic mode by setting SYS_CFG1[GO2DIAG] to logic 0. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_SYS_CFG1_OFFSET,
                           (sysCfg1Val & ~(MC33771C_SYS_CFG1_GO2DIAG_MASK)) |
                           MC33771C_SYS_CFG1_GO2DIAG(MC33771C_SYS_CFG1_GO2DIAG_EXIT_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 10. Reset Coulomb counter. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_ADC_CFG_OFFSET,
                            MC33771C_ADC_CFG_CC_RST_MASK,
                            MC33771C_ADC_CFG_CC_RST(MC33771C_ADC_CFG_CC_RST_RESET_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Evaluate / Publish results. */
    (*shuntConn) = ((fault1stVal & MC33771C_FAULT1_STATUS_IS_OL_FLT_MASK) ==
            MC33771C_FAULT1_STATUS_IS_OL_FLT(MC33771C_FAULT1_STATUS_IS_OL_FLT_NO_OPEN_LOAD_ENUM_VAL));

    /* Clear faults. */
    return BCC_Reg_Write(drvConfig, cid, MC33771C_FAULT1_STATUS_OFFSET, 0U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_GPIOxOtUt
 * Description   : This function implements GPIOx OT/UT functional verification
 *                 (SM05).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_GPIOxOtUt(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_gpiox_otut_res_t* const results)
{
    uint16_t regVal;          /* Read value of a GPIO_CFGx register. */
    uint16_t gpioCfgVal[2];   /* Original value of GPIO_CFG1 and GPIO_CFG2 registers. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(results != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Clear OT/UT faults. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_AN_OT_UT_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 1. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Store original values of GPIO_CFG1 and GPIO_CFG2 registers. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_GPIO_CFG1_OFFSET, 2U, gpioCfgVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Optionally program GPIOx to be tested as analog through GPIO_CFG1[GPIOx_CFG]
     * register. Only the GPIOx configured as analog inputs have buffers than
     * activated in diagnostic mode by the ANx_TEMP_DIAG bit. */
    regVal = MC33771C_GPIO_CFG1_GPIO0_CFG(MC33771C_GPIO_CFG1_GPIO0_CFG_ANALOG_RATIO_ENUM_VAL) | 
             MC33771C_GPIO_CFG1_GPIO1_CFG(MC33771C_GPIO_CFG1_GPIO1_CFG_ANALOG_RATIO_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO2_CFG(MC33771C_GPIO_CFG1_GPIO2_CFG_ANALOG_RATIO_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO3_CFG(MC33771C_GPIO_CFG1_GPIO3_CFG_ANALOG_RATIO_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO4_CFG(MC33771C_GPIO_CFG1_GPIO4_CFG_ANALOG_RATIO_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO5_CFG(MC33771C_GPIO_CFG1_GPIO5_CFG_ANALOG_RATIO_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO6_CFG(MC33771C_GPIO_CFG1_GPIO6_CFG_ANALOG_RATIO_ENUM_VAL);
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_GPIO_CFG1_OFFSET, regVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 4. Enable the GPIOx output buffer through the SYS_DIAG[ANx_TEMP_DIAG]. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_ANX_TEMP_DIAG_MASK,
                            MC33771C_SYS_DIAG_ANX_TEMP_DIAG(MC33771C_SYS_DIAG_ANX_TEMP_DIAG_ENABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 3. Set GPIO_CFG2[GPIOx_DR] register to drive output high to emulate UT. */
    /* 5. Request the IC to perform a conversion sequence by writing to ADC_CFG register. */
    /* 6. Conversions below the TH_ANx_OT threshold trigger the ANx_OT fault bit.
     * Conversions above the TH_ANx_UT threshold trigger the ANx_UT fault bit. */
    regVal = MC33771C_GPIO_CFG2_GPIO0_DR(MC33771C_GPIO_CFG2_GPIO0_DR_HIGH_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO1_DR(MC33771C_GPIO_CFG2_GPIO1_DR_HIGH_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO2_DR(MC33771C_GPIO_CFG2_GPIO2_DR_HIGH_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO3_DR(MC33771C_GPIO_CFG2_GPIO3_DR_HIGH_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO4_DR(MC33771C_GPIO_CFG2_GPIO4_DR_HIGH_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO5_DR(MC33771C_GPIO_CFG2_GPIO5_DR_HIGH_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO6_DR(MC33771C_GPIO_CFG2_GPIO6_DR_HIGH_ENUM_VAL);

    status = BCC_GpioOtUtPart(drvConfig, cid, regVal, &(results->untStat));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 3. Set GPIO_CFG2[GPIOx_DR] register to drive output low to emulate OT. */
    /* 5. Request BCC to perform a conversion sequence by writing to ADC_CFG register. */
    /* 6. Conversions below the TH_ANx_OT threshold trigger the ANx_OT fault bit.
     * Conversions above the TH_ANx_UT threshold trigger the ANx_UT fault bit. */
    regVal = MC33771C_GPIO_CFG2_GPIO0_DR(MC33771C_GPIO_CFG2_GPIO0_DR_LOW_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO1_DR(MC33771C_GPIO_CFG2_GPIO1_DR_LOW_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO2_DR(MC33771C_GPIO_CFG2_GPIO2_DR_LOW_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO3_DR(MC33771C_GPIO_CFG2_GPIO3_DR_LOW_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO4_DR(MC33771C_GPIO_CFG2_GPIO4_DR_LOW_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO5_DR(MC33771C_GPIO_CFG2_GPIO5_DR_LOW_ENUM_VAL) |
             MC33771C_GPIO_CFG2_GPIO6_DR(MC33771C_GPIO_CFG2_GPIO6_DR_LOW_ENUM_VAL);

    status = BCC_GpioOtUtPart(drvConfig, cid, regVal, &(results->ovtStat));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Disable GPIOx output buffer. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_ANX_TEMP_DIAG_MASK,
                            MC33771C_SYS_DIAG_ANX_TEMP_DIAG(MC33771C_SYS_DIAG_ANX_TEMP_DIAG_DISABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Restore original content of registers. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_GPIO_CFG1_OFFSET, gpioCfgVal[0]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    status = BCC_Reg_Write(drvConfig, cid, MC33771C_GPIO_CFG2_OFFSET, gpioCfgVal[1]);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Clear FAULT1_STATUS[AN_OT_FLT, AN_UT_FLT] bits.
     * Note: All the AN_OT_UT[Anx_OT, Anx_UT] bits were cleared already in the
     *       BCC_GpioOtUtPart function. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_FAULT1_STATUS_OFFSET,
             MC33771C_FAULT1_STATUS_AN_UT_FLT_MASK | MC33771C_FAULT1_STATUS_AN_OT_FLT_MASK, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 7. Exit diagnostic mode. */
    status = BCC_ExitDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Evaluate the results. */
    if ((results->untStat & (BCC_AN_UT_ALL_MASK | BCC_AN_OT_ALL_MASK)) != BCC_AN_UT_ALL_MASK)
    {
        results->error = true;
    }
    else if ((results->ovtStat & (BCC_AN_UT_ALL_MASK | BCC_AN_OT_ALL_MASK)) != BCC_AN_OT_ALL_MASK)
    {
        results->error = true;
    }
    else
    {
        results->error = false;
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_GPIOxOpen
 * Description   : This function implements GPIOx open terminal diagnostics
 *                 (SM06).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_GPIOxOpen(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint16_t* const openStatus)
{
    uint16_t gpioCfg1Val;  /* Original value of GPIO_CFG1 register. */
    uint16_t regVal;       /* Value of GPIO_CFG1 register. */
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(openStatus != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* Clear GPIOx short flags. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_GPIO_SHORT_ANX_OPEN_STS_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 1. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Store original value of GPIO_CFG1 register. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_GPIO_CFG1_OFFSET, 1U, &gpioCfg1Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. Program GPIOx to be tested as analog input. */
    regVal = MC33771C_GPIO_CFG1_GPIO0_CFG(MC33771C_GPIO_CFG1_GPIO0_CFG_ANALOG_ABS_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO1_CFG(MC33771C_GPIO_CFG1_GPIO1_CFG_ANALOG_ABS_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO2_CFG(MC33771C_GPIO_CFG1_GPIO2_CFG_ANALOG_ABS_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO3_CFG(MC33771C_GPIO_CFG1_GPIO3_CFG_ANALOG_ABS_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO4_CFG(MC33771C_GPIO_CFG1_GPIO4_CFG_ANALOG_ABS_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO5_CFG(MC33771C_GPIO_CFG1_GPIO5_CFG_ANALOG_ABS_ENUM_VAL) |
             MC33771C_GPIO_CFG1_GPIO6_CFG(MC33771C_GPIO_CFG1_GPIO6_CFG_ANALOG_ABS_ENUM_VAL);
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_GPIO_CFG1_OFFSET, regVal);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 3. Activate GPIOx weak pull-down.
     * Note: In Diagnostic mode, only GPIOx configured analog have a weak
     * pull-down activated by the ANx_OL_DIAG bit. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_ANX_OL_DIAG_MASK,
                            MC33771C_SYS_DIAG_ANX_OL_DIAG(MC33771C_SYS_DIAG_ANX_OL_DIAG_ENABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 4. Voltages below the VOL(TH) threshold set the GPIO_SHORT_AN_OPEN_STS
     * [ANx_OPEN] bit and the FAULT2_STATUS[AN_OPEN_FLT] fault bit. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_GPIO_SHORT_ANX_OPEN_STS_OFFSET, 1U, openStatus);
    *openStatus &= BCC_AN_OPEN_STS_ALL_MASK;
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5. Deactivate GPIOx weak pull down through the SYS_DIAG[ANx_OL_DIAG]. */
    status = BCC_Reg_Update(drvConfig, cid, MC33771C_SYS_DIAG_OFFSET,
                            MC33771C_SYS_DIAG_ANX_OL_DIAG_MASK, 
                            MC33771C_SYS_DIAG_ANX_OL_DIAG(MC33771C_SYS_DIAG_ANX_OL_DIAG_DISABLED_ENUM_VAL));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Restore original content of GPIO_CFG1 register. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_GPIO_CFG1_OFFSET, gpioCfg1Val);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 6. Exit diagnostic mode. */
    status = BCC_ExitDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Clear GPIOx short flags. */
    return BCC_Reg_Write(drvConfig, cid, MC33771C_GPIO_SHORT_ANX_OPEN_STS_OFFSET, 0U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_Diag_CBxOpen
 * Description   : This function implements Cell balance open load detection
 *                 (SM40).
 *
 *END**************************************************************************/
bcc_status_t BCC_Diag_CBxOpen(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_cbx_open_res_t* const results)
{
    bcc_status_t status;

    BCC_MCU_Assert(drvConfig != NULL);
    BCC_MCU_Assert(results != NULL);

    if ((cid == BCC_CID_UNASSIG) || (((uint8_t)cid) > drvConfig->devicesCnt))
    {
        return BCC_STATUS_PARAM_RANGE;
    }

    /* 1. Enter diagnostic mode. */
    status = BCC_EnterDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 2. If the number of cells on the cluster is odd, then write the bit
     * SYS_CFG2[NUMB_ODD] to logic 1, else write it to logic 0. */
    status = BCC_SetNumbOdd(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 3. Pause cell balancing by setting SYS_CFG1[CB_MANUAL_PAUSE] to logic 1. */
    status = BCC_CB_Pause(drvConfig, cid, true);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 4. Command the SYS_DIAG[CB_OL_ODD,CB_OL_EVEN] field to the 10 configuration. */
    status = BCC_CommandSwitches(drvConfig, cid, BCC_SWITCH_SEL_CB,
                                 BCC_SWITCH_POS_CLOSED, BCC_SWITCH_POS_OPEN);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 5. Wait for the time delay t_delay. */
    BCC_MCU_WaitUs(BCC_DIAG_T_DELAY_US);

    /* 6. Read CB_OPEN_FLT to determine all CBx_OPEN_FLT open load faut bits. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_CB_OPEN_FLT_OFFSET, 1U, &(results->cbxOpenStatusEven));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 7. Command the SYS_DIAG[CB_OL_ODD,CB_OL_EVEN] field to the 01 configuration. */
    status = BCC_CommandSwitches(drvConfig, cid, BCC_SWITCH_SEL_CB,
                                 BCC_SWITCH_POS_OPEN, BCC_SWITCH_POS_CLOSED);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 8. Wait for the time delay t_delay. */
    BCC_MCU_WaitUs(BCC_DIAG_T_DELAY_US);

    /* 9. Read CB_OPEN_FLT to determine all CBx_OPEN_FLT open load faut bits. */
    status = BCC_Reg_Read(drvConfig, cid, MC33771C_CB_OPEN_FLT_OFFSET, 1U, &(results->cbxOpenStatusOdd));
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 10. Command the SYS_DIAG[CB_OL_ODD,CB_OL_EVEN] field to the 00 configuration. */
    status = BCC_CommandSwitches(drvConfig, cid, BCC_SWITCH_SEL_CB,
                                BCC_SWITCH_POS_OPEN, BCC_SWITCH_POS_OPEN);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 11. Unpause cell balancing. */
    status = BCC_CB_Pause(drvConfig, cid, false);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* 12. Exit diagnostic mode. */
    status = BCC_ExitDiagnostics(drvConfig, cid);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Clear CB OL faults. */
    status = BCC_Reg_Write(drvConfig, cid, MC33771C_CB_OPEN_FLT_OFFSET, 0U);
    if (status != BCC_STATUS_SUCCESS)
    {
        return status;
    }

    /* Evaluate the diagnostics. */
    if ((results->cbxOpenStatusEven & drvConfig->drvData.cellMap[(uint8_t)cid - 1]) ||
        (results->cbxOpenStatusOdd & drvConfig->drvData.cellMap[(uint8_t)cid - 1]))
    {
        results->error = true;
    }
    else
    {
        results->error = false;
    }

    return BCC_STATUS_SUCCESS;
}
