/**
 * @file
 * @author 	Aqsa Ahmed <aqsa.ahmed@mail.mcgill.ca>
 */

#include "stm32f4_discovery_lis302dl.h"
#include "motor.h"

int tRoll;
int tPitch;
int i;
void initMotorR(void);
void initMotorP(void);
/* Delayer ------------------------------------------------------------------*/
void Delay(uint32_t delay) {
  for(i=0;i<delay;i++); 
}

/**
 * @brief Initializes both motors.
 */
void initMotors(void){
	initMotorR();
	initMotorP();
}

void initMotorR(void){
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );		//Enables  the AHB1 peripheral clock.
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );		//Enables  the APB1 peripheral clock.

  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
    
 
  GPIO_StructInit(&GPIO_InitStructure); // Reset init structure
 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);		//Change mapping of pins

	// Setup All LED on STM32-Discovery Board to use PWM.
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // Alt Function - Push Pull
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOD, &GPIO_InitStructure );
 
	// Let PWM frequency equal 100Hz.
	// Let period equal 1000. Therefore, timer runs from zero to 1000. Gives 0.1Hz resolution.
	// Solving for prescaler gives 240.
	TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct );
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseInitStruct.TIM_Period = 20000 ;   
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84;
	TIM_TimeBaseInit( TIM4, &TIM_TimeBaseInitStruct );

	TIM_OCStructInit( &TIM_OCInitStruct );
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	// Initial duty cycle equals 0%. Value can range from zero to 1000.
	TIM_OCInitStruct.TIM_Pulse = 500; // 0 .. 1000 (0=Always Off, 1000=Always On)
	//Channel 1-4 for the 4 LEDs
	TIM_OC1Init( TIM4, &TIM_OCInitStruct ); 
	TIM_OC2Init( TIM4, &TIM_OCInitStruct ); 
	TIM_OC3Init( TIM4, &TIM_OCInitStruct ); 
	TIM_OC4Init( TIM4, &TIM_OCInitStruct );

	TIM_Cmd( TIM4, ENABLE );
}


void initMotorP(void){
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );		//Enables  the AHB1 peripheral clock.
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, ENABLE );		//Enables  the APB1 peripheral clock.

  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
    
 
  GPIO_StructInit(&GPIO_InitStructure); // Reset init structure
 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);		//Change mapping of pins
	// Setup All LED on STM32-Discovery Board to use PWM.
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // Alt Function - Push Pull
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
       

 
	// Let PWM frequency equal 100Hz.
	// Let period equal 1000. Therefore, timer runs from zero to 1000. Gives 0.1Hz resolution.
	// Solving for prescaler gives 240.
	TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct );
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseInitStruct.TIM_Period = 20000 ;   // 0..999
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84; // Div 240
	TIM_TimeBaseInit( TIM5, &TIM_TimeBaseInitStruct );

	TIM_OCStructInit( &TIM_OCInitStruct );
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	// Initial duty cycle equals 0%. Value can range from zero to 1000.
	TIM_OCInitStruct.TIM_Pulse = 500; // 0 .. 1000 (0=Always Off, 1000=Always On)
	//Channel 1-4 for the 4 LEDs
	TIM_OC1Init( TIM5, &TIM_OCInitStruct ); 
	TIM_OC2Init( TIM5, &TIM_OCInitStruct ); 
	TIM_OC3Init( TIM5, &TIM_OCInitStruct ); 
	TIM_OC4Init( TIM5, &TIM_OCInitStruct );

	TIM_Cmd( TIM5, ENABLE ); 
}

/**
 * @brief Moves the motors to the correct angles.
 *
 * @param angle: The angle to move the motors to.
 */
void motorMove(angle_data angle){
	tRoll  = 10*angle.roll +1500;
	tPitch = 10*angle.pitch+1500;

	TIM4->CCR1 = tRoll; 	// set brightness
	TIM5->CCR2 = tPitch; 	// set 
}
