/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 */
 
 
#ifndef LED_CONTROL
#define LED_CONTROL

typedef enum{
	NORTH = GPIO_Pin_13,		/*!< Specifies the GPIO pins to be configured.*/
	EAST = GPIO_Pin_14,			/*!< Specifies the GPIO pins to be configured.*/
	SOUTH = GPIO_Pin_15,		/*!< Specifies the GPIO pins to be configured.*/
	WEST = GPIO_Pin_12			/*!< Specifies the GPIO pins to be configured.*/
}LED;
LED rotate_Clockwise(LED currentState);

LED rotate_CClockwise(LED currentState);
float pwd(float on_time);
#endif
