/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 *
 * @brief 	Implementation of the functionality required for Lab 2.
 *
 * @note 		This project reads in values from the temperature sensor, converts
 * 					them with an ADC, and rotates an LED accordingly. It also has the
 * 					ability to pulse the LED's using a software implementation of pulse
 * 					width modulation.
 */

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "adc_config.h"
#include "filter_config.h"
#include "gpio_config.h"
#include "led_control.h"
#include "state_logic.h"

//Will read at 20 Hz, and filter D is 5.
const uint16_t FREQ = 20;
const uint16_t D = 6;
struct Lowpass_Filter;

//Set everything up
Lowpass_Filter Filter_Structure;
GPIO_InitTypeDef  GPIO_InitStructure;
GPIO_InitTypeDef  BUTTON_InitStructure;
ADC_CommonInitTypeDef ADC_Common_InitStructure;
ADC_InitTypeDef		ADC_InitStructure;

void SysTick_Handler (void);
void delay (float duty_cycle);

//Gets changed by sys_tick handler
static volatile uint_fast16_t ticks;



/**
 * @brief	Initializes all necessary structures and loops forever.
 *
 * @note 	This method calls the appropriate methods to initialize
 * 				the necessary structures, initializes and sets any
 * 				variables, and then runs an infinite loop based off of
 * 				the SysTick timer. Inside this loop, appropriate behavior
 * 				is taken based off of the mode of the system.
 */
int main()
{
	//Initialize everything
	ADC_Configuration(&ADC_Common_InitStructure, &ADC_InitStructure);
	Filter_Configuration(&Filter_Structure);
	GPIO_Configuration(&GPIO_InitStructure, &BUTTON_InitStructure);
	
	//Variables for temp, previous temp, difference in temps,
	//tick_count, led_strength, and Led direction (make brighter or dimmer in pulse)
	float temp = 0;
	float delta_temp = 0;
	int tick_count = 0;
	float led_strength = 0.001; //0.001 to 1
	int direction = 1; //1=increasing
	uint16_t push;
	ticks = 0;
	//Initial LED state is NORTH
	LED led_state = NORTH;
	//Initial Button state is OFF (TEMP_READ)
	BUTTON button_state = OFF;
	//Initial system_state is TEMP_READ
	SYSTEM system_state = TEMP_READ;
	//Set up SYStick
	SysTick_Config(SystemCoreClock / FREQ);
	float prev_temp = -100;
	//Loop forever
	while(1){
		//Wait until tick is flagged
		while(!ticks);
		//Get pin state of Button
		push = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
		//Check state update
		Button_State(push, &button_state, &system_state, &led_state);
		if(system_state == TEMP_READ){
			//Read in temp and do necessary filtering
			temp = read_Temp();
			//printf("%f; ", temp);
			temp = filter_Value(&Filter_Structure, temp);
			delta_temp = temp - prev_temp;
// 			//If 2 deg difference, rotate the LEDs
 			//printf("Output: %f\n", temp);
			if(delta_temp > 2 && prev_temp != -100){
				led_state = rotate_Clockwise(led_state);
				//printf("Output: %f\n, Prev_temp: %f", temp, prev_temp);
				prev_temp = temp;
			}
			else if(delta_temp < -2 && prev_temp != -100){
				led_state = rotate_CClockwise(led_state);
				//printf("Output: %f\n, Prev_temp: %f", temp, prev_temp);
				prev_temp = temp;
			}
			if(prev_temp == -100 && temp != -100){
				prev_temp = temp;
			}
			
		}
		
		
		else if (system_state == PULSE){
			//Turn LEDs on
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
			//Delay for a percent of the duty cycle
			delay(led_strength);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
			//Turn off and delay for the rest of the duty cycle
			delay(1-led_strength);
			if(tick_count > 10){
				//Brighten/Dim LEDs if necessary.
				led_strength = (direction) ? led_strength*3 : led_strength/3;
				if(led_strength >= 1 || led_strength <=0.0001){
					direction = !direction;
				}
				tick_count = 0;
			}
			tick_count++;
		}
	}
}

/**
 * @brief	Handles the SysTick interupt.
 *
 * @note	On interupt, this method sets the 'ticks' flag to 1, indicating
 * 				that an interupt has occured.
 */
void SysTick_Handler (void)
{
	ticks = 1;
}

/**
 * @brief	Delays for a fraction of the complete cycle.
 *
 * @note	This method takes in a float representing the fraction of time
 * 				that the LED should be on. This is used in PWM to delay throughout
 * 				the duty cycle.
 *
 * @param	duty_cycle Fraction of the cycle that the LEDs should be on
 */
void delay (float duty_cycle){
	//SystemCoreClock / (FREQ*50) = Number of ticks between two interrupts.
	int cycles = SystemCoreClock / (FREQ*50) * duty_cycle;
	while(cycles > 0){
		while(!ticks);
		cycles--;
	}
}
