#include "acu.h"

extern uint8_t get_state();
extern void print_lpuart(char *arr);

extern bool check_ts_active;
extern char print_buffer[1000];
extern volatile CAN_RX_message
    CAN_RxBuffer[256]; // Array to store received CAN data
extern volatile uint8_t
    CAN_RxBufferBottom; // Index of oldest data ==> increment this whenever the
			// data is processed
extern volatile uint8_t CAN_RxBufferTop;
extern volatile uint8_t CAN_RxBufferLevel;

extern FDCAN_TxHeaderTypeDef TxHeader;
extern FDCAN_RxHeaderTypeDef RxHeader;
extern FDCAN_TxHeaderTypeDef TxHeader_Data;
extern FDCAN_RxHeaderTypeDef RxHeader_Data;
extern FDCAN_TxHeaderTypeDef TxHeader_Charger;
extern FDCAN_RxHeaderTypeDef RxHeader_Charger;

extern uint8_t CAN_TxData[64];
extern uint8_t CAN_RxData[64];
extern uint16_t adc_data[6];

extern volatile uint8_t p_top, p_bottom, p_level, d_top, d_bottom, d_level,
    c_top, c_bottom, c_level;
extern volatile uint8_t prim_q[256], data_q[256], charger_q[256];
extern volatile uint8_t CAN_1_flag, CAN_2_flag, CAN_3_flag;

data_union unicorn;

// Molicel P45B voltage to mAh lookup table
#define CELL_CHGR_ARR_SIZE 339
// mAh
const float cell_charge_tbl[] = {
    4627.54, 4625.92, 4624.54, 4622.87, 4621.72, 4619.81, 4618.07, 4617.07,
    4615.09, 4613.42, 4611.82, 4609.86, 4607.96, 4605.25, 4603.26, 4602.59,
    4600.89, 4598.53, 4596.67, 4595.12, 4594.24, 4592.32, 4590.77, 4587.59,
    4585.82, 4584.2,  4582.35, 4581.52, 4579.47, 4578.09, 4576.29, 4575.26,
    4573.49, 4571.63, 4569.04, 4566.68, 4564.69, 4562.33, 4559.81, 4557.66,
    4554.64, 4552.49, 4549.27, 4548.14, 4544.93, 4542.8,  4540.59, 4538.9,
    4536.47, 4534.02, 4531.63, 4528.95, 4526.18, 4524.42, 4521.84, 4517.77,
    4514.97, 4512,    4507.87, 4505.07, 4502.77, 4499.3,  4495.07, 4491.43,
    4487.02, 4484.23, 4479.72, 4476.8,	4474.06, 4471.92, 4468.68, 4463.54,
    4460.4,  4456.37, 4451.16, 4447.1,	4442.15, 4438.49, 4434.88, 4433.17,
    4428.66, 4425.35, 4420.02, 4415.89, 4410.34, 4404.8,  4400.44, 4394.9,
    4389,    4383.45, 4377.3,  4372.22, 4369.26, 4366.32, 4360.4,  4354.34,
    4349.17, 4342.36, 4335.8,  4332.84, 4326.94, 4319.25, 4314.74, 4306.69,
    4300.11, 4293.77, 4287.18, 4279.64, 4269.74, 4263.75, 4257.63, 4252.46,
    4244.99, 4237.17, 4229.24, 4221.43, 4215.22, 4206.2,  4200.74, 4190.76,
    4182.05, 4168.75, 4160.68, 4157.14, 4148.79, 4139.86, 4126.85, 4121.08,
    4111.93, 4102.47, 4092.19, 4082.73, 4073.28, 4066.18, 4056.72, 4049.31,
    4040.02, 4031.06, 4021.25, 4012.52, 4004.7,	 3995.53, 3984.59, 3977.14,
    3967.75, 3955.26, 3948.99, 3944.32, 3934.29, 3925.43, 3914.93, 3906.42,
    3899.19, 3890.84, 3880.72, 3863.73, 3853.01, 3843.41, 3834.77, 3825,
    3813.2,  3800.69, 3791.7,  3781.13, 3767.74, 3755.12, 3742.17, 3734.3,
    3722.77, 3710.72, 3698.45, 3684.64, 3670.17, 3655.25, 3641.88, 3633.24,
    3618.45, 3608.95, 3593.94, 3572.7,	3553.74, 3538.82, 3525.61, 3513.72,
    3500.56, 3485.51, 3471.01, 3460.58, 3445.57, 3431.73, 3417.37, 3394.98,
    3374.76, 3356.35, 3337.21, 3305.94, 3276.68, 3249.46, 3206.92, 3173.72,
    3157.99, 3128.49, 3107.51, 3088.68, 3073.79, 3059.66, 3041.47, 3019.46,
    3001.45, 2981.72, 2962.89, 2942.42, 2929.78, 2907.75, 2886.4,  2867.57,
    2846.13, 2823.46, 2800.63, 2780.89, 2758.59, 2736.76, 2719.79, 2697.4,
    2673.67, 2651.65, 2631.19, 2601.09, 2583.81, 2564.08, 2534.88, 2517.89,
    2494.68, 2464.21, 2447.3,  2425.29, 2396.08, 2379.55, 2357.53, 2335.24,
    2319.79, 2284.45, 2269.25, 2245.64, 2219.57, 2204.49, 2182.68, 2152.23,
    2128.34, 2103.06, 2081.86, 2052.75, 2030.65, 2009.46, 1987.45, 1956.71,
    1920.29, 1895.96, 1869.63, 1849.59, 1825.16, 1801.06, 1769.25, 1728.15,
    1700.51, 1676.13, 1642.28, 1616.11, 1586.92, 1560,	  1532.11, 1503.54,
    1474.7,  1443.06, 1423.32, 1402.86, 1378.55, 1357.84, 1336.85, 1316.76,
    1294.86, 1271.84, 1252.9,  1233.73, 1215.88, 1196.98, 1177.18, 1162.4,
    1138.75, 1120.65, 1108.18, 1093.26, 1077.7,	 1053.41, 1036.94, 1021.58,
    1001.07, 982.52,  968.03,  944.32,	923,	 900.06,  880.41,  863.43,
    834.11,  813.08,  794.26,  761.75,	729.25,	 676.58,  616.79,  557,
    487.74,  418.47,  369.77,  321.07,	272.38,	 244.14,  215.9,   187.67,
    169.83,  151.99,  134.15,  119.55,	104.94,	 90.34,	  75.74,   66,
    56.26,   46.53,   40.04,   33.55,	27.06,	 20.57,	  14.08,   10.56,
    7.04,    3.52,    0};
const float cell_volts_tbl[] = {
    2.5,   2.505, 2.51,	 2.515, 2.52,  2.525, 2.53,  2.535, 2.54,  2.545, 2.55,
    2.555, 2.56,  2.565, 2.57,	2.575, 2.58,  2.585, 2.59,  2.595, 2.6,	  2.605,
    2.61,  2.615, 2.62,	 2.625, 2.63,  2.635, 2.64,  2.645, 2.65,  2.655, 2.66,
    2.665, 2.67,  2.675, 2.68,	2.685, 2.69,  2.695, 2.7,   2.705, 2.71,  2.715,
    2.72,  2.725, 2.73,	 2.735, 2.74,  2.745, 2.75,  2.755, 2.76,  2.765, 2.77,
    2.775, 2.78,  2.785, 2.79,	2.795, 2.8,   2.805, 2.81,  2.815, 2.82,  2.825,
    2.83,  2.835, 2.84,	 2.845, 2.85,  2.855, 2.86,  2.865, 2.87,  2.875, 2.88,
    2.885, 2.89,  2.895, 2.9,	2.905, 2.91,  2.915, 2.92,  2.925, 2.93,  2.935,
    2.94,  2.945, 2.95,	 2.955, 2.96,  2.965, 2.97,  2.975, 2.98,  2.985, 2.99,
    2.995, 3,	  3.005, 3.01,	3.015, 3.02,  3.025, 3.03,  3.035, 3.04,  3.045,
    3.05,  3.055, 3.06,	 3.065, 3.07,  3.075, 3.08,  3.085, 3.09,  3.095, 3.1,
    3.105, 3.11,  3.115, 3.12,	3.125, 3.13,  3.135, 3.14,  3.145, 3.15,  3.155,
    3.16,  3.165, 3.17,	 3.175, 3.18,  3.185, 3.19,  3.195, 3.2,   3.205, 3.21,
    3.215, 3.22,  3.225, 3.23,	3.235, 3.24,  3.245, 3.25,  3.255, 3.26,  3.265,
    3.27,  3.275, 3.28,	 3.285, 3.29,  3.295, 3.3,   3.305, 3.31,  3.315, 3.32,
    3.325, 3.33,  3.335, 3.34,	3.345, 3.35,  3.355, 3.36,  3.365, 3.37,  3.375,
    3.38,  3.385, 3.39,	 3.395, 3.4,   3.405, 3.41,  3.415, 3.42,  3.425, 3.43,
    3.435, 3.44,  3.445, 3.45,	3.455, 3.46,  3.465, 3.47,  3.475, 3.48,  3.485,
    3.49,  3.495, 3.5,	 3.505, 3.51,  3.515, 3.52,  3.525, 3.53,  3.535, 3.54,
    3.545, 3.55,  3.555, 3.56,	3.565, 3.57,  3.575, 3.58,  3.585, 3.59,  3.595,
    3.6,   3.605, 3.61,	 3.615, 3.62,  3.625, 3.63,  3.635, 3.64,  3.645, 3.65,
    3.655, 3.66,  3.665, 3.67,	3.675, 3.68,  3.685, 3.69,  3.695, 3.7,	  3.705,
    3.71,  3.715, 3.72,	 3.725, 3.73,  3.735, 3.74,  3.745, 3.75,  3.755, 3.76,
    3.765, 3.77,  3.775, 3.78,	3.785, 3.79,  3.795, 3.8,   3.805, 3.81,  3.815,
    3.82,  3.825, 3.83,	 3.835, 3.84,  3.845, 3.85,  3.855, 3.86,  3.865, 3.87,
    3.875, 3.88,  3.885, 3.89,	3.895, 3.9,   3.905, 3.91,  3.915, 3.92,  3.925,
    3.93,  3.935, 3.94,	 3.945, 3.95,  3.955, 3.96,  3.965, 3.97,  3.975, 3.98,
    3.985, 3.99,  3.995, 4,	4.005, 4.01,  4.015, 4.02,  4.025, 4.03,  4.035,
    4.04,  4.045, 4.05,	 4.055, 4.06,  4.065, 4.07,  4.075, 4.08,  4.085, 4.09,
    4.095, 4.1,	  4.105, 4.11,	4.115, 4.12,  4.125, 4.13,  4.135, 4.14,  4.145,
    4.15,  4.155, 4.16,	 4.165, 4.17,  4.175, 4.18,  4.185, 4.19};

/// @brief converts data stored in ACU to one that can be sent over CAN bus
/// @param offset offset
/// @param source the float that ACU stores
/// @param size the number of bytes to copy
/// @param type the type of number we want to convert to
void prepare_can_send(uint8_t offset, float source, uint8_t type)
{
	if (type == UNSIGNED_16) {
		uint16_t tempVar = (uint16_t)source;
		memcpy(&CAN_TxData[offset], &tempVar, sizeof(uint16_t));
	} else {
		int16_t tVar = (int16_t)source;
		memcpy(&CAN_TxData[offset], &tVar, sizeof(int16_t));
	}
}

/// @brief initialize acu variables, reads data, updates adc
/// @param acu
void acu_init(ACU *acu)
{

	acu->relay_state = 0;
	acu->em->em_current = 0;
	acu->em->em_voltage = 0;
	acu->em->energy = 0;
	acu->em->min_temp = 0;
	acu->em->max_temp = 0;
	acu->em->num_sensors = 0;
	acu->em->status = 2;

	acu->imd->id = 0;
	acu->imd->hv_system_voltage = 0;
	acu->imd->r_iso_status = 0;
	acu->imd->r_iso_meas_count = 0;

	acu->imd->isolation_quality = 0;
	acu->imd->status_device_activity = 0;
	acu->imd->r_iso_corrected = 0;
	acu->imd->status_warnings_alarms = 0;

	acu->imd->r_iso_negative = 0;
	acu->imd->r_iso_positive = 0;
	acu->imd->r_iso_original = 0;
	acu->imd->iso_meas_count = 0;

	acu->chgr->chgr_status = 0;
	acu->chgr->charger_output_current = 0;
	acu->chgr->charger_output_voltage = 0;

	acu->lastChrgRecieveTime = 0;
	acu->chg_ctrl = NO_CHARGE;
	acu->acuErrCount = 0;
	acu->acu_err_warns = 0;

	acu->target_chg_voltage =
	    NUM_CELL_IC * NUM_TOTAL_IC * CELL_FULL_VOLTAGE;
	acu->bat_soc = 0.0f;

	// fix later
	// uint8_t count = 0;
	// while (fabsf(get_total_voltage(acu) - 1.235f) > ERRMG_ISNS_VREF) {
	//     // HV current too far from zero. Check hardware.
	//     if (count > 10) {
	//         for (uint8_t i = 0; i < NUM_TOTAL_IC; i++){
	//             acu->bty->stack_voltage[i] = 1.235f;
	//         }
	//         break;
	//     }
	//     count++;
	//     LL_mDelay(500);
	// }
}

/// @brief ACU check => current, glv voltage, shut down voltage, warnings
/// @param acu
/// @param state
/// @param startup
/// @return True if passes, False otherwise
bool acu_check(ACU *acu, bool startup)
{

	// clean the slate
	acu->acu_err_warns &= ~(ACU_CLEAR_ERRR);
	acu->acu_err_warns &= ~(ACU_CLEAR_WARN);

	uint8_t lastAcuErrCount = acu->acuErrCount;

	bool hasErrors = false;

	// (2) check overcurrent
	if (acu->ts_current > MAX_HV_CURRENT) {
		print_lpuart("Max HV Current detected\n");
		hasErrors = true;
		acu->acuErrCount++;
		if (acu->acuErrCount >= ERRMG_ACU_ERR) {
			acu->acuErrCount = ERRMG_ACU_ERR;
			acu->acu_err_warns |= ACU_ERR_OVER_CURR;
		}
	} else if (acu->ts_current > MAX_HV_CURRENT * 0.8f) {
		print_lpuart("High Current Warning\n");
	}

	// glv voltage => TODO: check this
	if (acu->voltage_12v < MIN_GLV_VOLT) {
		print_lpuart("GLV Undervolt detected\n");
		acu->acuErrCount++;
		hasErrors = true;
		if (acu->acuErrCount >= ERRMG_ACU_ERR) {
			acu->acuErrCount = ERRMG_ACU_ERR;
			acu->acu_err_warns |= ACU_ERR_UNDER_VOLT;
		}
	}
	if (acu->voltage_12v > MAX_GLV_VOLT) {
		print_lpuart("GLV Overvolt detected\n");
		acu->acuErrCount++;
		hasErrors = true;
		if (acu->acuErrCount >= ERRMG_ACU_ERR) {
			acu->acuErrCount = ERRMG_ACU_ERR;
			acu->acu_err_warns |= ACU_ERR_OVER_VOLT;
		}
	}

	// sdc_volt_v should be close to voltage_12v (glv_voltage)
	if (fabsf(acu->sdc_volt_v - acu->voltage_12v) > GLV_SDC_LOW &&
	    !startup && get_state() == 3) {
		print_lpuart("SD Volt not close enough to GLV\n");

		if (acu->sdc_volt_v < acu->voltage_12v) {
			print_lpuart("ACU_ERR_UNDER_VOLT detected\n");
			acu->acuErrCount++;
			hasErrors = true;
			if (acu->acuErrCount >= ERRMG_ACU_ERR) {
				acu->acuErrCount = ERRMG_ACU_ERR;
				acu->acu_err_warns |= ACU_ERR_UNDER_VOLT;
			}
		} else if (acu->sdc_volt_v > acu->voltage_12v) {
			print_lpuart("ACU_ERR_OVER_VOLT detected\n");
			acu->acuErrCount++;
			hasErrors = true;
			if (acu->acuErrCount >= ERRMG_ACU_ERR) {
				acu->acuErrCount = ERRMG_ACU_ERR;
				acu->acu_err_warns |= ACU_ERR_OVER_VOLT;
			}
		}
	}

	if (acu->water_sense < 1.5f) {
		// TODO: turn off pump & high voltage
		// hasErrors = true;
		// acu->acuErrCount++;
		turn_of_hv_pump(0);
		print_lpuart(
		    "Error in water sense! Ur cooked! [Ignoring for now]\n");
	}

	acu->acuErrCount = (lastAcuErrCount == acu->acuErrCount && !hasErrors)
			       ? 0
			       : acu->acuErrCount;

	if (acu->voltage_12v < UNDER_VOLTAGE_GLV) {
		acu->acu_err_warns |= ACU_ERR_UV_12_V;
	}
	if (acu->sdc_volt_v < UNDER_VOLTAGE_SDCV) {
		acu->acu_err_warns |= ACU_ERR_UV_SDC;
	}

	// IMD iso failure
	if (acu->imd->status_warnings_alarms > 0 &&
	    acu->imd->status_warnings_alarms < 2048) {

		// TODO: uncomment this later
		// hasErrors = true; undo this for now

		print_imd_err_warn(acu);
		print_lpuart("IMD failures exist!\n");
		acu->imd->status_warnings_alarms = 0;
	}
	return (!hasErrors) && (acu->acu_err_warns != 0);
}

/// @brief convert byte arrays to float
/// @param data array of bytes
/// @param size size of array
/// @return
float magical_union_flt(uint8_t data[], uint8_t size, bool big_endian)
{
	memset(&unicorn, 0, sizeof(unicorn));
	if (big_endian == false) {
		for (size_t i = 0; i < size; i++) {
			unicorn.byts[i] = data[i];
		}
	} else {
		for (size_t i = 0; i < (int)size; i++) {
			unicorn.byts[size - i - 1] = (int)data[i];
		}
	}
	return unicorn.flt;
}

/// @brief Charger Data Receive: Calculating Bytes 1-4
/// @param data data being sent/received, 2 bytes total
/// @param weird true if weird
/// @return uint16_t value to store
int16_t magical_union_chgr_rcv(uint8_t data[], bool weird)
{
	memset(&unicorn, 0, sizeof(unicorn));
	if (!weird) {
		unicorn.byts[0] = data[1];
		unicorn.byts[1] = data[0];
	} else {
		unicorn.byts[0] = data[1];
		unicorn.byts[1] =
		    data[0] & 0x7F; // mask out the "highest bit" => direction
	}
	return unicorn.i16;
}

void magical_union_chgr_send(uint8_t *buffer, float data)
{
	memset(&unicorn, 0, sizeof(unicorn));
	int16_t rounded = roundf(data * 10.0f);
	unicorn.u16 = rounded;
	buffer[0] = unicorn.byts[1];
	buffer[1] = unicorn.byts[0];
}

/// @brief convert byte arrays to uint16_t
/// @param data array of bytes
/// @return
uint16_t magical_union_u16(uint8_t data[])
{
	memset(&unicorn, 0, sizeof(unicorn));
	unicorn.byts[0] = data[0];
	unicorn.byts[1] = data[1];
	return unicorn.u16;
}

int32_t magical_union_i32(uint8_t data[], bool big_endian)
{
	memset(&unicorn, 0, sizeof(unicorn));
	if (big_endian == false) {
		unicorn.byts[0] = data[0];
		unicorn.byts[1] = data[1];
		unicorn.byts[2] = data[2];
		unicorn.byts[3] = data[3];
	} else {
		unicorn.byts[0] = data[3];
		unicorn.byts[1] = data[2];
		unicorn.byts[2] = data[1];
		unicorn.byts[3] = data[0];
	}
	return unicorn.i32;
}

/// @brief Checks CAN_RxBuffer & parses messages in FIFO order
/// @attention Case: too many messages to parse, maybe we have some MAX_PARSE
/// val?
/// @param acu
void can_read_handler(ACU *acu)
{
	while (CAN_RxBufferLevel > 0) {
		FDCAN_GlobalTypeDef *type =
		    CAN_RxBuffer[CAN_RxBufferBottom].instance;
		uint32_t id = CAN_RxBuffer[CAN_RxBufferBottom].identifier;
		can_read(acu, type, id,
			 (uint8_t *)(&CAN_RxBuffer[CAN_RxBufferBottom].data));
		bzero((void *)CAN_RxBuffer[CAN_RxBufferBottom].data,
		      sizeof(CAN_RxBuffer[CAN_RxBufferBottom].data));
		CAN_RxBufferBottom++;
		CAN_RxBufferLevel--;
	}
}

/// @brief Parses a single CAN message
/// @param acu acu in question
/// @param which_can between FDCAN_1, FDCAN_2, FDCAN_3
/// @param id CAN Message ID
/// @param size number of bytes
/// @param data data in question
void can_read(ACU *acu, FDCAN_GlobalTypeDef *which_can, uint32_t id,
	      uint8_t *data)
{
	uint32_t curr = HAL_GetTick();
	switch (id) {
	case Debug_2_ACU:
		enqueue(ACU_Debug_2_Debug, which_can);
		break;
	case Debug_FD_ACU:
		enqueue(ACU_Debug_FD, which_can);
		break;
	case Debug_Ping_ACU:
		enqueue(ACU_Ping_Debug, which_can);
		break;
	case ECU_Ping_ALL:
		enqueue(ACU_Ping_ECU, which_can);
		break;
	case Precharge_ACU:
		acu->ts_active = data[0] & 1U; // @details: command to precharge
		check_ts_active = true;
		if (get_state() != 1) {
			print_lpuart(
			    "[WARNING]: Precharge msg when != PRECHARGE???\n");
		}
		break;
	case Charger_Data_ACU:
		acu->lastChrgRecieveTime = curr;
		// acu->chgr->charger_output_voltage =
		// (int16_t)(magical_union_chgr_rcv(data, false) * 0.1);
		// acu->chgr->charger_output_current =
		// (int16_t)(magical_union_chgr_rcv(data+2, true) * 0.1f);
		memcpy(&acu->chgr->charger_output_voltage, &data[0], 2);
		memcpy(&acu->chgr->charger_output_current, &data[2], 2);
		acu->chgr->chgr_status = data[4];
		break;
	case Config_Charge_ACU:
		acu->target_chg_voltage = magical_union_u16(data) * 0.1f;
		acu->target_chg_current = magical_union_u16(data + 2) * 0.1f;
		break;
	case Config_Ops_ACU:
		acu->bty->min_volt_thresh = magical_union_u16(data) * 0.1f;
		acu->bty->max_temp_thresh = magical_union_u16(data + 2) * 0.1f;
		break;
	case EM_Measurements_ACU: // @remark: double check this
		acu->em->em_current = magical_union_flt(data, 4, true);
		acu->em->em_voltage = magical_union_flt(data + 4, 4, true);
		break;
	case EM_Data_1_ACU:
		acu->em->team_data[0] = magical_union_i32(data, true);
		acu->em->team_data[1] = magical_union_i32(data + 4, true);
		break;
	case EM_Data_2_ACU:
		acu->em->team_data[2] = magical_union_i32(data, true);
		acu->em->team_data[3] = magical_union_i32(data + 4, true);
		break;
	case EM_Status_ACU:
		acu->em->status = data[0];
		acu->em->energy = magical_union_flt(data + 1, 4, true);
		break;
	case EM_Temperature_ACU:
		uint8_t mux_signal = data[0] & 0b11100000;
		acu->em->num_sensors = (data[0] & 0b00011111) << 3;

		acu->em->min_temp = (uint8_t)(data[1] * 0.5f);
		acu->em->max_temp = (uint8_t)(data[2] * 0.5f);

		acu->em->temps[mux_signal * 5] = (float)(data[3] * 0.5f);
		acu->em->temps[mux_signal * 5 + 1] = (float)(data[4] * 0.5f);

		if (mux_signal != 6) {
			acu->em->temps[mux_signal * 5 + 2] =
			    (float)(data[5] * 0.5f);
			acu->em->temps[mux_signal * 5 + 3] =
			    (float)(data[6] * 0.5f);
			acu->em->temps[mux_signal * 5 + 4] =
			    (float)(data[7] * 0.5f);
		}
		break;
	case IMD_Response_ACU:
		acu->imd->id = data[0];
		break;
	case IMD_Isolation_ACU:
		acu->imd->r_iso_negative = magical_union_u16(data);
		acu->imd->r_iso_positive = magical_union_u16(data + 2);
		acu->imd->r_iso_original = magical_union_u16(data + 4);
		acu->imd->iso_meas_count = data[6];
		acu->imd->isolation_quality = data[7];
		break;
	case IMD_Voltage_ACU:
		acu->imd->hv_system_voltage =
		    (magical_union_u16(data) - 32128) * 0.05f;
		break;
	case IMD_IT_System_ACU:
		break;
	case IMD_Request_ACU:
		acu->imd->id = data[0];
		break;
	case IMD_General_ACU:
		acu->imd->r_iso_corrected = magical_union_u16(data);
		acu->imd->r_iso_status = data[2];
		acu->imd->r_iso_meas_count = data[3];
		acu->imd->status_warnings_alarms = magical_union_u16(data + 4);
		acu->imd->status_device_activity = data[6];
		break;
	default:
		break;
	}
}

/// @brief FIFO transfer of CAN messages
/// @param acu
void dequeue(ACU *acu)
{
	if (CAN_1_flag == 0 && CAN_2_flag == 0 && CAN_3_flag == 0) {
		return;
	}
	// priority 1: CAN_Primary
	switch (CAN_1_flag) {
	case 1: // ACU_Debug_2_Debug
		memcpy(CAN_TxData, CAN_RxData, 8 * sizeof(uint8_t));
		TxHeader.Identifier = ACU_Debug_2_Debug;
		TxHeader.DataLength = FDCAN_DLC_BYTES_8;
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Debug_2_Debug failed...\n");
		}
		CAN_1_flag = 0;
		break;
	case 2: // ACU_Ping_Debug
		memcpy(CAN_TxData, CAN_RxData, 4 * sizeof(uint8_t));
		TxHeader.Identifier = ACU_Ping_Debug;
		TxHeader.DataLength = FDCAN_DLC_BYTES_4;
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader_Data,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Ping_Debug > hfdcan1 failed...\n");
		}
		CAN_1_flag = 0;
		break;
	case 3: // ACU_Ping_ECU
		memcpy(CAN_TxData, CAN_RxData, 4 * sizeof(uint8_t));
		TxHeader.Identifier = ACU_Ping_ECU;
		TxHeader.DataLength = FDCAN_DLC_BYTES_4;
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Ping_ECU > hfdcan1 failed...\n");
		}
		CAN_1_flag = 0;
		break;
	case 4:
		memcpy(CAN_TxData, CAN_RxData, 64 * sizeof(uint8_t));
		TxHeader.Identifier = ACU_Debug_FD;
		TxHeader.DataLength = FDCAN_DLC_BYTES_64;
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Debug_FD > hfdcan1 failed...\n");
		}
		CAN_1_flag = 0;
		break;
	case 5: // ACU_Status_1
		TxHeader.Identifier = ACU_Status_1;
		TxHeader.DataLength = FDCAN_DLC_BYTES_8;

		CAN_TxData[0] =
		    (((uint16_t)((acu->bty->battery_total_voltage) * 100.0f)) &
		     0xFF);
		CAN_TxData[1] =
		    ((((uint16_t)((acu->bty->battery_total_voltage) *
				  100.0f)) >>
		      8) &
		     0xFF);
		CAN_TxData[2] =
		    (uint8_t)((uint16_t)(acu->ts_voltage * 100.0f) & 0xFF);
		CAN_TxData[3] =
		    (uint8_t)((uint16_t)(acu->ts_voltage * 100.0f) >> 8) & 0xFF;
		CAN_TxData[4] =
		    (uint8_t)((int8_t)(acu->ts_current * 100.0f) & 0xFF);
		CAN_TxData[5] =
		    (((uint16_t)(acu->ts_current * 100.0f) >> 8) & 0xFF);
		CAN_TxData[6] = (uint8_t)(acu->bat_soc * 51.0f * 0.2f);
		CAN_TxData[7] =
		    (uint8_t)(calculate_glv_soc(acu) * 51.0f * 0.2f);
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Status_1 failed...\n");
		}
		CAN_1_flag = 0;
		break;
	case 6: // ACU_Status_2
		TxHeader.Identifier = ACU_Status_2;
		TxHeader.DataLength = FDCAN_DLC_BYTES_7;
		CAN_TxData[0] = 0; // 20v GLV voltage
		CAN_TxData[1] = (uint8_t)(acu->voltage_12v * 10.0f);
		CAN_TxData[2] = (uint8_t)(acu->sdc_volt_v * 10.0f);
		CAN_TxData[3] =
		    (uint8_t)((acu->bty->min_cell_volt - 2.0f) * 100.0f);
		CAN_TxData[4] = (uint8_t)(acu->bty->max_cell_temp * 4.0f);
		CAN_TxData[5] = (uint8_t)(acu->acu_err_warns &
					  0xFF); // takes [OT, OV, UV, OC, UC,
						 // UV_20v, UV_GLV, UV_SDC]
		CAN_TxData[6] =
		    acu->acu_err_warns &
		    ACU_PRECHARGE >> 7; // takes precharge error to lsb
		CAN_TxData[6] |= (acu->relay_state & AIR_MINUS) >> 1;
		CAN_TxData[6] |= (acu->relay_state & AIR_PLUS) >> 1;
		CAN_TxData[6] |= (acu->relay_state & ACU_LATCH) << 3;

		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Status_2 failed...\n");
		}
		CAN_1_flag = 0;
		break;
	case 7: // ACU_Status_3
		CAN_1_flag = 0;
		break;
	default:
		CAN_1_flag = 0;
		break;
	}
	// priority 2: CAN_Data
	switch (CAN_2_flag) {
	case 1: // ACU_Ping_Debug
		memcpy(CAN_TxData, CAN_RxData, 4 * sizeof(uint8_t));
		TxHeader_Data.Identifier = ACU_Ping_Debug;
		TxHeader_Data.DataLength = FDCAN_DLC_BYTES_4;
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader_Data,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Ping_Debug > hfdcan2 failed...\n");
		}
		CAN_2_flag = 0;
		break;
	case 2: // ACU_Ping_ECU
		memcpy(CAN_TxData, CAN_RxData, 4 * sizeof(uint8_t));
		TxHeader.Identifier = ACU_Ping_ECU; // @attention check if it's
						    // sent through CAN1 or CAN2
		TxHeader.DataLength = FDCAN_DLC_BYTES_4;
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Ping_ECU > hfdcan2 failed...\n");
		}
		CAN_2_flag = 0;
		break;
	case 3: // ACU_Cell_Data_1
		TxHeader_Data.Identifier = ACU_Cell_Data_1;
		TxHeader_Data.DataLength = FDCAN_DLC_BYTES_64;

		// cell_volt range: [0:32]
		// cell_temp range: [0:64]
		for (uint8_t cell = 0U; cell < 32U; cell++) {
			float max_temp =
			    fmaxf(acu->bty->cell_temp[cell * 2] * 4.0f,
				  acu->bty->cell_temp[cell * 2 + 1] * 4.0f);
			CAN_TxData[2 * cell] = fconstrain(
			    (acu->bty->cell_volt[cell] - 2.0f) * 100.0f);
			CAN_TxData[2 * cell + 1] = fconstrain(max_temp);
		}
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader_Data,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Cell_Data_1 failed...\n");
		}
		CAN_2_flag = 0;
		break;
	case 4: // ACU_Cell_Data_2
		TxHeader_Data.Identifier = ACU_Cell_Data_2;
		TxHeader_Data.DataLength = FDCAN_DLC_BYTES_64;

		// cell_volt range: [32:64]
		// cell_temp range: [64:128]
		for (uint8_t cell = 0U; cell < 32U; cell++) {
			float max_temp = fmaxf(
			    acu->bty->cell_temp[cell * 2 + 64] * 4.0f,
			    acu->bty->cell_temp[cell * 2 + 64 + 1] * 4.0f);
			CAN_TxData[2 * cell] = fconstrain(
			    (acu->bty->cell_volt[cell + 32] - 2.0f) * 100.0f);
			CAN_TxData[2 * cell + 1] = fconstrain(max_temp);
		}
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader_Data,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Cell_Data_2 failed...\n");
		}
		CAN_2_flag = 0;
		break;
	case 5: // ACU_Cell_Data_3
		TxHeader_Data.Identifier = ACU_Cell_Data_3;
		TxHeader_Data.DataLength = FDCAN_DLC_BYTES_64;

		// cell_volt range: [64:96]
		// cell_temp range: [128:192]
		for (uint8_t cell = 0U; cell < 32U; cell++) {
			float max_temp = fmaxf(
			    acu->bty->cell_temp[cell * 2 + 128] * 4.0f,
			    acu->bty->cell_temp[cell * 2 + 128 + 1] * 4.0f);
			CAN_TxData[2 * cell] = fconstrain(
			    (acu->bty->cell_volt[cell + 64] - 2.0f) * 100.0f);
			CAN_TxData[2 * cell + 1] = fconstrain(max_temp);
		}
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader_Data,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Cell_Data_3 failed...\n");
		}
		CAN_2_flag = 0;
		break;
	case 6: // ACU_Cell_Data_4
		TxHeader_Data.Identifier = ACU_Cell_Data_4;
		TxHeader_Data.DataLength = FDCAN_DLC_BYTES_64;

		// cell_volt range: [96:128]
		// cell_temp range: [192:256]
		for (uint8_t cell = 0U; cell < 64U; cell += 2) {
			// float max_temp =
			// fmaxf(acu->bty->cell_temp[cell*2+128]*4.0f,
			// acu->bty->cell_temp[cell*2+128+1]*4.0f);
			// CAN_TxData[2*cell] =
			// fconstrain((acu->bty->cell_volt[cell+64] - 2.0f) *
			// 100.0f); CAN_TxData[2*cell+1] = fconstrain(max_temp);
		}
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader_Data,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Cell_Data_4 failed...\n");
		}
		CAN_2_flag = 0;
		break;
	case 7: // ACU_Cell_Data_5
		TxHeader_Data.Identifier = ACU_Cell_Data_5;
		TxHeader_Data.DataLength = FDCAN_DLC_BYTES_64;
		for (uint8_t cell = 0U; cell < 12U; cell += 2) {
			// float max_temp =
			// fmaxf(acu->bty->cell_temp[2*cell+256]*4.0f,
			// acu->bty->cell_temp[2*cell+256+1]*4.0f);
			// CAN_TxData[cell] =
			// fconstrain((acu->bty->cell_volt[cell+128] - 2.0f) *
			// 100.0f); CAN_TxData[cell+1] = fconstrain(max_temp);
		}
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader_Data,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Cell_Data_5 failed...\n");
		}
		CAN_2_flag = 0;
		break;
	default:
		CAN_2_flag = 0;
		break;
	}
	// priority 3: CAN_Charger
	switch (CAN_3_flag) {
	case 1:
		TxHeader_Charger.Identifier = ACU_Charger_Control;
		TxHeader_Charger.DataLength = FDCAN_DLC_BYTES_5;
		magical_union_chgr_send(CAN_TxData,
					acu->target_chg_voltage * 10.0f);
		magical_union_chgr_send(CAN_TxData + 2,
					acu->target_chg_current * 10.0f);
		CAN_TxData[4] = acu->chg_ctrl;
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &TxHeader_Charger,
						  CAN_TxData) != HAL_OK) {
			print_lpuart("ACU_Charger_Control failed...\n");
		}
		CAN_3_flag = 0;
		break;
	default:
		break;
	}
}

/// @brief Takes incoming CAN message request and put into send buffer
/// @param id
/// @param which_can
void enqueue(uint32_t id, FDCAN_GlobalTypeDef *which_can)
{
	__disable_irq();
	uint8_t primary_full = (p_level >= 255U);
	uint8_t data_full = (d_level >= 255U);
	uint8_t charger_full = (c_level >= 255U);
	__enable_irq();
	switch (id) {
	case ACU_Debug_2_Debug:
		if (primary_full) {
			return;
		}
		__disable_irq();
		prim_q[p_top] = 1;
		p_top++;
		p_level++;
		__enable_irq();
		break;
	case ACU_Ping_Debug:
		if (which_can == FDCAN1) {
			if (primary_full) {
				return;
			}
			__disable_irq();
			prim_q[p_top] = 2;
			p_top++;
			p_level++;
			__enable_irq();
		} else if (which_can == FDCAN2) {
			if (data_full) {
				return;
			}
			__disable_irq();
			data_q[d_top] = 1;
			d_top++;
			d_level++;
			__enable_irq();
		}
		break;
	case ACU_Ping_ECU:
		if (which_can == FDCAN1) {
			if (primary_full) {
				return;
			}
			__disable_irq();
			prim_q[p_top] = 3;
			p_top++;
			p_level++;
			__enable_irq();
		} else if (which_can == FDCAN2) {
			if (data_full) {
				return;
			}
			__disable_irq();
			data_q[d_top] = 2;
			d_top++;
			d_level++;
			__enable_irq();
		}
		break;
	case ACU_Debug_FD:
		if (data_full) {
			return;
		}
		__disable_irq();
		data_q[d_top] = 3;
		d_top++;
		d_level++;
		__enable_irq();
		break;
	case ACU_Status_1:
		// print_lpuart("enqueue ACU_Status_1\n");
		if (primary_full) {
			return;
		}
		__disable_irq();
		prim_q[p_top] = 5;
		p_top++;
		p_level++;
		__enable_irq();
		break;
	case ACU_Status_2:
		// print_lpuart("enqueue ACU_Status_2\n");
		if (primary_full) {
			return;
		}
		__disable_irq();
		prim_q[p_top] = 6;
		p_top++;
		p_level++;
		__enable_irq();
		break;
	case ACU_Status_3:
		break;
	case ACU_Cell_Data_1:
		if (data_full) {
			return;
		}
		__disable_irq();
		data_q[d_top] = 4;
		d_top++;
		d_level++;
		__enable_irq();
		break;
	case ACU_Cell_Data_2:
		if (data_full) {
			return;
		}
		__disable_irq();
		data_q[d_top] = 5;
		d_top++;
		d_level++;
		__enable_irq();
		break;
	case ACU_Cell_Data_3:
		if (data_full) {
			return;
		}
		__disable_irq();
		data_q[d_top] = 6;
		d_top++;
		d_level++;
		__enable_irq();
		break;
	case ACU_Cell_Data_4:
		if (data_full) {
			return;
		}
		__disable_irq();
		data_q[d_top] = 6;
		d_top++;
		d_level++;
		__enable_irq();
		break;
	case ACU_Cell_Data_5:
		if (data_full) {
			return;
		}
		__disable_irq();
		data_q[d_top] = 7;
		d_top++;
		d_level++;
		__enable_irq();
		break;
	case ACU_Charger_Control:
		if (charger_full) {
			return;
		}
		__disable_irq();
		charger_q[c_top] = 1;
		c_top++;
		c_level++;
		__enable_irq();
		break;
	default:
		print_lpuart("enqueue WTF\n");
		break;
	}
}

/// @brief All cell voltages added up and returns sum as float
/// @param acu
/// @return total voltage as a float
float get_total_voltage(ACU *acu) { return acu->bty->battery_total_voltage; }

/// @brief Constraints the value and casts to uint8_t
/// @param min
/// @param max
/// @param value
/// @return the uint8_t
uint8_t fconstrain(float value)
{
	if (value > 255.0)
		value = 255.0;
	else if (value < 0.0)
		value = 0.0;
	return (uint8_t)value;
}

/// @brief updates adc_data[]
/// @param acu
void update_adc_data(ACU *acu)
{
	acu->ts_current +=
	    ((adc_data[0] * 0.0005f - 1.235f) / 0.005f - acu->ts_current) *
	    0.05f; // 5mV/A
	acu->ts_voltage += (adc_data[1] * 0.0005f * 400.0f - acu->ts_voltage) *
			   0.2f; // 1:400 voltage divider
	acu->sdc_volt_w += (adc_data[2] * 0.0005f * 10.0f - acu->sdc_volt_w) *
			   0.2f; // 1:10 voltage divider
	acu->sdc_volt_v += (adc_data[3] * 0.0005f * 10.0f - acu->sdc_volt_v) *
			   0.2f; // 1:10 voltage divider
	acu->voltage_12v +=
	    (adc_data[4] * 0.0005f * 10.07475f - acu->voltage_12v) *
	    0.2f; // 1:10 voltage divider
	acu->water_sense += (adc_data[5] * 0.0005f - acu->water_sense) *
			    0.2f; // keep raw voltage
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/// @attention @OWEN PLS CHECK IF THIS IS RIGHT
float calculate_acu_soc(ACU *acu)
{
	float min_cell_open_volt =
	    acu->bty->min_cell_volt + acu->ts_current * 0.33333333f *
					  CELL_INT_RESISTANCE * NUM_TOTAL_IC *
					  NUM_CELL_IC;
	// float zero_chg_volt = NUM_TOTAL_IC * 16U * CELL_EMPTY_VOLTAGE;
	// float full_chg_volt = NUM_TOTAL_IC * 16U * CELL_FULL_VOLTAGE;
	// acu->bat_soc += (map(cell_open_volt, zero_chg_volt, full_chg_volt,
	// 0.0F, 255.0F) - acu->bat_soc) * 0.1; return acu->bat_soc;
	uint16_t i = 1;
	for (; i < CELL_CHGR_ARR_SIZE; i++) {
		if (cell_volts_tbl[i] > min_cell_open_volt)
			break;
	}
	float minCellCharge =
	    cell_charge_tbl[0] - map(min_cell_open_volt, cell_volts_tbl[i - 1],
				     cell_volts_tbl[i], cell_charge_tbl[i - 1],
				     cell_charge_tbl[i]);
	acu->bat_soc +=
	    (map(minCellCharge, 0.0f, cell_charge_tbl[0], 0.0f, 1.0f) -
	     acu->bat_soc) *
	    0.1;
	return acu->bat_soc;
}

/// @attention @OWEN PLS CHECK IF THIS IS RIGHT
float calculate_glv_soc(ACU *acu)
{
	return (acu->voltage_12v) / 12.0f; // ehhh this could also be wrong
}

/// @brief just prints adc_data
/// @param acu
void print_adc_data(ACU *acu)
{
	print_lpuart("ADC Data: -------------------------------\n");
	bzero(print_buffer, sizeof(print_buffer));
	sprintf(print_buffer,
		"ts_curr: %.3f | ts_volt: %.3f | sdc_w: %.3f | sdc_v: %.3f | "
		"12v: %.3f | water_sense: %.3f\n",
		acu->ts_current, acu->ts_voltage, acu->sdc_volt_w,
		acu->sdc_volt_v, acu->voltage_12v, acu->water_sense);
	print_lpuart(print_buffer);
}

/// @brief prints imd data
/// @param acu
void print_imd_data(ACU *acu)
{
	print_lpuart("IMD Data: -------------------------------\n");
	bzero(print_buffer, sizeof(print_buffer));
	sprintf(print_buffer,
		"R_ISO_Corrected: %hu | R_ISO_Status: %u | ISO_Meas_Count: %u\n"
		"Status_Warnings_Alarms: %hu | Status_Device_Activity: %u | "
		"HV_System: %.2f\n",
		(unsigned short int)(acu->imd->r_iso_corrected),
		(unsigned int)(acu->imd->r_iso_status),
		(unsigned int)(acu->imd->iso_meas_count),
		(unsigned short int)(acu->imd->status_warnings_alarms),
		(unsigned int)(acu->imd->status_device_activity),
		acu->imd->hv_system_voltage);
	print_lpuart(print_buffer);
	print_lpuart("-----------------------------------------\n");
}

/// @brief prints the errors and warnings of the IMD monitor
/// @param acu
void print_imd_err_warn(ACU *acu)
{
	print_lpuart("IMD Err/Warns: --------------------------\n");
	char buff[50];
	bzero(buff, sizeof(buff));
	sprintf(buff, "IMD ERR TYPE %u\n", acu->imd->status_warnings_alarms);
	print_lpuart(buff);

	if (acu->imd->status_warnings_alarms & IMD_ERROR_ACTIVE) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "⛔: IMD Error\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & HV_POS_CONN_FAIL) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "➕: HV pos connection failed\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & HV_NEG_CONN_FAIL) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "➖: HV neg connection failed\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & EARTH_CONNN_FAIL) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "🛸: Earth connection failed\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & ISO_ALARM_ERRROR) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "🚨: ISO alarm error\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & ISO_WARN_ERRRROR) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "🇺🇳: ISO warning error\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & ISO_OUTDATED_ERR) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "👵🏻: ISO outdated...\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & UN_BALANCE_ALARM) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "⚖️: Unbalanced alarm\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & UNDERVOLTG_ALARM) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "🪫: Undervoltage alarm\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & UNSAFE_TOO_START) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "☠️: Unsafe to start\n");
		print_lpuart(buff);
	}
	if (acu->imd->status_warnings_alarms & EARTH_LIFT_OPENN) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "🚠: Earth lift open\n");
		print_lpuart(buff);
	}
}

void print_relay_status(uint8_t relay)
{
	print_lpuart("Relay: ----------------------------------\n");
	char buff[100];
	sprintf(buff, "Relay (HEX): %02X\n", relay);
	print_lpuart(buff);
}

/// @brief prints acu target volts & current.
/// @param acu
void print_targets(ACU *acu)
{
	print_lpuart("Targets: --------------------------------\n");
	char buff[100];
	bzero(buff, sizeof(buff));
	sprintf(
	    buff,
	    "Target Charging Voltage: %.3f | Target Charging Current: %.3f\n",
	    acu->target_chg_voltage, acu->target_chg_current);
	print_lpuart(buff);
}

/// @brief prints charger data
/// @param acu
void print_charger_data(ACU *acu)
{
	print_lpuart("Charger Data: ---------------------------\n");
	char buff[150];
	bzero(buff, sizeof(buff));
	sprintf(buff, "Charger voltage: %hd |Charger current: %hd\n",
		acu->chgr->charger_output_voltage,
		acu->chgr->charger_output_current);
	print_lpuart(buff);
	print_lpuart("-----------------------------------------\n\n");

	print_lpuart("Charger Err/Warns: ----------------------\n");
	char bufff[50];
	if (acu->chgr->chgr_status & CHARGER_HW_FAIL) {
		bzero(bufff, sizeof(bufff));
		sprintf(bufff, "⚙️: Hardware Failure\n");
		print_lpuart(bufff);
	}
	if (acu->chgr->chgr_status & CHARGER_OV_TEMP) {
		bzero(bufff, sizeof(bufff));
		sprintf(bufff, "🥵: Over Temp\n");
		print_lpuart(bufff);
	}
	if (acu->chgr->chgr_status & CHARGER_IN_VOLT) {
		bzero(bufff, sizeof(bufff));
		sprintf(bufff, "⚡: Wrong input voltage\n");
		print_lpuart(bufff);
	}
	if (acu->chgr->chgr_status & CHARGER_CONNECT) {
		bzero(bufff, sizeof(bufff));
		sprintf(bufff, "🐻‍❄️: Wrong polarity or NC\n");
		print_lpuart(bufff);
	}
	if (acu->chgr->chgr_status & CHARGER_COOMMMM) {
		bzero(bufff, sizeof(bufff));
		sprintf(bufff, "⏱️: Timeout\n");
		print_lpuart(bufff);
	}
}

/// @brief prints acu errors & warnings
/// @param acu
void print_errors_warning(ACU *acu)
{
	print_lpuart("Err/Warns: ------------------------------\n");
	char buff[30];
	if (acu->acu_err_warns & ACU_ERR_OVER_TEMP) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "Error: ACU Overtemperature\n");
		print_lpuart(buff);
	}
	if (acu->acu_err_warns & ACU_ERR_OVER_VOLT) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "Error: ACU Overvoltage\n");
		print_lpuart(buff);
	}
	if (acu->acu_err_warns & ACU_ERR_UNDER_VOLT) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "Error: ACU Undervoltage\n");
		print_lpuart(buff);
	}
	if (acu->acu_err_warns & ACU_ERR_OVER_CURR) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "Error: ACU Overcurrent\n");
		print_lpuart(buff);
	}
	if (acu->acu_err_warns & ACU_PRECHARGE) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "Error: ACU Precharge Issue\n");
		print_lpuart(buff);
	}
	if (acu->acu_err_warns & ACU_ERR_UV_20_V) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "Error: Undervoltage for 20v\n");
		print_lpuart(buff);
	}
	if (acu->acu_err_warns & ACU_ERR_UV_12_V) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "Error: Undervoltage for 12v\n");
		print_lpuart(buff);
	}
	if (acu->acu_err_warns & ACU_ERR_UV_SDC) {
		bzero(buff, sizeof(buff));
		sprintf(buff, "Error: Undervoltage for SDC\n");
		print_lpuart(buff);
	}
	print_lpuart("-----------------------------------------\n");
}

/// @brief writes the BMS OK signal to PC8
/// @param state
void write_bms_ok(bool state)
{
	if (state) {
		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8);
	} else {
		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);
	}
}

/// @brief writes the IR- signal to PC5
/// @param state
void write_IRneg(bool state)
{
	if (state) {
		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_5);
	} else {
		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_5);
	}
}

/// @brief writes the IR+ signal to PC4
/// @param state
void write_IRpos(bool state)
{
	if (state) {
		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_4);
	} else {
		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_4);
	}
}

/// @brief writes the precharge relay signal to PB0
/// @param state
void write_prechg(bool state)
{
	if (state) {
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0);
	} else {
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_0);
	}
}

/// @brief writes the acu latch signal to PB1
/// @param state
void write_acu_latch(bool state)
{
	if (state) {
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
	} else {
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
	}
}

// deal wiht this later
void turn_of_hv_pump(bool state)
{
	// deal wiht this later
}

/// @brief writes the debug LED signal to PA15
/// @param state
void write_LED(bool state)
{
	if (state) {
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
	} else {
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);
	}
}

/// @brief resets (switch on) the ACU SDC latch (connect SDC_W to SDC_V)
void sdc_reset()
{
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
	LL_mDelay(1);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
}

/// @brief updates acu.relay_state based on relay control pins levels
/// @param acu
void update_relay_state(ACU *acu)
{
	acu->relay_state = 0;
	if (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_4)) {
		acu->relay_state |= AIR_PLUS;
	}
	if (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_5)) {
		acu->relay_state |= AIR_MINUS;
	}
	if (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_0)) {
		acu->relay_state |= RELAY_PRE;
	}
	if (!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_1)) {
		acu->relay_state |= ACU_LATCH;
	}
}