/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 *
 * @brief 	Implementation of the LED functionality
 */
 
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "led_control.h"

enum LED;

/**
 * @brief Rotates the LEDs clockwise.
 *
 * @note	Rotates the LEDs clockwise according to state.
 *				Having the state represented by cardinal directions
 *				seemed to make more sense than by clock notations.
 *
 * @return The new state of the LED.
 */
LED rotate_Clockwise(LED currentState){
	switch(currentState){
		case(NORTH):
			//13 -> 0, 14 -> 1
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
			return EAST;
		case(EAST) :
			//14 -> 0, 15 -> 1
			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
			return SOUTH;
		case(SOUTH):
			//15->0, 12->1
			GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
			return WEST;
		case(WEST):
			//12 -> 0, 13 -> 1
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
			return NORTH;
	}
}

/**
 * @brief See rotate_Clockwise
 * @return The new state of the LEDs.
 */
LED rotate_CClockwise(LED currentState){
	switch(currentState){
		case(NORTH):
			//13 -> 0, 12 -> 1
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
			return WEST;
		case(EAST) :
			//14 -> 0, 13 -> 1
			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
			return NORTH;
		case(SOUTH):
			//15->0, 14->1
			GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
			return EAST;
		case(WEST):
			//12 -> 0, 15 -> 1
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
			GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
			return SOUTH;
	}
}
