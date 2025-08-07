#ifndef __BCC_H__
#define __BCC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdbool.h>
#include <stddef.h>

#include "bcc_utils.h"

/*******************************************************************************
 * User definitions
 ******************************************************************************/

/* The following macros determine how the SPI data buffers (arrays) passed to
 * BCC_MCU_TransferTpl and BCC_MCU_TransferSpi functions are organized.
 * Specifically, the macros influence the byte order in the buffers and
 * alignment of SPI frames in the buffers.
 *
 * Note that the natural assignment (for big-endian) is as follows:
 * #define BCC_MSG_SIZE              6U (48b frames)
 *
 * #define BCC_MSG_IDX_DATA_H        0U (first sent/received byte)
 * #define BCC_MSG_IDX_DATA_L        1U
 * #define BCC_MSG_IDX_ADDR          2U
 * #define BCC_MSG_IDX_CID           3U
 * #define BCC_MSG_IDX_CNT_CMD       4U
 * #define BCC_MSG_IDX_CRC           5U (last sent/received byte)
 *
 * Different values can be useful in order to fit low-level drivers of various
 * MCUs (e.g. big-endian vs. little-endian). The default configuration in this
 * BCC driver (as it follows) is determined for S32K1xx SDK 3.0.0 RTM. Note that
 * LPSPI driver in S32K1xx SDK requires alignment of the buffer to multiples of
 * four. Therefore, BCC_MSG_SIZE is eight instead of six. */

/*! @brief Number of consecutive bytes reserved for each SPI frame in buffers
 * passed to BCC_MCU_Transfer* functions (i.e. SPI frame alignment). */
#define BCC_MSG_SIZE 6U

/*! @brief Index to Register data byte (higher byte) of SPI frame in the
 * buffers passed to BCC_MCU_Transfer* functions.
 * Note: This byte should be the first sent to (received from) the SPI bus. */
#define BCC_MSG_IDX_DATA_H 0U

/*! @brief Index to Register data byte (lower byte) of SPI frame in the buffers
 * passed to BCC_MCU_Transfer* functions.
 * Note: This byte should be the second sent to (received from) the SPI bus. */
#define BCC_MSG_IDX_DATA_L 1U

/*! @brief Index to Register address byte of SPI frame in the buffers passed to
 * BCC_MCU_Transfer* functions.
 * Note: This byte should be the third sent to (received from) the SPI bus. */
#define BCC_MSG_IDX_ADDR 2U

/*! @brief Index to Device address (CID) byte of SPI frame in the buffers
 * passed to BCC_MCU_Transfer* functions.
 * Note: This byte should be the fourth sent to (received from) the SPI bus. */
#define BCC_MSG_IDX_CID 3U

/*! @brief Index to Message counter and Command byte of SPI frame in the
 * buffers passed to BCC_MCU_Transfer* functions.
 * Note: This byte should be the fifth sent to (received from) the SPI bus. */
#define BCC_MSG_IDX_CNT_CMD 4U

/*! @brief Index to CRC byte of SPI frame in the buffers passed to
 * BCC_MCU_Transfer* functions.
 * Note: This byte should be the last sent to (received from) the SPI bus. */
#define BCC_MSG_IDX_CRC 5U

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Maximal number of BCC devices in SPI mode. */
#define BCC_DEVICE_CNT_MAX_SPI 1U
/*! @brief Maximal number of BCC devices in TPL mode. */
#define BCC_DEVICE_CNT_MAX_TPL 63U
/*! @brief Maximal number of BCC devices. */
#define BCC_DEVICE_CNT_MAX BCC_DEVICE_CNT_MAX_TPL

/*! @brief Minimal battery cell count connected to MC33771C. */
#define MC33771C_MIN_CELLS 7U
/*! @brief Maximal battery cell count connected to MC33771C. */
#define MC33771C_MAX_CELLS 14U
/*! @brief Minimal battery cell count connected to MC33772C. */
#define MC33772C_MIN_CELLS 3U
/*! @brief Maximal battery cell count connected to MC33772C. */
#define MC33772C_MAX_CELLS 6U
/*! @brief Maximal battery cell count connected to any BCC device. */
#define BCC_MAX_CELLS 14U

/*! @brief Maximal battery cell count connected to the BCC device.
 *
 * @param dev BCC device type.
 */
#define BCC_MAX_CELLS_DEV(dev)                                                 \
	((dev == BCC_DEVICE_MC33771C) ? MC33771C_MAX_CELLS : MC33772C_MAX_CELLS)

/*! @brief  Number of MC33771C measurement registers.
 *
 * Note MC33772C contains 22 measurement registers. For compliance
 * with bcc_measurements_t indexes, BCC_Meas_GetRawValues function
 * requires 30 x uint16_t array for both BCC devices.
 */
#define BCC_MEAS_CNT 30U

/*! @brief  Number of BCC status (fault) registers. */
#define BCC_STAT_CNT 11U

/*! @brief Max. number of frames that can be read at once in TPL mode. */
#define BCC_RX_LIMIT_TPL 0x7FU

/*! @brief Size of buffer that is used for receiving via SPI in TPL mode. */
#define BCC_RX_BUF_SIZE_TPL (BCC_MSG_SIZE * (BCC_RX_LIMIT_TPL + 1U))

/*! @brief Number of GPIO/temperature sensor inputs. */
#define BCC_GPIO_INPUT_CNT 7U

/*!
 * Returns True if value VAL is in the range defined by MIN and MAX values
 * (range includes the border values).
 *
 * @param val Comparison value.
 * @param min Minimal value of the range.
 * @param max Maximal value of the range.
 *
 * @return True if value is the range. False otherwise.
 */
#define BCC_IS_IN_RANGE(val, min, max) (((val) >= (min)) && ((val) <= (max)))

/*!
 * @brief Returns a non-zero value when desired cell (cellNo) is connected
 * to the BCC specified by CID.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param cellNo    Cell number (range is {1, ..., 14} for MC33771C and
 *                  {1, ..., 6} for MC33772C).
 *
 * @return Non-zero value if cell is connected, zero otherwise.
 */
#define BCC_IS_CELL_CONN(drvConfig, cid, cellNo)                               \
	((drvConfig)->drvData.cellMap[(cid) - 1U] & (1U << ((cellNo) - 1U)))

/* Enum types definition. */
/*!
 * @addtogroup enum_group
 * @{
 */
/*! @brief Error codes. */
typedef enum {
	BCC_STATUS_SUCCESS = 0U,     /*!< No error. */
	BCC_STATUS_PARAM_RANGE = 1U, /*!< Parameter out of range. */
	BCC_STATUS_SPI_FAIL = 2U,    /*!< Fail in the SPI communication. */
	BCC_STATUS_COM_TIMEOUT = 3U, /*!< Communication timeout. */
	BCC_STATUS_COM_ECHO = 4U, /*!< Received "echo" frame from MC33664 does
				     not correspond to the sent frame. */
	BCC_STATUS_COM_CRC = 5U,  /*!< Wrong CRC in the received SPI frame. */
	BCC_STATUS_COM_MSG_CNT =
	    6U, /*!< Received frame has a valid CRC but the message counter
		     value does not match to the expected one. */
	BCC_STATUS_COM_NULL =
	    7U, /*!< Received frame has a valid CRC but all bit-fields
		     except CRC and message counter are zero. This occurs only
		     in SPI communication mode: during the very first message
		     or as a response to an invalid request from MCU. */
	BCC_STATUS_DIAG_FAIL =
	    8U, /*!< It is not allowed to enter diagnostic mode. */
	BCC_STATUS_EEPROM_ERROR =
	    9U, /*!< An error occurred during the communication to EEPROM. */
	BCC_STATUS_EEPROM_PRESENT = 10U, /*!< No EEPROM detected. */
	BCC_STATUS_DATA_RDY =
	    11U, /*!< A new sequence of conversions is currently running. */
	BCC_STATUS_TIMEOUT_START =
	    12U /*!< An error reported in BCC_MCU_StartTimeout function. */
} bcc_status_t;

/*! @brief Cluster Identification Address.
 *
 * Note that SPI communication mode allows one cluster/device only.
 * The maximum number of clusters/devices for TPL mode is 63. */
typedef enum {
	BCC_CID_UNASSIG = 0U, /*!< ID of uninitialized BCC devices. */
	BCC_CID_DEV1 = 1U,    /*!< Cluster ID of device 1.
				   In SPI mode, it is the only connected device.
				   In TPL mode, it is the first device in daisy
				   chain (connected directly to MC33664). */
	BCC_CID_DEV2 = 2U,    /*!< Cluster ID of device 2. */
	BCC_CID_DEV3 = 3U,    /*!< Cluster ID of device 3. */
	BCC_CID_DEV4 = 4U,    /*!< Cluster ID of device 4. */
	BCC_CID_DEV5 = 5U,    /*!< Cluster ID of device 5. */
	BCC_CID_DEV6 = 6U,    /*!< Cluster ID of device 6. */
	BCC_CID_DEV7 = 7U,    /*!< Cluster ID of device 7. */
	BCC_CID_DEV8 = 8U,    /*!< Cluster ID of device 8. */
	BCC_CID_DEV9 = 9U,    /*!< Cluster ID of device 9. */
	BCC_CID_DEV10 = 10U,  /*!< Cluster ID of device 10. */
	BCC_CID_DEV11 = 11U,  /*!< Cluster ID of device 11. */
	BCC_CID_DEV12 = 12U,  /*!< Cluster ID of device 12. */
	BCC_CID_DEV13 = 13U,  /*!< Cluster ID of device 13. */
	BCC_CID_DEV14 = 14U,  /*!< Cluster ID of device 14. */
	BCC_CID_DEV15 = 15U,  /*!< Cluster ID of device 15. */
	BCC_CID_DEV16 = 16U,  /*!< Cluster ID of device 16. */
	BCC_CID_DEV17 = 17U,  /*!< Cluster ID of device 17. */
	BCC_CID_DEV18 = 18U,  /*!< Cluster ID of device 18. */
	BCC_CID_DEV19 = 19U,  /*!< Cluster ID of device 19. */
	BCC_CID_DEV20 = 20U,  /*!< Cluster ID of device 20. */
	BCC_CID_DEV21 = 21U,  /*!< Cluster ID of device 21. */
	BCC_CID_DEV22 = 22U,  /*!< Cluster ID of device 22. */
	BCC_CID_DEV23 = 23U,  /*!< Cluster ID of device 23. */
	BCC_CID_DEV24 = 24U,  /*!< Cluster ID of device 24. */
	BCC_CID_DEV25 = 25U,  /*!< Cluster ID of device 25. */
	BCC_CID_DEV26 = 26U,  /*!< Cluster ID of device 26. */
	BCC_CID_DEV27 = 27U,  /*!< Cluster ID of device 27. */
	BCC_CID_DEV28 = 28U,  /*!< Cluster ID of device 28. */
	BCC_CID_DEV29 = 29U,  /*!< Cluster ID of device 29. */
	BCC_CID_DEV30 = 30U,  /*!< Cluster ID of device 30. */
	BCC_CID_DEV31 = 31U,  /*!< Cluster ID of device 31. */
	BCC_CID_DEV32 = 32U,  /*!< Cluster ID of device 32. */
	BCC_CID_DEV33 = 33U,  /*!< Cluster ID of device 33. */
	BCC_CID_DEV34 = 34U,  /*!< Cluster ID of device 34. */
	BCC_CID_DEV35 = 35U,  /*!< Cluster ID of device 35. */
	BCC_CID_DEV36 = 36U,  /*!< Cluster ID of device 36. */
	BCC_CID_DEV37 = 37U,  /*!< Cluster ID of device 37. */
	BCC_CID_DEV38 = 38U,  /*!< Cluster ID of device 38. */
	BCC_CID_DEV39 = 39U,  /*!< Cluster ID of device 39. */
	BCC_CID_DEV40 = 40U,  /*!< Cluster ID of device 40. */
	BCC_CID_DEV41 = 41U,  /*!< Cluster ID of device 41. */
	BCC_CID_DEV42 = 42U,  /*!< Cluster ID of device 42. */
	BCC_CID_DEV43 = 43U,  /*!< Cluster ID of device 43. */
	BCC_CID_DEV44 = 44U,  /*!< Cluster ID of device 44. */
	BCC_CID_DEV45 = 45U,  /*!< Cluster ID of device 45. */
	BCC_CID_DEV46 = 46U,  /*!< Cluster ID of device 46. */
	BCC_CID_DEV47 = 47U,  /*!< Cluster ID of device 47. */
	BCC_CID_DEV48 = 48U,  /*!< Cluster ID of device 48. */
	BCC_CID_DEV49 = 49U,  /*!< Cluster ID of device 49. */
	BCC_CID_DEV50 = 50U,  /*!< Cluster ID of device 50. */
	BCC_CID_DEV51 = 51U,  /*!< Cluster ID of device 51. */
	BCC_CID_DEV52 = 52U,  /*!< Cluster ID of device 52. */
	BCC_CID_DEV53 = 53U,  /*!< Cluster ID of device 53. */
	BCC_CID_DEV54 = 54U,  /*!< Cluster ID of device 54. */
	BCC_CID_DEV55 = 55U,  /*!< Cluster ID of device 55. */
	BCC_CID_DEV56 = 56U,  /*!< Cluster ID of device 56. */
	BCC_CID_DEV57 = 57U,  /*!< Cluster ID of device 57. */
	BCC_CID_DEV58 = 58U,  /*!< Cluster ID of device 58. */
	BCC_CID_DEV59 = 59U,  /*!< Cluster ID of device 59. */
	BCC_CID_DEV60 = 60U,  /*!< Cluster ID of device 60. */
	BCC_CID_DEV61 = 61U,  /*!< Cluster ID of device 61. */
	BCC_CID_DEV62 = 62U,  /*!< Cluster ID of device 62. */
	BCC_CID_DEV63 = 63U   /*!< Cluster ID of device 63. */
} bcc_cid_t;

/*! @brief BCC communication mode.  */
typedef enum {
	BCC_MODE_SPI = 0U, /*!< SPI communication mode. */
	BCC_MODE_TPL = 1U  /*!< TPL communication mode. */
} bcc_mode_t;

/*! @brief BCC device.  */
typedef enum {
	BCC_DEVICE_MC33771C = 0U, /*!< MC33771C. */
	BCC_DEVICE_MC33772C = 1U  /*!< MC33772C. */
} bcc_device_t;

/*! @brief Measurements provided by battery cell controller.
 *
 * Note that MC33772C doesn't have MEAS_CELL7 - MEAS_CELL14 registers.
 * Function BCC_Meas_GetRawValues returns 0x0000 at these positions.
 */
typedef enum {
	BCC_MSR_CC_NB_SAMPLES = 0U, /*!< Number of samples in Coulomb counter
				       (register CC_NB_SAMPLES). */
	BCC_MSR_COULOMB_CNT1 =
	    1U, /*!< Coulomb counting accumulator (register COULOMB_CNT1). */
	BCC_MSR_COULOMB_CNT2 =
	    2U, /*!< Coulomb counting accumulator (register COULOMB_CNT2). */
	BCC_MSR_ISENSE1 =
	    3U, /*!< ISENSE measurement (register MEAS_ISENSE1). */
	BCC_MSR_ISENSE2 =
	    4U, /*!< ISENSE measurement (register MEAS_ISENSE2). */
	BCC_MSR_STACK_VOLT =
	    5U, /*!< Stack voltage measurement (register MEAS_STACK). */
	BCC_MSR_CELL_VOLT14 =
	    6U, /*!< Cell 14 voltage measurement (register MEAS_CELL14). */
	BCC_MSR_CELL_VOLT13 =
	    7U, /*!< Cell 13 voltage measurement (register MEAS_CELL13). */
	BCC_MSR_CELL_VOLT12 =
	    8U, /*!< Cell 12 voltage measurement (register MEAS_CELL12). */
	BCC_MSR_CELL_VOLT11 =
	    9U, /*!< Cell 11 voltage measurement (register MEAS_CELL11). */
	BCC_MSR_CELL_VOLT10 =
	    10U, /*!< Cell 10 voltage measurement (register MEAS_CELL10). */
	BCC_MSR_CELL_VOLT9 =
	    11U, /*!< Cell 9 voltage measurement (register MEAS_CELL9). */
	BCC_MSR_CELL_VOLT8 =
	    12U, /*!< Cell 8 voltage measurement (register MEAS_CELL8). */
	BCC_MSR_CELL_VOLT7 =
	    13U, /*!< Cell 7 voltage measurement (register MEAS_CELL7). */
	BCC_MSR_CELL_VOLT6 =
	    14U, /*!< Cell 6 voltage measurement (register MEAS_CELL6). */
	BCC_MSR_CELL_VOLT5 =
	    15U, /*!< Cell 5 voltage measurement (register MEAS_CELL5). */
	BCC_MSR_CELL_VOLT4 =
	    16U, /*!< Cell 4 voltage measurement (register MEAS_CELL4). */
	BCC_MSR_CELL_VOLT3 =
	    17U, /*!< Cell 3 voltage measurement (register MEAS_CELL3). */
	BCC_MSR_CELL_VOLT2 =
	    18U, /*!< Cell 2 voltage measurement (register MEAS_CELL2). */
	BCC_MSR_CELL_VOLT1 =
	    19U, /*!< Cell 1 voltage measurement (register MEAS_CELL1). */
	BCC_MSR_AN6 =
	    20U, /*!< Analog input 6 voltage measurement (register MEAS_AN6). */
	BCC_MSR_AN5 =
	    21U, /*!< Analog input 5 voltage measurement (register MEAS_AN5). */
	BCC_MSR_AN4 =
	    22U, /*!< Analog input 4 voltage measurement (register MEAS_AN4). */
	BCC_MSR_AN3 =
	    23U, /*!< Analog input 3 voltage measurement (register MEAS_AN3). */
	BCC_MSR_AN2 =
	    24U, /*!< Analog input 2 voltage measurement (register MEAS_AN2). */
	BCC_MSR_AN1 =
	    25U, /*!< Analog input 1 voltage measurement (register MEAS_AN1). */
	BCC_MSR_AN0 =
	    26U, /*!< Analog input 0 voltage measurement (register MEAS_AN0). */
	BCC_MSR_ICTEMP =
	    27U, /*!< IC temperature measurement (register MEAS_IC_TEMP). */
	BCC_MSR_VBGADC1A = 28U, /*!< ADCIA Band Gap Reference measurement
				   (register MEAS_VBG_DIAG_ADC1A). */
	BCC_MSR_VBGADC1B = 29U	/*!< ADCIB Band Gap Reference measurement
				   (register MEAS_VBG_DIAG_ADC1B). */
} bcc_measurements_t;

/*! @brief Number of samples to be averaged in the conversion request. */
typedef enum {
	BCC_AVG_1 =
	    MC33771C_ADC_CFG_AVG_NO_AVERAGING_ENUM_VAL, /*!< No averaging, the
							   result is taken as
							   is. */
	BCC_AVG_2 =
	    MC33771C_ADC_CFG_AVG_2_SAMPLES_ENUM_VAL, /*!< Averaging of 2
							consecutive samples. */
	BCC_AVG_4 =
	    MC33771C_ADC_CFG_AVG_4_SAMPLES_ENUM_VAL, /*!< Averaging of 4
							consecutive samples. */
	BCC_AVG_8 =
	    MC33771C_ADC_CFG_AVG_8_SAMPLES_ENUM_VAL, /*!< Averaging of 8
							consecutive samples. */
	BCC_AVG_16 =
	    MC33771C_ADC_CFG_AVG_16_SAMPLES_ENUM_VAL, /*!< Averaging of 16
							 consecutive samples. */
	BCC_AVG_32 =
	    MC33771C_ADC_CFG_AVG_32_SAMPLES_ENUM_VAL, /*!< Averaging of 32
							 consecutive samples. */
	BCC_AVG_64 =
	    MC33771C_ADC_CFG_AVG_64_SAMPLES_ENUM_VAL, /*!< Averaging of 64
							 consecutive samples. */
	BCC_AVG_128 =
	    MC33771C_ADC_CFG_AVG_128_SAMPLES_ENUM_VAL, /*!< Averaging of 126
							  consecutive samples.
							*/
	BCC_AVG_256 =
	    MC33771C_ADC_CFG_AVG_256_SAMPLES_ENUM_VAL /*!< Averaging of 256
							 consecutive samples. */
} bcc_avg_t;

/*! @brief Status provided by battery cell controller. */
typedef enum {
	BCC_FS_CELL_OV =
	    0U, /*!< CT overvoltage fault (register CELL_OV_FLT). */
	BCC_FS_CELL_UV =
	    1U, /*!< CT undervoltage fault (register CELL_UV_FLT). */
	BCC_FS_CB_OPEN = 2U,	 /*!< Open CB fault (register CB_OPEN_FLT). */
	BCC_FS_CB_SHORT = 3U,	 /*!< Short CB fault (register CB_SHORT_FLT). */
	BCC_FS_GPIO_STATUS = 4U, /*!< GPIO status (register GPIO_STS). */
	BCC_FS_AN_OT_UT =
	    5U, /*!< AN over and undertemperature (register AN_OT_UT_FLT). */
	BCC_FS_GPIO_SHORT = 6U, /*!< Short GPIO/open AN diagnostic (register
				   GPIO_SHORT_ANx_OPEN_STS). */
	BCC_FS_COMM = 7U, /*!< Number of communication errors detected (register
			     COM_STATUS). */
	BCC_FS_FAULT1 = 8U, /*!< Fault status (register FAULT1_STATUS). */
	BCC_FS_FAULT2 = 9U, /*!< Fault status (register FAULT2_STATUS). */
	BCC_FS_FAULT3 = 10U /*!< Fault status (register FAULT3_STATUS). */
} bcc_fault_status_t;

/*! @brief Mode of a GPIOx/ANx pin. */
typedef enum {
	BCC_PIN_ANALOG_IN_RATIO =
	    0U, /*!< Analog input for ratiometric measurement). */
	BCC_PIN_ANALOG_IN_ABS =
	    1U,			  /*!< Analog input for absolute measurement. */
	BCC_PIN_DIGITAL_IN = 2U,  /*!< Digital input. */
	BCC_PIN_DIGITAL_OUT = 3U, /*!< Digital output. */
	BCC_PIN_WAKE_UP_IN = 4U,  /*!< Digital input with a wake-up capability
				     (only GPIO0).  Wakes-up BCC from sleep to
				     normal mode on any edge of GPIO0. */
	BCC_PIN_CONVERT_TR_IN =
	    5U /*!< Digital input with a convert trigger capability (only
		  GPIO2). A rising edge on GPIO2 triggers an ADC1-A and ADC1-B
		    conversion when BCC is in normal mode. */
} bcc_pin_mode_t;

/*! @brief Unit of the result from BCC_Meas_GetIcTemperature function. */
typedef enum {
	BCC_TEMP_KELVIN = 0U,	 /*!< Result resolution: 0.1 K. */
	BCC_TEMP_CELSIUS = 1U,	 /*!< Result resolution: 0.1 �C. */
	BCC_TEMP_FAHRENHEIT = 2U /*!< Result resolution: 0.1 �F. */
} bcc_temp_unit_t;
/*! @} */

/* Configure struct types definition. */
/*!
 * @addtogroup struct_group
 * @{
 */
/*! @brief Coulomb counter data. */
typedef struct {
	int32_t ccAccumulator; /*!< Coulomb counting accumulator with V_2RES
				  resolution. */
	uint16_t nbSamples;    /*!< Number of samples accumulated in the Coulomb
				  counter. */
} bcc_cc_data_t;

/*!
 * @brief Driver internal data.
 *
 * Note that it is initialized in BCC_Init function by the driver
 * and the user mustn't change it at any time.
 */
typedef struct {
	uint16_t cellMap[BCC_DEVICE_CNT_MAX]; /*!< Bit map of used cells of each
						 BCC device. */
	uint8_t msgCntr[BCC_DEVICE_CNT_MAX +
			1]; /*!< Last received value of Message counter (values
			       0-15). MsgCntr[0] contains Message counter of
			       CID=0. */
	uint8_t rxBuf[BCC_RX_BUF_SIZE_TPL]; /*!< Buffer for receiving data in
					       TPL mode. */
} bcc_drv_data_t;

/*!
 * @brief Driver configuration.
 *
 * This structure contains all information needed for proper functionality of
 * the driver, such as used communication mode, BCC device(s) configuration or
 * internal driver data.
 */
typedef struct {
	uint8_t drvInstance; /*!< BCC driver instance. Passed to the external
				functions defined by the user. */

	bcc_mode_t commMode; /*!< BCC communication mode. */
	uint8_t devicesCnt;  /*!< Number of BCC devices. SPI mode allows one
				device only,  TPL mode allows up to 63 devices. */
	bcc_device_t device[BCC_DEVICE_CNT_MAX]; /*!< BCC device type of
						      [0] BCC with CID=1, [1]
						    BCC with CID=2, etc. */
	uint16_t cellCnt[BCC_DEVICE_CNT_MAX]; /*!< Number of connected cells to
						 each BCC. [0] BCC with CID=1,
						 [1] BCC with CID=2, etc. */
	bool loopBack; /*!< Loop back mode. If False, TPL_TX_Term. (RDTX_OUT)
			  bit is set for the last BCC device in the TPL chain.
			    This configuration item is ignored in SPI mode. */

	bcc_drv_data_t drvData; /*!< Internal driver data. */
} bcc_drv_config_t;
/*! @} */

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @addtogroup function_group
 * @{
 */

/*!
 * @brief This function initializes the battery cell controller device(s),
 * assigns CID and initializes internal driver data.
 *
 * Note that this function initializes only the INIT register of connected
 * BCC device(s).
 *
 * Note that the function is implemented for an universal use. It is capable to
 * move BCC devices into the NORMAL mode from all modes (INIT, IDLE, SLEEP and
 * NORMAL) except the RESET mode. Such implementation can generate more wake-up
 * sequences via CSB_TX pin than required for the current mode. These extra
 * sequences project to the COM_ERR_COUNT bit field in COM_STATUS register.
 * Therefore, it is recommended to clear the COM_STATUS register after calling
 * of this function.
 *
 * @param drvConfig Pointer to driver instance configuration.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Init(bcc_drv_config_t *const drvConfig);

/*!
 * @brief This function sends No Operation command the to BCC device. It can be
 * used to reset the communication timeout of the device without performing
 * any operation.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_SendNop(bcc_drv_config_t *const drvConfig,
			 const bcc_cid_t cid);

/*!
 * @brief This function sets sleep mode to all battery cell controller devices.
 *
 * In case of TPL communication mode, MC33664 has to be put into the sleep mode
 * separately, by the BCC_TPL_Disable function.
 *
 * @param drvConfig Pointer to driver instance configuration.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Sleep(bcc_drv_config_t *const drvConfig);

/*!
 * @brief This function sets normal mode to all battery cell controller devices.
 *
 * It is recommended to perform a check whether the device(s) is already woken
 * up by reading the FAULT or the INTB pin before calling this function. If the
 * device is already woken up, this function will increase the COM_ERR_COUNT bit
 * field in the COM_STATUS register and, moreover, will case so-called NULL
 * response (all bit filed set to zero except message counter and correct CRC)
 * in the following SPI response in case of the SPI communication mode (BCC
 * device received an invalid request from MCU).
 *
 * @param drvConfig Pointer to driver instance configuration.
 *
 * @return bcc_status_t Error code.
 */
void BCC_WakeUp(const bcc_drv_config_t *const drvConfig);

/*!
 * @brief This function resets BCC device using software reset. It enters reset
 * via SPI or TPL interface. In TPL, you can use BCC_CID_UNASSIG as CID
 * parameter for software reset of all devices in the chain (global write).
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_SoftwareReset(bcc_drv_config_t *const drvConfig,
			       const bcc_cid_t cid);

/*!
 * @brief This function resets BCC device using RESET pin.
 *
 * @param drvConfig Pointer to driver instance configuration.
 */
void BCC_HardwareReset(const bcc_drv_config_t *const drvConfig);

/*!
 * @brief This function enables MC33664 device (sets a normal mode).
 * Intended for TPL mode only!
 *
 * During the driver initialization (BCC_Init function), normal mode of MC33664
 * is set automatically. This function can be used when sleep mode of BCC
 * device(s) and MC33664 together is required. The typical function flow is as
 * follows: BCC_Sleep -> BCC_TPL_Disable -> ... -> BCC_TPL_Enable -> BCC_WakeUp.
 *
 * @param drvInstance BCC driver instance (content of the bcc_drv_config_t
 *                    structure). Passed to the external functions defined by
 *                    the user in order to distinguish between more TPL
 *                    interfaces.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_TPL_Enable(const uint8_t drvInstance);

/*!
 * @brief This function puts MC33664 device into the sleep mode.
 * Intended for TPL mode only!
 *
 * This function can be (optionally) used after BCC_Sleep function. Function
 * BCC_TPL_Enable must be then called before BCC_WakeUp function!
 *
 * @param drvInstance BCC driver instance (content of the bcc_drv_config_t
 *                    structure). Passed to the external functions defined by
 *                    the user.
 */
void BCC_TPL_Disable(const uint8_t drvInstance);

/*!
 * @brief This function reads a value from addressed register (or desired
 * number of registers) of selected battery cell controller device.
 *
 * In case of simultaneous read of more registers, address is incremented
 * in ascending manner.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param regAddr   Register address. See MC33771C.h and MC33772C.h header files
 *                  for possible values (MC3377*C_*_OFFSET macros).
 * @param regCnt    Number of registers to be read.
 * @param regVal    Pointer to memory where content of selected 16 bit registers
 *                  is stored.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Reg_Read(bcc_drv_config_t *const drvConfig,
			  const bcc_cid_t cid, const uint8_t regAddr,
			  const uint8_t regCnt, uint16_t *regVal);

/*!
 * @brief This function writes a value to addressed register of selected battery
 * cell controller device.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param regAddr   Register address. See MC33771C.h and MC33772C.h header files
 *                  for possible values (MC3377*C_*_OFFSET macros).
 * @param regVal    New value of selected register.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Reg_Write(bcc_drv_config_t *const drvConfig,
			   const bcc_cid_t cid, const uint8_t regAddr,
			   const uint16_t regVal);

/*!
 * @brief This function writes a value to addressed register of all configured
 * BCC devices. Intended for TPL mode only!
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param regAddr   Register address. See MC33771C.h and MC33772C.h header files
 *                  for possible values (MC3377*C_*_OFFSET macros).
 * @param regVal    New value of selected register.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Reg_WriteGlobal(bcc_drv_config_t *const drvConfig,
				 const uint8_t regAddr, const uint16_t regVal);

/*!
 * @brief This function updates content of a selected register; affects bits
 * specified by a bit mask only.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param regAddr   Register address. See MC33771C.h and MC33772C.h header files
 *                  for possible values (MC3377*C_*_OFFSET macros).
 * @param regMask   Bit mask. Bits set to 1 will be updated.
 * @param regVal    New value of register bits defined by bit mask.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Reg_Update(bcc_drv_config_t *const drvConfig,
			    const bcc_cid_t cid, const uint8_t regAddr,
			    const uint16_t regMask, const uint16_t regVal);

/*!
 * @brief This function starts ADC conversion in selected BCC device. It sets
 * number of samples to be averaged and Start of Conversion bit in ADC_CFG
 * register.
 *
 * You can use function BCC_Meas_IsConverting to check the conversion status or
 * a blocking function BCC_Meas_StartAndWait instead.
 * Measured values can be retrieved e.g. by function BCC_Meas_GetRawValues.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param avg       Number of samples to be averaged.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_StartConversion(bcc_drv_config_t *const drvConfig,
				      const bcc_cid_t cid, const bcc_avg_t avg);

/*!
 * @brief This function starts ADC conversion for all devices in TPL chain. It
 * uses a Global Write command to set ADC_CFG register. Intended for TPL mode
 * only!
 *
 * You can use function BCC_Meas_IsConverting to check conversion status and
 * function BCC_Meas_GetRawValues to retrieve the measured values.
 *
 * @param drvConfig   Pointer to driver instance configuration.
 * @param adcCfgValue Value of ADC_CFG register to be written to all devices in
 *                    the chain. Note that SOC bit is automatically added by
 *                    this function.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_StartConversionGlobal(bcc_drv_config_t *const drvConfig,
					    uint16_t adcCfgValue);

/*!
 * @brief This function checks status of conversion defined by End of Conversion
 * bit in ADC_CFG register.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param completed Pointer to check result. True if a conversion is complete.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_IsConverting(bcc_drv_config_t *const drvConfig,
				   const bcc_cid_t cid, bool *const completed);

/*!
 * @brief This function starts an on-demand conversion in selected BCC device
 * and waits for completion.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param avg       Number of samples to be averaged.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_StartAndWait(bcc_drv_config_t *const drvConfig,
				   const bcc_cid_t cid, const bcc_avg_t avg);

/*!
 * @brief This function reads the measurement registers and returns raw values.
 *
 * Macros defined in bcc_utils.h can be used to perform correct unit conversion.
 *
 * Warning: The "data ready" flag is ignored.
 *
 * @param drvConfig    Pointer to driver instance configuration.
 * @param cid          Cluster Identification Address of the BCC device.
 * @param measurements Array containing all values measured by BCC. Size of this
 *                     array must be BCC_MEAS_CNT*16b. Indexes into the array
 *                     are defined in enumeration bcc_measurements_t
 *                     placed in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetRawValues(bcc_drv_config_t *const drvConfig,
				   const bcc_cid_t cid,
				   uint16_t *const measurements);

/*!
 * @brief This function reads the Coulomb counter registers.
 *
 * Note that the Coulomb counter is independent on the on-demand conversions.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param cc        Coulomb counter data.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetCoulombCounter(bcc_drv_config_t *const drvConfig,
					const bcc_cid_t cid,
					bcc_cc_data_t *const cc);

/*!
 * @brief This function reads the ISENSE measurement and converts it to [uV].
 *
 * @param drvConfig  Pointer to driver instance configuration.
 * @param cid        Cluster Identification Address of the BCC device.
 * @param isenseVolt Pointer to memory where the ISENSE voltage (in [uV]) will
 *                   be stored.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetIsenseVoltage(bcc_drv_config_t *const drvConfig,
				       const bcc_cid_t cid,
				       int32_t *const isenseVolt);

/*!
 * @brief This function reads the stack measurement and converts it to [uV].
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param stackVolt Pointer to memory where the stack voltage (in [uV]) will
 *                  be stored.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetStackVoltage(bcc_drv_config_t *const drvConfig,
				      const bcc_cid_t cid,
				      uint32_t *const stackVolt);

/*!
 * @brief This function reads the cell measurements and converts them to [uV].
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param cellVolt  Pointer to the array where the cell voltages (in [uV]) will
 *                  be stored. Array must have an suitable size (14*32b for
 *                  MC33771C and 6*32b for MC33772C). Cell 1 voltage is stored
 *                  at [0], Cell 2 voltage at [1], etc.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetCellVoltages(bcc_drv_config_t *const drvConfig,
				      const bcc_cid_t cid,
				      uint32_t *const cellVolt);

/*!
 * @brief This function reads the voltage measurement of a selected cell and
 * converts it to [uV].
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param cellIndex Cell index. Use 0U for CELL 1, 1U for CELL2, etc.
 * @param cellVolt  Pointer to memory where the cell voltage (in [uV]) will
 *                  be stored.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetCellVoltage(bcc_drv_config_t *const drvConfig,
				     const bcc_cid_t cid, uint8_t cellIndex,
				     uint32_t *const cellVolt);

/*!
 * @brief This function reads the voltage measurement for all ANx and converts
 * them to [uV]. Intended for ANx configured for absolute measurements only!
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param anVolt    Pointer to the array where ANx voltages (in [uV]) will
 *                  be stored. Array must have an suitable size
 *                  (BCC_GPIO_INPUT_CNT * 32b). AN0 voltage is stored at [0],
 *                  AN1 at [1], etc.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetAnVoltages(bcc_drv_config_t *const drvConfig,
				    const bcc_cid_t cid,
				    uint32_t *const anVolt);

/*!
 * @brief This function reads the voltage measurement of a selected ANx and
 * converts it to [uV]. Intended for ANx configured for absolute measurements
 * only!
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param anIndex   ANx index. Use 0U for AN0, 1U for AN1, etc.
 * @param anVolt    Pointer to memory where the ANx voltage (in [uV]) will
 *                  be stored.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetAnVoltage(bcc_drv_config_t *const drvConfig,
				   const bcc_cid_t cid, uint8_t anIndex,
				   uint32_t *const anVolt);

/*!
 * @brief This function reads the BCC temperature and converts it to the
 * selected unit.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param unit      Temperature unit.
 * @param icTemp    Pointer to memory where the IC temperature will be stored.
 *                  Resolution is 0.1; unit is selected by parameter "unit".
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Meas_GetIcTemperature(bcc_drv_config_t *const drvConfig,
				       const bcc_cid_t cid,
				       bcc_temp_unit_t unit,
				       int16_t *const icTemp);

/*!
 * @brief This function reads the fault status registers of the BCC device.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param fltStatus Array containing all fault status information provided by
 *                  BCC. Required size of the array is BCC_STAT_CNT*16b.
 *                  Indexes into the array are defined in bcc_fault_status_t
 *                  enumeration placed in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Fault_GetStatus(bcc_drv_config_t *const drvConfig,
				 const bcc_cid_t cid,
				 uint16_t *const fltStatus);

/*!
 * @brief This function clears selected fault status register.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param statSel   Selection of a fault status register to be cleared. See
 *                  definition of this enumeration in this header file.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_Fault_ClearStatus(bcc_drv_config_t *const drvConfig,
				   const bcc_cid_t cid,
				   const bcc_fault_status_t statSel);

/*!
 * @brief This function sets the mode of one BCC GPIOx/ANx pin.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param gpioSel   Index of pin to be configured. Index starts at 0 (GPIO0).
 * @param mode      Pin mode.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_GPIO_SetMode(bcc_drv_config_t *const drvConfig,
			      const bcc_cid_t cid, const uint8_t gpioSel,
			      const bcc_pin_mode_t mode);

/*!
 * @brief This function reads a value of one BCC GPIO pin.
 *
 * Note that such GPIO/AN pin should be configured as digital input or output.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param gpioSel   Index of GPIOx to be read. Index starts at 0 (GPIO0).
 * @param val       Pointer where the pin value will be stored. Possible values
 *                  are: False (logic 0, low level) and True (logic 1, high
 *                  level).
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_GPIO_ReadPin(bcc_drv_config_t *const drvConfig,
			      const bcc_cid_t cid, const uint8_t gpioSel,
			      bool *const val);

/*!
 * @brief This function sets output value of one BCC GPIO pin.
 *
 * Note that this function is determined for GPIO/AN pins configured as output
 * pins.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param gpioSel   Index of GPIO output to be set. Index starts at 0 (GPIO0).
 * @param val       Output value. Possible values are: False (logic 0, low
 *                  level) and True (logic 1, high level).
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_GPIO_SetOutput(bcc_drv_config_t *const drvConfig,
				const bcc_cid_t cid, const uint8_t gpioSel,
				const bool val);

/*!
 * @brief This function enables or disables the cell balancing via
 * SYS_CFG1[CB_DRVEN] bit.
 *
 * Note that each cell balancing driver needs to be setup separately, e.g. by
 * BCC_CB_SetIndividual function.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param enable    State of cell balancing. False (cell balancing disabled) or
 *                  True (cell balancing enabled).
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_CB_Enable(bcc_drv_config_t *const drvConfig,
			   const bcc_cid_t cid, const bool enable);

/*!
 * @brief This function enables or disables cell balancing for a specified cell
 * and sets its timer.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param cellIndex Cell index. Use 0U for CELL 1, 1U for CELL2, etc.
 * @param enable    True for enabling of CB, False otherwise.
 * @param timer     Timer for enabled CB driver in minutes. Value of zero
 *                  represents 30 seconds.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_CB_SetIndividual(bcc_drv_config_t *const drvConfig,
				  const bcc_cid_t cid, const uint8_t cellIndex,
				  const bool enable, const uint16_t timer);

/*!
 * @brief This function pauses cell balancing. It can be useful during an on
 * demand conversion. As a result more precise measurement can be done. Note
 * that it is user obligation to re-enable cell balancing after measurement
 * ends.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param pause     True (pause) or False (unpause).
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_CB_Pause(bcc_drv_config_t *const drvConfig,
			  const bcc_cid_t cid, const bool pause);

/*!
 * @brief This function reads a fuse mirror register of selected BCC device.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param fuseAddr  Address of a fuse mirror register to be read.
 * @param value     Pointer to memory where the read value will be stored.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_FuseMirror_Read(bcc_drv_config_t *const drvConfig,
				 const bcc_cid_t cid, const uint8_t fuseAddr,
				 uint16_t *const value);

/*!
 * @brief This function writes a fuse mirror register of selected BCC device.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param fuseAddr  Address of a fuse mirror register to be written.
 * @param value     Value to be written.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_FuseMirror_Write(bcc_drv_config_t *const drvConfig,
				  const bcc_cid_t cid, const uint8_t fuseAddr,
				  const uint16_t value);

/*!
 * @brief This function reads an unique serial number of the BCC device from the
 * content of mirror registers.
 *
 * GUID is created according to the following table:
 * |    Device    | GUID [36:21] | GUID [20:5] | GUID [4:0] |
 * |:------------:|:------------:|:-----------:|:----------:|
 * |   MC33771C   | 0x18 [15:0]  | 0x19 [15:0] | 0x1A [4:0] |
 * | fuse address |  (16 bit)    |  (16 bit)   |  (5 bit)   |
 * |:------------:|:------------:|:-----------:|:----------:|
 * |   MC33772C   | 0x10 [15:0]  | 0x11 [15:0] | 0x12 [4:0] |
 * | fuse address |  (16 bit)    |  (16 bit)   |  (5 bit)   |
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of the BCC device.
 * @param guid      Pointer to memory where 37b unique ID will be stored.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_GUID_Read(bcc_drv_config_t *const drvConfig,
			   const bcc_cid_t cid, uint64_t *const guid);

/*!
 * @brief This function reads a byte from specified address of EEPROM memory
 * connected to BCC device via I2C bus.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of BCC device the EEPROM
 *                  memory is connected to.
 * @param addr      Address of EEPROM data will be read from. The admission
 *                  range is from 0 to 127.
 * @param data      Data read from specified address of EEPROM memory.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_EEPROM_Read(bcc_drv_config_t *const drvConfig,
			     const bcc_cid_t cid, const uint8_t addr,
			     uint8_t *const data);

/*!
 * @brief This function writes a byte to specified address of EEPROM memory
 * connected to BCC device via I2C bus.
 *
 * Warning: EEPROM write time depends on device selection and is usually
 * around 5 ms. Therefore, another EEPROM (write & read) operations to the same
 * EEPROM memory cannot be done within 5 ms after returning from this function.
 *
 * @param drvConfig Pointer to driver instance configuration.
 * @param cid       Cluster Identification Address of BCC device the EEPROM
 *                  memory is connected to.
 * @param addr      Address of EEPROM data will be written to. The admission
 *                  range is from 0 to 127.
 * @param data      Data written to specified address of EEPROM memory.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_EEPROM_Write(bcc_drv_config_t *const drvConfig,
			      const bcc_cid_t cid, const uint8_t addr,
			      const uint8_t data);

/*******************************************************************************
 * Platform specific functions
 ******************************************************************************/

/*!
 * @brief Waits for specified amount of microseconds. This function needs to be
 * implemented for specified MCU by the user.
 *
 * The timing within the implementation of this functions should be accurate.
 * E.g. In case of timer usage, the time for setting and starting the timer
 * should be taken also into account and compensated. Waiting time down to 5 us
 * must be supported.
 *
 * Note: Functions BCC_MCU_WaitUs and BCC_MCU_WaitMs are both blocking. Thus,
 * they are called independently by the BCC SW driver and can be implemented by
 * the same timer.
 *
 * Note: Timeout mechanism (implemented by functions BCC_MCU_StartTimeout and
 * BCC_MCU_TimeoutExpired) is also not used together with BCC_MCU_WaitUs.
 * I.e. resources allocated by BCC_MCU_StartTimeout function can be released
 * when BCC_MCU_WaitUs is called.
 *
 * @param delay Number of microseconds to wait.
 */
extern void BCC_MCU_WaitUs(uint32_t delay);

/*!
 * @brief Waits for specified amount of milliseconds. This function needs to be
 * implemented for specified MCU by the user.
 *
 * Note: Functions BCC_MCU_WaitUs and BCC_MCU_WaitMs are both blocking. Thus,
 * they are called independently by the BCC SW driver and can be implemented by
 * the same timer.
 *
 * Note: Timeout mechanism (implemented by functions BCC_MCU_StartTimeout and
 * BCC_MCU_TimeoutExpired) is also not used together with BCC_MCU_WaitMs.
 * I.e. resources allocated by BCC_MCU_StartTimeout function can be released
 * when BCC_MCU_WaitMs is called.
 *
 * @param delay Number of milliseconds to wait.
 */
extern void BCC_MCU_WaitMs(uint16_t delay);

/*!
 * @brief Starts a non-blocking timeout mechanism. After expiration of the time
 * passed as a parameter, function BCC_MCU_TimeoutExpired should signalize an
 * expired timeout. This function needs to be implemented for specified MCU by
 * the user.
 *
 * Note: An optimization of this function to last as short as possible is
 * recommended but not absolutely required for the proper function of the
 * driver. If the timeout mechanism is implemented by a timer, the time needed
 * for setting the timer does not have to be compensated - i.e. The timeout can
 * be signalized a bit later than requested.
 *
 * Note: When function BCC_MCU_WaitUs or BCC_MCU_WaitMs is called, information
 * about the timeout state will not be required by the BCC SW driver anymore
 * and resources allocated by this function can be released even if the timeout
 * has not expired yet.
 *
 * @param timeoutUs Length of the timeout in microseconds.
 *
 * @return Returns BCC_STATUS_TIMEOUT_START in case of error, BCC_STATUS_SUCCESS
 *         otherwise.
 */
extern bcc_status_t BCC_MCU_StartTimeout(uint32_t timeoutUs);

/*!
 * @brief Returns state of the timeout mechanism started by the function
 * BCC_MCU_StartTimeout. This function needs to be implemented for specified MCU
 * by the user.
 *
 * @return True if timeout expired, false otherwise.
 */
extern bool BCC_MCU_TimeoutExpired(void);

/*!
 * @brief User implementation of assert.
 *
 * @param x True if everything is OK.
 */
extern void BCC_MCU_Assert(const bool x);

/*!
 * @brief This function performs one 48b transfer via SPI bus. Intended for SPI
 * mode only. This function needs to be implemented for specified MCU by the
 * user.
 *
 * The byte order of buffers is given by BCC_MSG_* macros (in bcc.h).
 *
 * @param drvInstance Instance of BCC driver.
 * @param txBuf       Pointer to TX data buffer (of BCC_MSG_SIZE size).
 * @param rxBuf       Pointer to RX data buffer (of BCC_MSG_SIZE size).
 *
 * @return bcc_status_t Error code.
 */
extern bcc_status_t BCC_MCU_TransferSpi(const uint8_t drvInstance,
					uint8_t txBuf[], uint8_t rxBuf[]);

/*!
 * @brief This function sends and receives data to MC33664 via TX and RX SPI
 * buses. Intended for TPL mode only. This function needs to be implemented
 * for specified MCU by the user.
 *
 * TX SPI bus always performs only one 48b SPI transfer. Expected number of RX
 * transfers is passed as the last parameter. The byte order of buffers is given
 * by BCC_MSG_* macros (in bcc.h) and can be changed by the user.
 *
 * @param drvInstance Instance of BCC driver.
 * @param txBuf       Pointer to TX data buffer (of BCC_MSG_SIZE size).
 * @param rxBuf       Pointer to buffer for received data. Its size is at least
 *                    (BCC_MSG_SIZE * recvTrCnt) bytes.
 * @param recvTrCnt   Number of 48b transfers to be received. A non-zero value.
 *
 * @return bcc_status_t Error code.
 */
extern bcc_status_t BCC_MCU_TransferTpl(const uint8_t drvInstance,
					uint8_t txBuf[], uint8_t rxBuf[],
					const uint16_t recvTrCnt);

/*!
 * @brief Writes logic 0 or 1 to the CSB (SPI mode) or CSB_TX pin (TPL mode).
 * This function needs to be implemented by the user.
 *
 * @param drvInstance Instance of BCC driver.
 * @param value       Zero or one to be set to CSB_TX pin.
 */
extern void BCC_MCU_WriteCsbPin(const uint8_t drvInstance, const uint8_t value);

/*!
 * @brief Writes logic 0 or 1 to the RESET pin. This function needs to be
 * implemented by the user. If no RESET pin is used, keep the function body
 * empty.
 *
 * @param drvInstance Instance of BCC driver.
 * @param value       Zero or one to be set to RESET pin.
 */
extern void BCC_MCU_WriteRstPin(const uint8_t drvInstance, const uint8_t value);

/*!
 * @brief Writes logic 0 or 1 to the EN pin of MC33664. This function is
 * called only in the TPL mode and it needs to be implemented by the user.
 *
 * @param drvInstance Instance of BCC driver.
 * @param value       Zero or one to be set to EN pin.
 */
extern void BCC_MCU_WriteEnPin(const uint8_t drvInstance, const uint8_t value);

/*!
 * @brief Reads logic value of INTB pin of MC33664. This function is
 * called only in the TPL mode and it needs to be implemented by the user.
 *
 * @param drvInstance Instance of BCC driver.
 *
 * @return Zero value for logic zero, non-zero value otherwise.
 */
extern uint32_t BCC_MCU_ReadIntbPin(const uint8_t drvInstance);

/*! @} */
#ifdef __cplusplus
}
#endif

#endif /* __BCC_H__ */
/*******************************************************************************
 * EOF;
 ******************************************************************************/
