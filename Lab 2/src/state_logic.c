/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 *
 * @brief 	Implementation of the state logic functionality
 */
 
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "state_logic.h"
#include "led_control.h"

enum BUTTON;
enum SYSTEM;
enum LED;

extern float led_strength;
extern const uint16_t FREQ;


/**
 * @brief State logic for updating the system and button states.
 *
 * @note 	This implements a basic FSM for updating the button and 
 *				system states. The system can either be in read_temp mode
 *				or PWM mode. The button can either be in On, Off, or
 *				On/Off_in_run mode. The In_Run states are necessary because
 *				the button generates a string of 1's when pressed. Having
 *				these two states avoids repeated 1's triggering system
 *				state changes.
 *				When  the state is from Read_Temp, the SysTick clock is sped
 *				up for PWM, and all the LEDs are turned off. When the state is
 *				changed from PWM, the SysTick clock is reset, and the proper LED
 *				is light for the current LED state. 
 */
void Button_State(int pin, BUTTON *button_state, SYSTEM *system_state, LED *led_state){
	if(pin){ //If the pin is 1, button is down, so take these actions 
		switch(*button_state){
			case(OFF):
				//First 1 read in from an off state. Put system into pulse state
				//Put button into ON_IN_RUN state, as more 1's will be read.
				*button_state = ON_IN_RUN;
				*system_state = PULSE;
				//Reconfigure systick to handle PWM. 20Hz wasn't fast enough
				SysTick_Config(SystemCoreClock / (FREQ*50));
				//Turn all LED's off to start.
				GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
			break;
			case(ON):
				//First 1 read in from an on state, so put system into temp_read state.
				//Put button into OFF_IN_RUN state as more 1's will be read
				*button_state = OFF_IN_RUN;
				*system_state = TEMP_READ;
				//Reset systick
				SysTick_Config(SystemCoreClock / FREQ);
				//Turn off all LEDs
				GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				//Turn on the LED that was on when last in the temp_read state
				GPIO_ToggleBits(GPIOD, *led_state);
				break;
			default:
				break;
			}
		}
	else{
		switch(*button_state){
			//Reading in 0 now, so buttons are no longer in runs
			case(ON_IN_RUN):
				*button_state = ON;
				break;
			case(OFF_IN_RUN):
				*button_state = OFF;
				break;
			default:
				break;
			}
		}
}
