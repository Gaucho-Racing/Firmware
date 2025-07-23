/*!
 * @file bcc_diagnostics.h
 *
 * Diagnostics part of Battery cell controller SW driver for MC33771C and
 * MC33772C v2.2.
 */

#ifndef __BCC_DIAGNOSTICS_H__
#define __BCC_DIAGNOSTICS_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "bcc.h"

/*******************************************************************************
 * User definitions
 ******************************************************************************/

/*! @brief Minimal measured voltage (in [uV]) of contacts with abnormally high
 *  resistance, a default value for MC33771C. See safety manual for your use
 *  case. */
#define MC33771C_DIAG_DV          33000

/*! @brief Minimal measured voltage (in [uV]) of contacts with abnormally high
 *  resistance, a default value for MC33772C. See safety manual for your use
 *  case. */
#define MC33772C_DIAG_DV          37600

/*! @brief Number of ISENSE measurements to be averaged in SM37 and SM38. */
#define BCC_DIAG_CURR_MEAS_AVG    4

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief ISENSE diagnostic common mode offset voltage (in [uV]),
 *  maximal value. */
#define BCC_DIAG_VOFF_MAX         37U
/*! @brief MC33771C ISENSE diagnostic reference (in [uV])
 *  with PGA having gain 4, minimal value. */
#define MC33771C_DIAG_VREF_MIN    124000
/*! @brief MC33771C ISENSE diagnostic reference (in [uV])
 *  with PGA having gain 4, typical value. */
#define MC33771C_DIAG_VREF_TYP    127000
/*! @brief MC33771C ISENSE diagnostic reference (in [uV])
 *  with PGA having gain 4, maximal value. */
#define MC33771C_DIAG_VREF_MAX    130000
/*! @brief MC33772C ISENSE diagnostic reference (in [uV])
 *  with PGA having gain 4, minimal value. */
#define MC33772C_DIAG_VREF_MIN    123500
/*! @brief MC33772C ISENSE diagnostic reference (in [uV])
 *  with PGA having gain 4, typical value. */
#define MC33772C_DIAG_VREF_TYP    126500
/*! @brief MC33772C ISENSE diagnostic reference (in [uV])
 *  with PGA having gain 4, maximal value. */
#define MC33772C_DIAG_VREF_MAX    129500

/*! @brief Cell terminal open load V detection threshold (in [uV]),
 *  Type T (1.5 V <= V_CELL <= 2.7 V). */
#define BCC_DIAG_V_OL_DETECT_T    50000U
/*! @brief Cell terminal open load V detection threshold (in [uV]),
 *  Type F (2.5 V <= V_CELL <= 3.7 V). */
#define BCC_DIAG_V_OL_DETECT_F    100000U
/*! @brief Cell terminal open load V detection threshold (in [uV]),
 *  Type N (2.5 V <= V_CELL <= 4.3 V). */
#define BCC_DIAG_V_OL_DETECT_N    150000U

/*! @brief Undervoltage functional verification threshold (in [mV]),
 *  Type T (1.5 V <= V_CELL <= 2.7 V). */
#define BCC_DIAG_CTX_UV_TH_T      390U
/*! @brief Undervoltage functional verification threshold (in [mV]),
 *  Type F (2.5 V <= V_CELL <= 3.7 V). */
#define BCC_DIAG_CTX_UV_TH_F      650U
/*! @brief Undervoltage functional verification threshold (in [mV]),
 *  Type N (2.5 V <= V_CELL <= 4.3 V). */
#define BCC_DIAG_CTX_UV_TH_N      1200U
/*! @brief Overvoltage functional verification threshold (in [mV]),
 *  Type T (1.5 V <= V_CELL <= 2.7 V). */
#define BCC_DIAG_CTX_OV_TH_T      1800U
/*! @brief MC33771C Overvoltage functional verification threshold (in [mV]),
 *  Type F (2.5 V <= V_CELL <= 3.7 V). */
#define MC33771C_DIAG_CTX_OV_TH_F 4000U
/*! @brief MC33772C Overvoltage functional verification threshold (in [mV]),
 *  Type F (2.5 V <= V_CELL <= 3.7 V). */
#define MC33772C_DIAG_CTX_OV_TH_F 3500U
/*! @brief Overvoltage functional verification threshold (in [mV]),
 *  Type N (2.5 V <= V_CELL <= 4.3 V). */
#define BCC_DIAG_CTX_OV_TH_N      4000U

/*! @brief Voltage reference (in [uV]) used in ADC1-A,B functional verification. */
#define BCC_DIAG_V_BGP            1180000U
/*! @brief MC33771C Maximum tolerance (in [uV]) between ADC1-A,B and diagnostic
 *  reference (1.5 V <= VCELL <= 4.3 V) in ADC1-A,B functional verification,
 *  absolute value. */
#define MC33771C_DIAG_ADC1X_FV    5250U
/*! @brief MC33772C Maximum tolerance (in [uV]) between ADC1-A,B and diagnostic
 *  reference (1.5 V <= VCELL <= 4.3 V) in ADC1-A,B functional verification,
 *  absolute value. */
#define MC33772C_DIAG_ADC1X_FV    6660U

/*! @brief Cell voltage channel functional verification allowable error
 *  in CT verification measurement, minimal value (in [uV]). */
#define BCC_DIAG_VCVFV_MIN        -22000
/*! @brief Cell voltage channel functional verification allowable error
 *  in CT verification measurement, maximal value (in [uV]). */
#define BCC_DIAG_VCVFV_MAX        6000

/*! @brief MC33771 Cell terminal leakage detection level (in [uV]).
 *  This value can be used also as negative. */
#define MC33771C_DIAG_VLEAK       27000U
/*! @brief MC33772 Cell terminal leakage detection level (in [uV]).
 *  This value can be used also as negative. */
#define MC33772C_DIAG_VLEAK       15000U

/* Enum types definition. */
/*!
 * @addtogroup enum_group
 * @{
 */
/*! @brief Selection between Cell terminal and Cell balancing diagnostic
    switches. */
typedef enum
{
    BCC_SWITCH_SEL_CT         = 0U,  /*!< Cell terminal switches. */
    BCC_SWITCH_SEL_CB         = 1U   /*!< Cell balancing switches. */
} bcc_diag_switch_sel_t;

/*! @brief Selection between opened and closed diagnostic switches. */
typedef enum
{
    BCC_SWITCH_POS_OPEN       = 0U,  /*!< Opened switches. */
    BCC_SWITCH_POS_CLOSED     = 1U   /*!< Closed switches. */
} bcc_diag_switch_pos_t;

/*! @brief Selection of diagnostic type and source of ADC2 for Current
 *  measurement diagnostics. */
typedef enum
{
    BCC_DCM_PGA_SHORTED       = 0U,  /*!< Measuring the PGA with shorted input (SM37). */
    BCC_DCM_VREF_GAIN4        = 1U   /*!< Measuring of VREF_DIAG, with the PGA having
                                          the gain fixed to 4 (SM38). */
} bcc_diag_current_meas_t;
/*! @} */

/*!
 * @addtogroup struct_group
 * @{
 */
/*! @brief Diagnostic time constants. See MC3377xC safety manuals for more information. */
typedef struct
{
    uint32_t sm01twait;       /*!< SM01 time constant T_wait (in [us]). */
    uint32_t sm01trecv;       /*!< SM01 time constant T_recv (in [us]). */
    uint16_t sm01uvTh;        /*!< SM01 diagnostic UV threshold (in [mV]).
                                   You can use BCC_DIAG_CTX_UV_TH_* macros. */
    uint16_t sm01ovTh;        /*!< SM01 diagnostic OV threshold (in [mV]).
                                   You can use *_DIAG_CTX_OV_TH_* macros. */
    uint32_t sm02twait;       /*!< SM02 time constant T_wait (in [us]). */
    uint32_t sm02trecv;       /*!< SM02 time constant T_recv (in [us]). */
    uint32_t sm02uvTh;        /*!< SM02 diagnostic UV threshold (in [uV]). */
    uint32_t sm02vcellMin;    /*!< SM02 min(Vcell) (in [uV]). */
    uint32_t sm02voldetect;   /*!< SM02 V_OL_DETECT threshold (Cell terminal
                                   open load V detection threshold) (in [uV]).
                                   You can use BCC_DIAG_V_OL_DETECT_* macros. */
    uint32_t sm34uvTh;        /*!< SM34 UV detection threshold (in [uV]). May be
                                   equal to the minimum cell voltage minus 50 mV. */
    uint32_t sm34ovTh;        /*!< SM34 OV detection threshold (in [uV]). May be
                                   equal to the maximum cell voltage plus 50 mV. */
    uint32_t sm36tdiag;       /*!< SM36 time constant t_diag (in [us]). */
    uint32_t sm36trecv;       /*!< SM36 time constant K * tau_I (in [us]). */
} bcc_diag_const_t;

/*! @brief Result of ADC1-A and ADC1-B functional verification. */
typedef struct
{
    uint32_t adc1aAvg;        /*!< Average of ADC1-A measured values (in [uV]). */
    uint32_t adc1bAvg;        /*!< Average of ADC1-B measured values (in [uV]). */
    bool error;               /*!< True if error detected, False otherwise. */
} bcc_diag_adc1x_res_t;

/*! @brief Result of overvoltage and undervoltage functional verification. */
typedef struct
{
    uint16_t ovOdd;           /*!< Content of CELL_OV_FLT register,
                                   OV fault is expected on odd cells. */
    uint16_t uvEven;          /*!< Content of CELL_UV_FLT register,
                                   UV fault is expected on even cells. */
    uint16_t ovEven;          /*!< Content of CELL_OV_FLT register,
                                   OV fault is expected on even cells. */
    uint16_t uvOdd;           /*!< Content of CELL_UV_FLT register,
                                   UV fault is expected on odd cells. */
    bool error;               /*!< True if error detected, False otherwise. */
} bcc_diag_ov_uv_ver_res_t;

/*! @brief Result overvoltage and undervoltage detection. */
typedef struct
{
    uint32_t cellVoltCbOff[BCC_MAX_CELLS];  /*!< Measured cell voltages (in [uV]) when CB off.
                                                 MC33771C: [0] Cell 1, .., [13] Cell 14.
                                                 MC33772C: [0] Cell 1, .., [5] Cell 6. */
    uint32_t cellVoltCbOn[BCC_MAX_CELLS];   /*!< Measured cell voltages (in [uV]) when CB on.
                                                 MC33771C: [0] Cell 1, .., [13] Cell 14.
                                                 MC33772C: [0] Cell 1, .., [5] Cell 6. */
    bool error;                             /*!< True if error detected, False otherwise. */
} bcc_diag_ov_uv_det_res_t;

/*! @brief Result of the CTx open detect and open detect functional verification. */
typedef struct
{
    uint32_t measOddClosed[BCC_MAX_CELLS];  /*!< Measured cell voltages (in [uV]) when odd CT open
                                                 terminal switches are closed.
                                                 MC33771C: [0] Cell 1, .., [13] Cell 14.
                                                 MC33772C: [0] Cell 1, .., [5] Cell 6. */
    uint32_t measEvenClosed[BCC_MAX_CELLS]; /*!< Measured cell voltages (in [uV]) when even CT open
                                                 terminal switches are closed.
                                                 MC33771C: [0] Cell 1, .., [13] Cell 14.
                                                 MC33772C: [0] Cell 1, .., [5] Cell 6. */
    uint16_t ctxOpen;                       /*!< Bit map representing open terminal status. Zero if no
                                                 CTx was detected open, non-zero otherwise.
                                                 Bit value "1" at 0th bit means open CT1,
                                                 Bit value "1" at 1st bit means open CT2, etc.
                                                 Faults at CTx of unused cells are ignored. */
    uint16_t swxOpen;                       /*!< Bit map representing open SWx. Zero if no
                                                 error, non-zero otherwise.
                                                 0th bit is related to CT1, 1st to CT2, etc.
                                                 Faults at CTx of unused cells are ignored. */
    uint16_t swxShort;                      /*!< Bit map representing short SWx. Zero if no
                                                 error, non-zero otherwise.
                                                 0th bit is related to CT1, 1st to CT2, etc.
                                                 Faults at CTx of unused cells are ignored. */
} bcc_diag_ctx_open_res_t;

/*! @brief Result of the cell voltage channel functional verification. */
typedef struct
{
    int32_t vErrX[BCC_MAX_CELLS];           /*!< Computed V_err_x errors (in [uV]).
                                                 MC33771C: [0] V_err_1, .., [13] V_err_14.
                                                   If an error in V_err_x is detected, vX and vDiffX
                                                   are not relevant in this structure.
                                                 MC33772C: [0] V_err_3, .., [13] V_err_5. */
    int32_t vX[BCC_MAX_CELLS];              /*!< Computed V_x errors (in [uV]).
                                                 MC33771C: [0] V_1, .., [13] V_14.
                                                 MC33772C: Not used. */
    int32_t vDiffX[BCC_MAX_CELLS];          /*!< Computed (V_err_x - V_x) in [uV].
                                                 MC33771C: [0] (V_err_1 - V_1), .., [13] (V_err_14 - V_14).
                                                 MC33772C: Not used. */
    uint16_t result;                        /*!< Bit map representing errors detected in V_err_x
                                                 or |V_err_x - V_x|.
                                                 MC33771C:
                                                   0th bit: V_err_1 or |V_err_1 - V_1|, ...
                                                   13th bit: V_err_14 or |V_err_14 - V_14|
                                                 MC33772:
                                                   0th bit: V_err_3,
                                                   1st bit: V_err_5.
                                                 Bit value 0: OK. Bit value 1: Error detected.
                                                 If result is zero, no error was detected. */
} bcc_diag_cell_volt_res_t;

/*! @brief Result of the procedure for detecting a connector having
 * an abnormally high contact resistance. */
typedef struct
{
    int32_t diff[BCC_MAX_CELLS];            /*!< Voltage difference between CB ON and CB OFF
                                                 (Vcell(CB=ON) - Vcell(CB=OFF)) in [uV].
                                                 MC33771C: [0] Cell 1, .., [13] Cell 14.
                                                 MC33772C: [0] Cell 1, .., [5] Cell 6. */
    uint16_t result;                        /*!< Bit map representing cells with a high cell voltage
                                                 difference between CB ON and CB OFF.
                                                 0th bit: Cell 1, ...,
                                                 MC33771C: 13th bit: Cell 14.
                                                 MC33772C: 5th bit: Cell 6.
                                                 Bit value 0: OK. Bit value 1: High resistance.
                                                 If result is zero, no error was detected. */
} bcc_diag_conn_res_res_t;

/*! @brief Result of the cell terminal leakage diagnostics. */
typedef struct
{
    uint32_t vLeakX[BCC_MAX_CELLS + 1U];    /*!< Vleak_x in [uV].
                                                 [0] CT_REF (Vleak_1), [1] CT1 (Vleak_2), ...,
                                                 MC33771C: [14] CT14 (Vleak_15).
                                                 MC33772C: [6] CT6 (Vleak_7). */
    uint16_t result;                        /*!< Bit map representing leakage status on CTx terminals.
                                                 0th bit: CT_REF, 1st bit: CT_1, ...,
                                                 MC33771C: 14th bit: CT14.
                                                 MC33772C: 6th bit: CT6.
                                                 Bit value 0: CT not leaky. Bit value 1: CT is leaky.
                                                 If result is zero, no error was detected. */
} bcc_diag_ctx_leak_res_t;

/*! @brief Result of GPIOx OT/UT functional verification. */
typedef struct
{
    uint16_t untStat;      /*!< Contains value of AN_OT_UT_FLT register when
                                under-temperature is expected for all GPIOs). */
    uint16_t ovtStat;      /*!< Contains value of AN_OT_UT_FLT register when
                                over-temperature is expected for all GPIOs). */
    bool error;            /*!< True if error detected, False otherwise. */
} bcc_diag_gpiox_otut_res_t;

/*! @brief Result of the cell balance fault diagnostics. */
typedef struct
{
    uint16_t cbxOpenStatusEven;  /*!< Contains CB_OPEN_FLT register when even
                                      CB open detection switches are closed. */
    uint16_t cbxOpenStatusOdd;   /*!< Contains CB_OPEN_FLT register when odd
                                      CB open detection switches are closed. */
    bool error;                  /*!< True if error detected, False otherwise.
                                      Faults at CBx of unused cells are ignored. */
} bcc_diag_cbx_open_res_t;
/*! @} */

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @addtogroup function_group
 * @{
 */
/*!
 * @brief This function implements the ADC1-A and ADC1-B functional verification
 * (SM07). An on-demand conversion (with averaging of 8 samples) is performed
 * and the measured values from  MEAS_VBG_DIAG_ADC1A and MEAS_VBG_DIAG_ADC1B
 * registers are compared with the expected ones.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param results   Pointer to structure with results of diagnostics. See
 *                  definition of this structure in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_ADC1(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_adc1x_res_t* const results);

/*!
 * @brief This function implements OV/UV functional verification (SM01) through
 * digital comparators against tunable thresholds. This can be done by forcing
 * an OV/UV condition on cell terminal pins with use of diagnostic switches.
 *
 * Note that OV/UV thresholds are temporary changed to CTx_OV_TH and CTx_UV_TH
 * values from BCC datasheets according to the battery type.
 *
 * Ensure that the safety mechanism runs with deactivated cell balancing.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param diagConst Pointer to structure with diagnostic time constants.
 * @param results   Pointer to structure with results of diagnostics. See
 *                  definition of this structure in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_OvUvVer(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_const_t* const diagConst,
    bcc_diag_ov_uv_ver_res_t* const results);

/*!
 * @brief This function implements OV and UV detection in the pack controller
 * (SM34). The purpose of running the present safety mechanism is not to monitor
 * the cell voltage, but to detect faults which may occur on the cell terminal
 * pins, e.g. due to a short circuit across a CT pin and a CB pin. This function
 * checks the cell voltage values are contained in a proper range under the
 * following conditions:
 * A. with all CB switches turned off, to detect the CT_2N-1 pin shorted to the
 * CB_2N-1 pin;
 * B. With all CB switches turned on, to detect the CT_2N pin shorted to the
 * CB_2N pin.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param diagConst Pointer to structure with diagnostic constants.
 * @param results   Pointer to structure with results of diagnostics. See
 *                  definition of this structure in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_OvUvDet(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_const_t* const diagConst,
    bcc_diag_ov_uv_det_res_t* const results);

/*!
 * @brief This function implements CTx open detection and functional
 * verification (SM02). CTx open/CT SWx open/CT SWx short detection is achieved
 * by measuring the cell voltages separately for even SWx short and odd SWx
 * short and by comparing the measured values with diagnostic thresholds.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param diagConst Pointer to structure with diagnostic constants.
 * @param results   Pointer to structure with results of diagnostics. See
 *                  definition of this structure in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_CTxOpen(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_const_t* const diagConst,
    bcc_diag_ctx_open_res_t* const results);

/*!
 * @brief This function implements Cell Voltage Channel functional verification
 * (SM03). Purpose of this verification is to check that gain variations
 * introduced by multiplexers (used to route CTx pins to ADC1-A,B) are small
 * compared to the unity. The diagnostics disconnects the cell terminal input
 * circuitry and places a precision Zener reference on the input to each
 * differential amplifier attenuator to verify the integrity of the level
 * shifting differential amplifier, attenuator and multiplexer chain. Unused
 * cell voltage channels are skipped.
 *
 * See the MC33771C safety manual for assumption of minimum cell voltage for
 * this verification.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param results   Pointer to structure with results of diagnostics. See
 *                  definition of this structure in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_CellVolt(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_cell_volt_res_t* const results);

/*!
 * @brief This function detects a connector having an abnormally high contact
 * resistance. It is a part of cell terminals and cell balancing terminals
 * leakage diagnostics (SM04).
 *
 * The second part of SM04 is implemented in function BCC_Diag_CTxLeak.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param result    Pointer to structure with results of diagnostics.
 *                  See definition of this structure in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_ConnResistance(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_conn_res_res_t* const result);

/*!
 * @brief This function detects a leakage current. It is a part of cell
 * terminals and cell balancing terminals leakage diagnostics (SM04).
 *
 * The first part of SM04 is implemented in function BCC_Diag_ConnResistance.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param result    Pointer to structure with results of diagnostics.
 *                  See definition of this structure in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_CTxLeak(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_ctx_leak_res_t* const result);

/*!
 * @brief This function implements diagnostics of IC internal resources for
 * current measurement (SM37, SM38). It verifies integrity of current
 * measurement chain. It implements two different diagnostics selected by
 * parameter "sel": SM37 (measuring the PGA with shorted input to detect if
 * there is an abnormal offset in the current chain) and SM38 (measuring
 * a large and precise voltage reference, VREF_DIAG, with the PGA having the
 * gain fixed to 4).
 *
 * Note that this diagnostics resets the coulomb counter. Read coulomb counter
 * COULOMB_CNT to retain count information before calling this function.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param sel       Selection of the diagnostics. See definition of this
 *                  enumeration in bcc_diagnostics.h header file.
 * @param current   Measured ISENSE voltage (in [uV]).
 * @param fault     True (faulted condition) / false (un-faulted condition).
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_CurrentMeas(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_current_meas_t sel,
    int32_t* const current, bool* const fault);

/*!
 * @brief This function verifies whether the shunt resistor is properly
 * connected to the current channel low-pass filter (SM36).
 *
 * Note that this diagnostics resets the coulomb counter. Read coulomb counter
 * COULOMB_CNT to retain count information before calling this function.
 *
 * @param drvConfig     Pointer to driver instance configuration.
 * @param cid           Cluster Identification Address of the BCC device.
 * @param diagTimeConst Pointer to structure with diagnostic time constants.
 * @param shuntConn     True (shunt resistor is connected) or false
 *                      (shunt is not connected).
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_ShuntConn(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, const bcc_diag_const_t* const diagTimeConst,
    bool* const shuntConn);

/*!
 * @brief This function implements GPIOx OT/UT functional verification (SM05).
 * All GPIOs are forced to analog input RM mode. Driving pin to low/high
 * simulates OT/UT condition. Note that programmed OT/UT thresholds are used to
 * verify functionality.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param results   Pointer to structure with results of diagnostics. See
 *                  definition of this structure in this header file above.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_GPIOxOtUt(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_gpiox_otut_res_t* const results);

/*!
 * @brief This function implements GPIOx open terminal diagnostics (SM06).
 * To detect open terminals on the GPIO pins, a weak internal pull-down is
 * commanded ON and OFF. Voltages below the VOL(TH) threshold are considered
 * open terminals.
 *
 * @param drvConfig  Pointer to driver instance configuration.
 * @param cid        Cluster Identification Address of the BCC device.
 * @param openStatus Open terminal status for each GPIOx terminal (ANx_OPEN bits
 *                   in OPEN GPIO_SHORT_ANx_OPEN_STS register). If the value is
 *                   zero, no GPIOx terminal is opened.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_GPIOxOpen(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, uint16_t* const openStatus);

/*!
 * @brief This function implements Cell balance open load detection (SM40).
 * To detect open load on the cell balance terminals, Rpd_cb resistor is applied
 * between the CBx outputs and their common terminal. Voltages below the
 * Vout(FLT_TH) activate the CB_OPEN_FLT register bits. Note that results for
 * short detection are not part of this diagnostics. It is diagnosed
 * continuously with the cell balance FET active.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param results   Pointer to structure with results of diagnostics. See
 *                  definition of this structure in this header file above.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Diag_CBxOpen(bcc_drv_config_t* const drvConfig,
    const bcc_cid_t cid, bcc_diag_cbx_open_res_t* const results);

/*! @} */

#endif /* __BCC_DIAGNOSTICS_H__ */
/*******************************************************************************
 * EOF;
 ******************************************************************************/
