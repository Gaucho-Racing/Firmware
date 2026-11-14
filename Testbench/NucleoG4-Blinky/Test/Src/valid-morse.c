#include <stdint.h>
#include <stdlib.h>

#include "Logomatic.h"
#include "events.h"
#include "main.h"
#include "morse.h"

#define QUEUE_SIZE 100

PinEvent events[QUEUE_SIZE];

static uint32_t delay_since_change_ms = 0;

void HAL_Delay(uint32_t Delay)
{
	delay_since_change_ms += Delay;
}

void HAL_GPIO_WritePin(uint32_t port, uint16_t pin, GPIO_PinState state)
{
	if (port != USER_LED_GPIO_Port || pin != USER_LED_Pin) {
		LOGOMATIC_CRITICAL("Error: HAL_GPIO_WritePin called with unexpected port/pin\n");
		exit(1);
	}

	PinEvent event = {.timestamp_ms = delay_since_change_ms, .pin_state = state};
	events_add_event(event);
}


void events_add_event(PinEvent event)
{
	static int index = 0;
	events[index] = event;
	index++;
	if (index >= QUEUE_SIZE) {
		printf("Error: events array overflow\n");
		exit(1);
	}
}

int main(void)
{
	const PinEvent expected_events[] = {
		{.timestamp_ms = 0U, .pin_state = GPIO_PIN_SET},
		{.timestamp_ms = morse_code_dot_delay, .pin_state = GPIO_PIN_RESET},
		{.timestamp_ms = morse_code_dot_delay * 2U, .pin_state = GPIO_PIN_SET},
		{.timestamp_ms = morse_code_dot_delay * 5U, .pin_state = GPIO_PIN_RESET},
	};

	if (morse_code_send(".- /", 4U) != HAL_OK) {
		return 1;
	}

	for (size_t i = 0; i < sizeof(expected_events) / sizeof(expected_events[0]); i++) {
		if (events[i].timestamp_ms != expected_events[i].timestamp_ms
			|| events[i].pin_state != expected_events[i].pin_state) {
			return 2;
		}
	}

	if (morse_code_send(NULL, 0U) != HAL_ERROR) {
		return 3;
	}

	if (morse_code_send("x", 1U) != HAL_ERROR) {
		return 4;
	}

	return 0;
}
