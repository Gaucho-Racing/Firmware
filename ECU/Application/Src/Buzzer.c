#include "Buzzer.h"

#include <stdint.h>

#include "Plan_R.h"
#include "StateUtils.h"
#include "main.h"

#ifdef PLAN_R
#define BUZZER_SHORT 40
#define BUZZER_LONG 180
#define BUZZER_GAP 60
#define BUZZER_ERROR 300
#else
#define BUZZER_SHORT 0
#define BUZZER_LONG 0
#define BUZZER_GAP 0
#define BUZZER_ERROR 0
#endif

#define BUZZER_RTD 2000

typedef enum {
	PLAY_IDLE,
	PLAY_START_PULSE,
	PLAY_PULSE_ON,
	PLAY_PULSE_GAP
} PlaybackState;

static PlaybackState play_state = PLAY_IDLE;
static uint32_t state_timer = 0;
static uint8_t total_pulses = 0;
static uint8_t current_pulse = 0;
static uint32_t pulse_duration = 0;

void buzzer_rtd(void)
{
	total_pulses = 1;
	current_pulse = 0;
	pulse_duration = BUZZER_RTD;
	play_state = PLAY_START_PULSE;
}

void buzzer_error_both(void)
{
	total_pulses = 2;
	current_pulse = 0;
	pulse_duration = BUZZER_ERROR;
	play_state = PLAY_START_PULSE;
}

void buzzer_error_min(void)
{
	total_pulses = 1;
	current_pulse = 0;
	pulse_duration = BUZZER_ERROR;
	play_state = PLAY_START_PULSE;
}

void buzzer_error_max(void)
{
	total_pulses = 1;
	current_pulse = 0;
	pulse_duration = BUZZER_ERROR;
	play_state = PLAY_START_PULSE;
}

void buzzer_downward(void)
{
	total_pulses = 1;
	current_pulse = 0;
	pulse_duration = BUZZER_LONG;
	play_state = PLAY_START_PULSE;
}

void buzzer_upward(void)
{
	total_pulses = 1;
	current_pulse = 0;
	pulse_duration = BUZZER_SHORT;
	play_state = PLAY_START_PULSE;
}

void buzzer_minpoint(void)
{
	total_pulses = 2;
	current_pulse = 0;
	pulse_duration = BUZZER_LONG;
	play_state = PLAY_START_PULSE;
}

void buzzer_midpoint(void)
{
	total_pulses = 2;
	current_pulse = 0;
	pulse_duration = BUZZER_SHORT;
	play_state = PLAY_START_PULSE;
}

void buzzer_maxpoint(void)
{
	total_pulses = 3;
	current_pulse = 0;
	pulse_duration = BUZZER_SHORT;
	play_state = PLAY_START_PULSE;
}

void buzzer_tick(void)
{
	switch (play_state) {
		case PLAY_IDLE:
			LL_GPIO_ResetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
			break;

		case PLAY_START_PULSE:
			if (pulse_duration == 0 || total_pulses == 0 || current_pulse >= total_pulses) {
				play_state = PLAY_IDLE;
				LL_GPIO_ResetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
				break;
			}
			LL_GPIO_SetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
			state_timer = MillisecondsSinceBoot();
			play_state = PLAY_PULSE_ON;
			break;

		case PLAY_PULSE_ON:
			if (MillisecondsSinceBoot() - state_timer >= pulse_duration) {
				LL_GPIO_ResetOutputPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin);
				current_pulse++;

				if (current_pulse < total_pulses) {
					state_timer = MillisecondsSinceBoot();
					play_state = PLAY_PULSE_GAP;
				} else {
					play_state = PLAY_IDLE;
				}
			}
			break;

		case PLAY_PULSE_GAP:
#if defined(PLAN_R) && (BUZZER_GAP > 0)
			if (MillisecondsSinceBoot() - state_timer >= BUZZER_GAP) {
				play_state = PLAY_START_PULSE;
			}
#else
			play_state = PLAY_IDLE;
#endif
			break;
	}
}
