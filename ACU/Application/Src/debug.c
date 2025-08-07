#include "debug.h"
#include "acu.h"
#include "state.h"

extern ACU acu;
extern State state;

extern void print_imd_data(ACU *acu);
extern void print_targets(ACU *acu);
extern void print_adc_data(ACU *acu);
extern void print_lpuart(char *arr);
extern void print_voltage(Battery *bty);
extern void print_imd_err_warn(ACU *acu);
extern void print_charger_data(ACU *acu);
extern void print_temperature(Battery *bty);
extern void print_errors_warning(ACU *acu);

extern char print_buffer[1000];

/// @brief Print BCC Status messages
/// @param stat bcc_status_t
void print_bcc_status(bcc_status_t stat)
{
	switch (stat) {
	case BCC_STATUS_SUCCESS:
		print_lpuart("Success Status\n");
		break;
	case BCC_STATUS_PARAM_RANGE:
		print_lpuart("Parameter out of range\n");
		break;
	case BCC_STATUS_SPI_FAIL:
		print_lpuart("SPI failed\n");
		break;
	case BCC_STATUS_COM_TIMEOUT:
		print_lpuart("communication timeout\n");
		break;
	case BCC_STATUS_COM_ECHO:
		print_lpuart("Echo frame doesn't correspond to sent frame\n");
		break;
	case BCC_STATUS_COM_CRC:
		print_lpuart("CRC error\n");
		break;
	case BCC_STATUS_COM_MSG_CNT:
		print_lpuart("Message counter mismatch\n");
		break;
	case BCC_STATUS_COM_NULL:
		print_lpuart("NULL message\n");
		break;
	case BCC_STATUS_DIAG_FAIL:
		print_lpuart("Diagnoctic mode not allowed\n");
		break;
	case BCC_STATUS_EEPROM_ERROR:
		print_lpuart("EEPROM communication error\n");
		break;
	case BCC_STATUS_EEPROM_PRESENT:
		print_lpuart("EEPROM device not detected\n");
		break;
	case BCC_STATUS_DATA_RDY:
		print_lpuart("New convertion already running\n");
		break;
	case BCC_STATUS_TIMEOUT_START:
		print_lpuart("BCC_MCU_StartTimeout function error\n");
		break;
	default:
		print_lpuart("Unknown status\n");
		break;
	}
}

void debug()
{
	print_lpuart("\n\n\n\n\n\n\n\n\n\n\n------------------- 💥 Debug Start "
		     "💥 -------------------\n");
	sprintf(print_buffer, "Timestamp: %lums | ", TIM5->CNT / 1000);
	print_lpuart(print_buffer);
	print_state();

	print_relay_status(acu.relay_state);

#if DUMP_TARGETS == 1
	print_targets(acu);
#endif

#if DUMP_VOLTS == 1
	print_voltage(acu.bty);
#endif

#if DUMP_TEMPS == 1
	print_temperature(acu.bty);
#endif

#if DUMP_ADC_DATA == 1
	print_adc_data(&acu);
#endif

#if DUMP_ERR_WARN == 1
	print_errors_warning(&acu);
#endif

#if CHARG_CTL == 1
	char buff[40];
	bzero(buff, sizeof(buff));
	if (acu.chg_ctrl & PLS_CHARGE) {
		sprintf(buff, "Charge Control: pls charge\n");
	} else {
		sprintf(buff, "Charge Control: pls no charge\n");
	}

	print_lpuart(buff);
#endif

#if DUMP_IMD_DATA == 1
	print_imd_err_warn(&acu);
	print_imd_data(&acu);

#endif

#if DUMP_CHARGER_DATA == 1
	print_charger_data(&acu);

#endif
	print_lpuart(
	    "-------------------- 💣 End Debug 💣 --------------------\n");
}
