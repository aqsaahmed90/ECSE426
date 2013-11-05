/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 */
 
 
#include "gpio.h"
#include "stm32f4xx.h"

/**
 * @brief Configures and initializes all GPIO objects.
 *
 * @note 	Enables the clocks to GPIOD and GPIOA. Then
 *				configures the structure representing the LEDs
 *				by assigning it the values of the LED pins, setting
 *				the Mode to output, setting it to push pull,
 *				configuring the speed, and setting it to have no
 *				push/pull resistor. Open Drain does not activate the
 *				LEDs. Copy and pasted from Lab 2, with minor modifications.
 */
 
void GPIO_Config(void){
	
	//Copy and pasted from Lab2. Just initializes the LEDs and turns them on.
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	//Set up everything for LEDs
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	//They're in AF mode
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//PushPull output type
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//Set speed
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//Set pull up/pull down/don't pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
	
}
