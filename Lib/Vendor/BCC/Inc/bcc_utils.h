/*!
 * @file bcc_utils.h
 *
 * Macros for conversion of measurement results, voltage thresholds and other
 * configurations for MC33771C and MC33772C.
 */

#ifndef __BCC_UTILS_H__
#define __BCC_UTILS_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "MC33771C.h"
#include "MC33772C.h"

/*******************************************************************************
 * Macros for conversion of the measurement results
 ******************************************************************************/

/*!
 * @brief Returns a 32 bit signed value of Coulomb counter composed from values
 * of registers COULOMB_CNT1 (higher part) and COULOMB_CNT2 (lower part).
 *
 * @param coulombCnt1 Content of register COULOMB_CNT1.
 * @param coulombCnt2 Content of register COULOMB_CNT2.
 */
#define BCC_GET_COULOMB_CNT(coulombCnt1, coulombCnt2)                               \
	((int32_t)(((uint32_t)((coulombCnt1) &                                      \
			       MC33771C_TH_COULOMB_CNT_MSB_TH_COULOMB_CNT_MSB_MASK) \
		    << 16U) |                                                       \
		   ((uint32_t)(coulombCnt2) &                                       \
		    MC33771C_TH_COULOMB_CNT_LSB_TH_COULOMB_CNT_LSB_MASK)))

/*!
 * @brief Returns a raw value of ISENSE measurement composed from values
 * of registers MEAS_ISENSE1 and MEAS_ISENSE2.
 *
 * @param measISense1 Content of register MEAS_ISENSE1.
 * @param measISense2 Content of register MEAS_ISENSE2.
 */
#define BCC_GET_ISENSE_RAW(measISense1, measISense2)                           \
	((((uint32_t)(measISense1) & MC33771C_MEAS_ISENSE1_MEAS_I_MSB_MASK)    \
	  << 4U) |                                                             \
	 ((uint32_t)(measISense2) & MC33771C_MEAS_ISENSE2_MEAS_I_LSB_MASK))

/*!
 * @brief Performed a sign extension on the raw value of ISENSE measurement
 * (result of macro BCC_GET_ISENSE_RAW). Returns a signed 32 bit value.
 *
 * @param iSenseRaw Raw value of measured current (result of
 *                  BCC_GET_ISENSE_RAW macro).
 */
#define BCC_GET_ISENSE_RAW_SIGN(iSenseRaw)                                     \
	((int32_t)(((iSenseRaw) & 0x040000U) ? ((iSenseRaw) | 0xFFF80000U)     \
					     : (iSenseRaw)))

/*!
 * @brief This macro calculates ISENSE value (in [uV]) from the content of
 * MEAS_ISENSE1 and MEAS_ISENSE2 registers.
 *
 * Resolution of the measurement is 0.6 uV/LSB (V2RES).
 *
 * Note: V_IND (ISENSE+/ISENSE- Differential Input Voltage Range) is between
 * -150 mV and 150 mV (see datasheet).
 *
 * @param iSense1 Content of register MEAS_ISENSE1.
 * @param iSense2 Content of register MEAS_ISENSE2.
 *
 * @return ISENSE voltage in [uV]; int32_t type.
 */
#define BCC_GET_ISENSE_VOLT(iSense1, iSense2)                                  \
	((BCC_GET_ISENSE_RAW_SIGN(BCC_GET_ISENSE_RAW(iSense1, iSense2)) * 6) / \
	 10)

/*!
 * @brief This macro calculates the measured current (in [mA]) from the SHUNT
 * resistance and from the content of MEAS_ISENSE1 and MEAS_ISENSE2 registers.
 *
 * Resolution of the measurement is (600/R_SHUNT) mA/LSB when R_SHUNT in [uOhm].
 *
 * @param rShunt  Resistance of Shunt resistor (in [uOhm]).
 * @param iSense1 Content of register MEAS_ISENSE1.
 * @param iSense2 Content of register MEAS_ISENSE2.
 *
 * @return ISENSE current in [mA]; int32_t type.
 */
#define BCC_GET_ISENSE_AMP(rShunt, iSense1, iSense2)                           \
	((BCC_GET_ISENSE_RAW_SIGN(BCC_GET_ISENSE_RAW(iSense1, iSense2)) *      \
	  600) /                                                               \
	 (int32_t)(rShunt))

/*!
 * @brief Masks a register value and returns a raw measured value.
 *
 * This macro can be used only for MEAS_STACK, MEAS_CELLx, MEAS_ANx,
 * MEAS_VBG_DIAG_ADC1x and MEAS_IC_TEMP registers!
 *
 * @param reg Value from a measurement register (MEAS_STACK, MEAS_CELLx,
 *            MEAS_ANx, MEAS_IC_TEMP or MEAS_VBG_DIAG_ADC1x).
 */
#define BCC_GET_MEAS_RAW(reg) ((reg) & MC33771C_MEAS_STACK_MEAS_STACK_MASK)

/*!
 * @brief Converts a value of the MEAS_STACK register to [uV].
 *
 * Resolution of the register is 2.4414 mV/LSB.
 * Result is in the range of 0 and 80 000 000 uV.
 *
 * @param reg Value of the MEAS_STACK register.
 *
 * @return Converted value in [uV]; uint32_t type.
 */
#define BCC_GET_STACK_VOLT(reg) ((uint32_t)BCC_GET_MEAS_RAW(reg) * 24414U / 10U)

/*!
 * @brief Converts a value of MEAS_CELLx, MEAS_ANx (absolute measurement),
 * MEAS_VBG_DIAG_ADC1x registers to [uV].
 *
 * Resolution of these registers is 152.58789 uV/LSB.
 * Result is in the range of 0 and 4 999 847 uV.
 *
 * Note: 78125/512 = 152.5878906 provides the best accuracy in 32b arithmetic
 * w/o any overflow. A logical shift speeds up the operation.
 *
 * @param reg Value of a measurement register.
 *
 * @return Converted value in [uV]; uint32_t type.
 */
#define BCC_GET_VOLT(reg) (((uint32_t)BCC_GET_MEAS_RAW(reg) * 78125U) >> 9)

/*!
 * @brief Converts a value of MEAS_ANx (ratiometric measurement) MC33771C
 * registers to [uV].
 *
 * Resolution of these registers is: VCOM*30.51851 uV/LSB.
 * Result is in the range of 0 and (1000000 * VCOM) uV.
 *
 * Note: Instead of operation ((reg * 30.51851) * VCOM) / 1000, which could
 * overflow, ((reg * 15.62547712) * VCOM) / 512 is used.
 *
 * @param reg  Value of a measurement register.
 * @param vCom VCOM voltage in [mV], max 5800 mV.
 *
 * @return Converted value in [uV]; uint32_t type.
 */
#define MC33771C_GET_AN_RATIO_VOLT(reg, vCom)                                  \
	(((((uint32_t)BCC_GET_MEAS_RAW(reg) * 3922U) / 251U) * (vCom)) >> 9)

/*!
 * @brief Converts a value of MEAS_ANx (ratiometric measurement) MC33771C
 * registers to [uV].
 *
 * Resolution of these registers is: VCOM*30.5176 uV/LSB.
 * Result is in the range of 0 and (999970 * VCOM) uV.
 *
 * Note: Instead of operation ((reg * 30.5176) * VCOM) / 1000, which could
 * overflow, ((reg * 15.6250112) * VCOM) / 512 is used.
 *
 * @param reg  Value of a measurement register.
 * @param vCom VCOM voltage in [mV], max 5800 mV.
 *
 * @return Converted value in [uV]; uint32_t type.
 */
#define MC33772C_GET_AN_RATIO_VOLT(reg, vCom)                                  \
	(((((uint32_t)BCC_GET_MEAS_RAW(reg) * 39422U) / 2523U) * (vCom)) >> 9)

/*!
 * @brief Converts a value of the MEAS_IC_TEMP register to [K].
 *
 * Resolution of the MEAS_IC_TEMP register is 0.032 Kelvin/LSB.
 *
 * @param reg Value of the MEAS_IC_TEMP register.
 *
 * @return Converted value in [K] multiplied by 10
 *         (i.e. resolution of 0.1 K); int32_t type.
 */
#define BCC_GET_IC_TEMP_K(reg)                                                 \
	((int16_t)((((int32_t)(BCC_GET_MEAS_RAW(reg))) * 32) / 100))

/*!
 * @brief Converts a value of the MEAS_IC_TEMP register to [�C].
 *
 * Resolution of the MEAS_IC_TEMP register is 0.032 Kelvin/LSB.
 *
 * @param reg Value of the MEAS_IC_TEMP register.
 *
 * @return Converted value in [�C] multiplied by 10
 *         (i.e. resolution of 0.1 �C); int32_t type.
 */
#define BCC_GET_IC_TEMP_C(reg)                                                 \
	((int16_t)((((int32_t)(BCC_GET_MEAS_RAW(reg))) * 32 - 273150) / 100))

/*!
 * @brief Converts a value of the MEAS_IC_TEMP register to [�F].
 *
 * Resolution of the MEAS_IC_TEMP register is 0.032 Kelvin/LSB.
 *
 * @param reg Value of the MEAS_IC_TEMP register.
 *
 * @return Converted value in [�F] multiplied by 10
 *         (i.e. resolution of 0.1 �F); int32_t type.
 */
#define BCC_GET_IC_TEMP_F(reg)                                                 \
	((int16_t)((((int32_t)(BCC_GET_MEAS_RAW(reg))) * 288 - 2298350) / 500))

/*******************************************************************************
 * Macros for conversion of the voltage thresholds
 ******************************************************************************/

/*!
 * @brief Extracts a value from Over Coulomb counting threshold to be placed in
 * the TH_COULOMB_CNT_MSB register.
 *
 * @param threshold Threshold value (signed two's complement, with V_2RES
 *                  resolution).
 */
#define BCC_GET_TH_COULOMB_CNT_MSB(threshold) ((uint16_t)((threshold) >> 16U))

/*!
 * @brief Extracts a value from Over Coulomb counting threshold to be placed in
 * the TH_COULOMB_CNT_LSB register.
 *
 * @param threshold Threshold value (signed two's complement, with V_2RES
 *                  resolution).
 */
#define BCC_GET_TH_COULOMB_CNT_LSB(threshold)                                  \
	((uint16_t)((threshold) && 0xFFFF))

/*!
 * @brief Converts an overcurrent threshold voltage (in sleep mode) to a raw
 * value to be placed in the TH_ISENSE_OC register.
 *
 * @param threshold Threshold value in [uV].
 */
#define BCC_GET_TH_ISENSE_OC(threshold)                                        \
	(((((threshold) * 5U) / 6U) > 0xFFFU) ? 0xFFFU                         \
					      : (((threshold) * 5U) / 6U))

/*!
 * @brief Converts a cell terminal OV/UV threshold voltage to a raw value to be
 * placed in appropriate bit fields of TH_ALL_CT and TH_CTx registers.
 *
 * Note that value of OV_UV_EN[COMMON_UV_TH] and OV_UV_EN[COMMON_OV_TH] bits
 * determines whether thresholds stored in TH_ALL_CT register are applied or
 * individual thresholds stored in TH_CTx registers are applied.
 *
 * @param threshold Threshold value in [mV].
 */
#define BCC_GET_TH_CTX(threshold)                                              \
	(uint16_t)(((((threshold) * 10U) / 195U) > 0xFF)                       \
		       ? 0xFF                                                  \
		       : (((threshold) * 10U) / 195U))

/*!
 * @brief Converts an analog input OV/UV (UT/OT) threshold voltage to a raw
 * value to be placed in TH_ANx_OT and TH_ANx_UT registers.
 *
 * @param threshold Threshold value in [mV].
 */
#define BCC_GET_TH_ANX(threshold)                                              \
	(uint16_t)((((((uint32_t)(threshold)) * 100U) / 488U) > 0x3FFU)        \
		       ? 0x3FFU                                                \
		       : ((((uint32_t)(threshold)) * 100U) / 488U))

/*******************************************************************************
 * Macros for other configuration
 ******************************************************************************/

/*!
 * @brief Converts ADC2 offset value from [uV] to a raw value with 0.6 uV
 * resolution to be placed to ADC2_OFFSET_COMP bit-field of ADC2_OFFSET_COMP
 * register.
 *
 * @param offset Offset value in [uV] as 2's complement in int16_t.
 */
#define BCC_GET_ADC2_OFFSET(offset)                                            \
	((uint16_t)((((((int16_t)(offset)) * 10) / 6) > 127)                   \
			? 127                                                  \
			: ((((((int16_t)(offset)) * 10) / 6) < -128)           \
			       ? -128                                          \
			       : ((((int16_t)(offset)) * 10) / 6))))

#endif /* __BCC_UTILS_H__ */
/*******************************************************************************
 * EOF;
 ******************************************************************************/
