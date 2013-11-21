#include "led_control.h"

#ifndef LOGIC_CONTROL
#define LOGIC_CONTROL
typedef enum{
	SENSOR_DISPLAY = 0,
	LED_CHANGE = 1
}MINOR_STATE;

typedef enum{
	TEMP_READ = 0,
	ACCEL_READ = 1
}MAJOR_STATE;

#endif
