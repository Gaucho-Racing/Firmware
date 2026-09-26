#include "morse.h"

#include <stdint.h>

#include "main.h"

HAL_StatusTypeDef morse_code_send(const char *message, uint8_t length)
{
	if (message == NULL || length == 0) {
		return HAL_ERROR;
	}

	for (uint8_t i = 0; i < length; i++) {
		if (message[i] == '\0') {
			break;
		}

		switch (message[i]) {
			case '.':
				HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
				HAL_Delay(morse_code_dot_delay);
				HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
				HAL_Delay(morse_code_intra_element_delay);
				break;

			case '-':
				HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
				HAL_Delay(morse_code_dash_delay);
				HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
				HAL_Delay(morse_code_intra_element_delay);
				break;

			case ' ':
				HAL_Delay(morse_code_letter_space_delay);
				break;

			case '/':
				HAL_Delay(morse_code_word_space_delay);
				break;

			default:
				return HAL_ERROR;
		}
	}
	return HAL_OK;
}
