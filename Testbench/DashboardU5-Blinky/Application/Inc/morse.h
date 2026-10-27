#include <stdint.h>

#include "main.h"

#ifndef MORSE_H
#define MORSE_H

#define morse_code_dot_delay (100U)
#define morse_code_intra_element_delay (morse_code_dot_delay)
#define morse_code_dash_delay (morse_code_dot_delay * 3U)
#define morse_code_letter_space_delay (morse_code_dot_delay * 3U)
#define morse_code_word_space_delay (morse_code_dot_delay * 7U)

HAL_StatusTypeDef morse_code_send(const char *message, uint8_t length);

#endif
