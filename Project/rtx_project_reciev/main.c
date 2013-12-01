#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "CC2500.h"
#include "KEYPAD.h"
#include "ACCEL.h"
#include "cmsis_os.h"
#include "TIMERS.h"

#define GARBAGE -333

#define RECIEV

#ifdef RECIEV
//Reciever threads and mutexes.
#include "MOTOR.h"
#include "LCD.h"
#endif


void DATA_thread(void const *args);
osThreadDef(DATA_thread, osPriorityHigh, 1, 0);
osThreadId DATA_tid;

#ifndef RECIEV
//TX threads and mutexes.
osMutexId SPI_mutex;
osMutexId STATE_mutex;
osMutexDef(SPI_mutex);
osMutexDef(STATE_mutex);

osThreadId BUTTON_tid;

void BUTTON_thread(void const *args);

osThreadDef(BUTTON_thread, osPriorityNormal, 1, 0);

uint8_t state = 0; //0=board, 1=buttonImmediate, 2= buttonQueue
angle_data button_data;
angle_data button_queue[10];
uint8_t queue_elements;
#endif


/*!
 @brief Program entry point
 */
int main (void) {
	osDelay(20);
	CC2500_Init();
	INTTIM_Config();
	
#ifndef RECIEV
	SPI_mutex = osMutexCreate(osMutex(SPI_mutex));
	STATE_mutex = osMutexCreate(osMutex(STATE_mutex));
	KEYPAD_Init();
	ACCEL_Config();
	
	BUTTON_tid = osThreadCreate(osThread(BUTTON_thread), NULL);
#endif
#ifdef RECIEV
	initMotors();
#endif
	DATA_tid = osThreadCreate(osThread(DATA_thread), NULL);
	osDelay(osWaitForever);
}

void DATA_thread(void const *args){
	angle_data new_vals;
#ifndef RECIEV
	uint8_t button_pressed;
	int i = 0;
	while(1){
		osSignalWait(0x1, osWaitForever);
		new_vals = read_Accelerometer();
		button_pressed = KEYPAD_Check();
		if(button_pressed != 99){
			osSignalSet(BUTTON_tid, 0x1);
		}
		osMutexWait(STATE_mutex, osWaitForever);
		switch(state){
			case 0:
				CC2500_TXData(new_vals);
				break;
			case 1:
				//Transmit single point from a place
				if(button_data.pitch > GARBAGE){
					CC2500_TXData(button_data);
					button_data.pitch = GARBAGE;
				}
				break;
			case 2:
				if(button_data.pitch == -666){
					while(queue_elements>0){
						CC2500_TXData(button_queue[i]);
						i++;
						queue_elements--;
						osDelay(1000);
					}
					i = 0;
				}
				break;
		}
		osMutexRelease(STATE_mutex);
	}
#endif
	
#ifdef RECIEV
	while(1){
		osSignalWait(0x1, osWaitForever);
		new_vals = CC2500_RXData();
		if(new_vals.pitch > GARBAGE){
			LCD_write_angle(new_vals);
			motorMove(new_vals);
		}
	}
#endif
}

void BUTTON_thread(void const *args){
#ifndef RECIEV
	uint8_t button_pressed;
	while(1){
		osSignalWait(0x1, osWaitForever);
		button_pressed = KEYPAD_Check();
		osMutexWait(STATE_mutex, osWaitForever);
		if(button_pressed == 14){
					state = (state + 1) % 3;
		}
		else if (button_pressed != 99){
			switch(state){
				case 0:
					break;
				case 1:
					button_data = KEYPAD_buttonToAngle(button_pressed);
					break;
				case 2:
					button_data = KEYPAD_buttonToAngle(button_pressed);
					if(button_data.pitch > GARBAGE){
						if(queue_elements < 10){
							button_queue[queue_elements] = button_data;
							queue_elements++;
						}
					}
					break;
			}
		}
		osMutexRelease(STATE_mutex);
		osDelay(250);
		osSignalClear(BUTTON_tid, 0x1);
	}
#endif
}
