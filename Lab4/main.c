#include "arm_math.h"
#include "stm32f4xx.h"
#ifndef OS_PROTECT
#define OS_PROTECT
#include "cmsis_os.h"
#endif

#include "timers.h"
#include "gpio_config.h"
#include "adc_config.h"
#include "accelerometer.h"
#include "led_control.h"
#include "state_logic.h"
 
/* TO DO: COMMENT ANY ADDITIONS THAT YOU MAKE.
 * -Gather data
 *	=Initialize clocks													(DONE)
 *		+TIM2 @ 25Hz															(DONE)
 *		+TIM5 @ 20Hz															(DONE)
 *	=Configure interrupts												(DONE)
 *		+Has to do with OS stuff									(DONE)
 *		+Signals																	(DONE)
 *	=Initialize and configure sensors						(DONE)
 *	=Configure threads													(DONE)
 *		+Thread 1																	(DONE)
 *			-Accelerometer data collection					(DONE)
 *		+Thread 2																	(DONE)
 *			-Temperature data collection						(DONE)
 *	=Collect and process temp data							(DONE)
 *		+Configure and initialize temp sensor			(DONE)
 *		+Convert temp sensor output								(DONE)
 *		+Filter data															(DONE)
 *	=Collect and process accel data							(DONE)
 *		+Configure and initialize accelerometer		(DONE)
 *		+Convert accelerometer output							(DONE)
 *		+Filter data															(DONE)
 * -LED states from LAB2												(DONE)
 * -State Logic																	(DONE)						
 *	=Find shared resources											(DONE)
 *	=Find critical sections											(DONE)
 *	=Implement protection												(DONE)
 * -Temperature Mode														(DONE)
 * -Blink LED Mode															(DONE)
 * -PWM Mode																		(DONE)
 * -Accelerometer display Mode									(DONE)
 * -State change actions												(DONE)
 *	=Insure that all variables are at initial		(DONE)
 *	values.																			(DONE)
 *		+Button																		(DONE)
 *		+Tap																			(DONE)
 * -Tap to change mode													(DONE)
 * -Button press to change submode							(DONE)
 *	=Read value																	(DONE)
 *	=Toggle state																(DONE) 
 * 	=Debounce it																(DONE)
 * -Ensure that button press isn't tapping
 */

osMutexId state_mutex;
osMutexDef(state_mutex);

osThreadId accel_tid;
osThreadId temp_tid;
osThreadId button_tid;
osThreadId tap_tid;

void accel_thread(void const *args);
void temp_thread(void const *args);
void button_thread(void const *args);
void tap_thread(void const *args);

osThreadDef(temp_thread, osPriorityNormal, 1, 0);
osThreadDef(accel_thread, osPriorityNormal, 1, 0);
osThreadDef(button_thread, osPriorityNormal, 1, 0);
osThreadDef(tap_thread, osPriorityNormal, 1, 0);

LED led_state = NORTH;
MINOR_STATE sub_state = SENSOR_DISPLAY;
MAJOR_STATE over_state = !ACCEL_READ;
/*!
 @brief Program entry point
 */
int main (void) {
	
	GPIO_Configuration();
	ADC_Configuration();
	ACCEL_Config();
	INTTIM_Config();
	
	state_mutex = osMutexCreate(osMutex(state_mutex));
	
	accel_tid = osThreadCreate(osThread(accel_thread), NULL);
	temp_tid = osThreadCreate(osThread(temp_thread), NULL);
	button_tid = osThreadCreate(osThread(button_thread), NULL);
	tap_tid = osThreadCreate(osThread(tap_thread), NULL);
	osDelay(osWaitForever);
}

/**
 * @brief Acceleration thread.
 *
 * @note	The acceleration data collection thread. This thread handles
 *				all data collection and processing for the accelerometer. It
 *				also updates the LEDs if the system is in ACCEL_READ mode.
 *				All equations and values are taken from Lab 2.
 */
void accel_thread(void const *args){
	angle_data new_vals;
	int cycle_off = 1500;
	int direction = 0;
	int i = 0;
	while(1){
		osSignalWait(0x1, osWaitForever);
		new_vals = read_Accelerometer();
		osMutexWait(state_mutex, osWaitForever);
		if(over_state == ACCEL_READ){
			if(sub_state == LED_CHANGE){
				i++;
				if(i>0){
					i = 0;

          TIM4->CCR1 = pow((0.43033 * cycle_off),2);
          TIM4->CCR2 = pow((0.43033 * cycle_off),2);
          TIM4->CCR3 = pow((0.43033 * cycle_off),2);
          TIM4->CCR4 = pow((0.43033 * cycle_off),2);
					cycle_off = (direction) ? cycle_off + 3 : cycle_off - 3;
					if(cycle_off <= 0 || cycle_off >= 90){
						direction = !direction;
						cycle_off = (cycle_off <=0) ? 1 : 90;
					}
				}
				osMutexRelease(state_mutex);
			}
			else{
				//.2867*90 = 665.98 This means that CCRs vary between 0 and ~666, that is,
				//always on and always off.
				//Pitch and roll vary between -90 and 90. As pitch approaches 90 CCRS 
				//should increase and CCRN should stay at 0. As pitch approaches -90
				//CCRN should increase.
				//As roll -> 90, CCRE should increase. Roll -> -90, CCRW increases.
				TIM4->CCR4 = (new_vals.pitch) > 0 ? pow((0.43033 * new_vals.pitch),2) : 0; 	//South
				TIM4->CCR2 = (new_vals.pitch) < 0 ? pow((0.43033 * new_vals.pitch),2) : 0; 	//North
				TIM4->CCR3 = (new_vals.roll) > 0 ? pow((0.43033 * new_vals.roll),2) : 0;		//East
				TIM4->CCR1 = (new_vals.roll) < 0 ? pow((0.43033 * new_vals.roll),2) : 0;		//West
				osMutexRelease(state_mutex);
				
			}
		}
		else{
			osMutexRelease(state_mutex);
		}
	}
}

/**
 * @brief Temperature thread.
 *
 * @note	The temperature data collection thread. This thread handles
 *				all data collection and processing for the temperature sensor. 
 *				It also updates the LEDs if the system is in TEMP_READ mode.
 *				All equations and values are taken from Lab 1.
 */
void temp_thread(void const *args){
	 float temp = 0; 
   float delta_temp = 0; 
   float prev_temp = 0; 
   int flash_counter=0; 
	while(1){
		osSignalWait(0x1, osWaitForever);
 		temp = read_Temp();
		osMutexWait(state_mutex, osWaitForever);
		if(over_state == TEMP_READ){
			if(sub_state == LED_CHANGE){
				flash_counter++; 
        if(flash_counter == 5){ 
					flash_counter = 0; 
					GPIO_ToggleBits(GPIOD, NORTH | SOUTH | EAST | WEST); 
					osMutexRelease(state_mutex);
        } 
				else{
					osMutexRelease(state_mutex);
				}
			}
			else{
        delta_temp = temp - prev_temp; 
        if(delta_temp > 2 && prev_temp != -100){ 
             led_state = rotate_Clockwise(led_state);             
             prev_temp = temp; 
        } 
        else if(delta_temp < -2 && prev_temp != -100){ 
            led_state = rotate_CClockwise(led_state);                
            prev_temp = temp; 
        }
				osMutexRelease(state_mutex);
			}
		}
		else{
			osMutexRelease(state_mutex);
		}
	}
}

/**
 * @brief The button thread.
 *
 * @note 	This thread handles updating the substate. It checks the current
 *				state of the button and updates accordingly. Debouncing is done
 *				through a timer.
 */
void button_thread(void const *args){
	int push = 0;
	while(1){
		osSignalWait(0x1, osWaitForever);
		push = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
		if(push == 1){
			//Change substate
			//If it's in temp, make sure LEDs are all in the same state
			osMutexWait(state_mutex, osWaitForever);
			sub_state = !sub_state;
			if(over_state == TEMP_READ){
				if(sub_state != LED_CHANGE){
					GPIO_WriteBit(GPIOD, NORTH|SOUTH|EAST|WEST, 0);
					GPIO_ToggleBits(GPIOD, led_state);
					osMutexRelease(state_mutex);
				}
				else{
					GPIO_WriteBit(GPIOD, NORTH|SOUTH|EAST|WEST, 0);
					osMutexRelease(state_mutex);
				}
				
			}
			else{
				osMutexRelease(state_mutex);
			}
			osDelay(250);
		}
	}
}

/**
 * @brief The tap thread.
 *
 * @note 	The tap thread. This thread handles switching between major modes 
 *				when triggered by a tap. It handles debouncing by a thread wait,
 *				and then clearing the external interrupt line.
 */
void tap_thread(void const *args){
	while(1){
		osSignalWait(0x1, osWaitForever);
		//Change major state
		//Change LED modes here
		osMutexWait(state_mutex, osWaitForever);
		over_state = !over_state;
		if(over_state == TEMP_READ){
			LED_BlinkConfig();
			if(sub_state !=LED_CHANGE){
				GPIO_ToggleBits(GPIOD, led_state);
			}
			osMutexRelease(state_mutex);
		}
		else{
			LED_AFConfig();
			osMutexRelease(state_mutex);
		}
		osDelay(250);
		osSignalClear (tap_tid, 0x1);
	}
}
