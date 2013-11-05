/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 *
 * @brief 	Implementation of the GPIO functionality
 */
 
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

/**
 * @brief Configures and initializes all GPIO objects.
 *
 * @note 	Enables the clocks to GPIOD and GPIOA. Then
 *				configures the structure representing the LEDs
 *				by assigning it the values of the LED pins, setting
 *				the Mode to output, setting it to push pull,
 *				configuring the speed, and setting it to have no
 *				push/pull resistor. Open Drain does not activate the
 *				LEDs.
 *				Then configures and initializes the structure
 *				representing the Button. It is set to the correct pin,
 *				input mode, pull down, and the speed remains at default.
 *
 * @param *GPIO_InitStructure A pointer to a GPIO_InitTypeDef
 *				structure that represents the LEDs.
 * @param *BUTTON_InitStructure A pointer to a GPIO_InitTypeDef
					structure that represents the button.
 */
void GPIO_Configuration(GPIO_InitTypeDef *GPIO_InitStructure, GPIO_InitTypeDef  *BUTTON_InitStructure){
	//Enable clocks to GPIOA and D (LEDs and Button)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//Set up everything for LEDs
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	//They're outputs
  GPIO_InitStructure->GPIO_Mode = GPIO_Mode_OUT;
	//PushPull output type
  GPIO_InitStructure->GPIO_OType = GPIO_OType_PP;
	//Set speed
  GPIO_InitStructure->GPIO_Speed = GPIO_Speed_100MHz;
	//Set pull up/pull down/don't pull
  GPIO_InitStructure->GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, GPIO_InitStructure);
	
	GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
	//See above
	BUTTON_InitStructure->GPIO_Pin = GPIO_Pin_0;
	BUTTON_InitStructure->GPIO_Mode = GPIO_Mode_IN;
	//Set to pull down so that non-active button is 0 V.
	BUTTON_InitStructure->GPIO_PuPd = GPIO_PuPd_DOWN;
	//BUTTON_InitStructure->GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, BUTTON_InitStructure);
}
