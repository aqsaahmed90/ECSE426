/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 */
 
 
#include "timers.h"
#include "stm32f4_discovery_lis302dl.h"

extern uint_fast16_t tick;
void PWM_config(void);

/**
 * @brief Configures and initializes TIM2 and the TIM2 interrupt in the NVIC.
 *
 * @note	Configures the NVIC TIM2 entry to have priority (0,1). Then enables
 *				TIM2 at 25Hz. TIM4 is then enabled at 1.5KHz for PWM. No interrupt
 *				is set for TIM4.
 */
void INTTIM_Config(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//Configuring the NVIC
  //Will be using TIM2 as the timer, so set TIM2_IRQn
	//as the channel
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	//Give it the highest priority possible as no other
	//interrupts will be running at this point (will need to
	//update in the future)
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//Set the subpriority level so that it can be preempted by
	//a 0,0 interrupt
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	//Enable the channel
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//Initiate.
  NVIC_Init(&NVIC_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/*
		We want TIM2 counter clock at 1MHz
		Prescaler = (TIM2CLK / TIM2 counter clock) - 1
		TIM2CLK = 2 * APB1 clock
		APB1 clock = SystemCoreClock / 4
		TIM2CLK = (SystemCoreClock/2)/1000000 - 1
	*/
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) ((SystemCoreClock /2) /1000000) - 1;
  /*
		We want TIM2 output clock at 25Hz. The period is:
		period 	= (TIM2 counter clock/ TIM2 output clock) - 1;
						= (1000000Hz/25Hz) - 1
	*/
	TIM_TimeBaseStructure.TIM_Period = (1000000/25) - 1;
	//No clock division will occur
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//Clock counts up rather than down. Doesn't seem to have any affect on operation
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//Configure the interrupt and enable
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
	
	//TIM4 is at 1.5KHz
	TIM_TimeBaseStructure.TIM_Period = (1000000/1500) - 1;
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	PWM_config();
}
/**
 * @brief Sets up paramters for hardware PWM.
 *
 * @note	Sets the proper settings for OC. Puts the mode
 *				into PWM1, enables the output state, and sets it to active high.
 *				Then it initializes all four channels, and enables TIM4.
 */
void PWM_config(void){
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	//Will be doing PWM, so put it into PWM1 mode. 
	//Can't find any data on PWM2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//Enables the output state.
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//1 and 0 both work, so I figure leaving it at default is fine.
	//Still not entirely sure what it does.
	//TIM_OCInitStructure.TIM_Pulse = 1;
	//Sets active high
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//Enable all four output compares with our settings
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	//Enables or disables the TIMx peripheral Preload register on CCR1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	//Enables ARR preload register.
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

/**
 * @brief The TIM2 interrupt handler.
 *
 * @note	When TIM2 is high, the interrupt handler flips the tick bit and resets
 *				the TIM2 flag.
 */
void TIM2_IRQHandler(void){
	//TIM_GetFlagStatus returns status flag, where 0 == Reset, Set == !Reset == 1
	if(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update)){
		tick = 1;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
  
}
