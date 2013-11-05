#include "led_control.h"

#ifndef LOGIC_CONTROL
#define LOGIC_CONTROL
enum LED;
typedef enum{
	ON_IN_RUN = 0,
	ON = 1,
	OFF_IN_RUN = 2,
	OFF = 3
}BUTTON;

typedef enum{
	TEMP_READ = 0,
	PULSE = 1
}SYSTEM;

void Button_State(int push, BUTTON *button_state, SYSTEM *system_state, LED *led_state);
#endif
