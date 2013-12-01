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

/**
 * @brief Initializes both motors.
 */
void initMotors(void){
	initMotorR();
	initMotorP();
}


/**
 * @brief Intialize Motor for Roll.
 * 
 * @note 	GPIO-D Pin 12 was configured to provide PWM signal
 * 				to the roll Motor using TIM4 in PWM mode.
 *
 * @param  void
 * @return void
 */
void initMotorR(void){
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );		//Enables  the AHB1 peripheral clock.
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );		//Enables  the APB1 peripheral clock.

  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
    
 
  GPIO_StructInit(&GPIO_InitStructure); // Reset init structure
 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);		// Change mapping of pins

	// Setup All LED on STM32-Discovery Board to use PWM.
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;									// Pin12 to use TIM4 CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            		// Alterbalte Function 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// Push Pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOD, &GPIO_InitStructure );
 
	TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct );
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseInitStruct.TIM_Period = 20000 ;   							//Set to match motor polling frequency
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84;
	TIM_TimeBaseInit( TIM4, &TIM_TimeBaseInitStruct );

	TIM_OCStructInit( &TIM_OCInitStruct );
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_Pulse = 500;
	//Channel 1-4
	TIM_OC1Init( TIM4, &TIM_OCInitStruct ); 
	TIM_OC2Init( TIM4, &TIM_OCInitStruct ); 
	TIM_OC3Init( TIM4, &TIM_OCInitStruct ); 
	TIM_OC4Init( TIM4, &TIM_OCInitStruct );

	TIM_Cmd( TIM4, ENABLE );
}

/**
 * @brief Intialize Motor for Pitch.
 * 
 * @note 	GPIO-A Pin 1 was configured to provide PWM signal
 * 				to the pitch Motor using TIM4 in PWM mode.
 *
 * @param  void
 * @return void
 */
void initMotorP(void){
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );		//Enables  the AHB1 peripheral clock.
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, ENABLE );		//Enables  the APB1 peripheral clock.

  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
    
 
  GPIO_StructInit(&GPIO_InitStructure); 										//Reset init structure
 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);		//Change mapping of pins
	// Setup All LED on STM32-Discovery Board to use PWM.
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;								//Pin1 to use TIM5 CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            	//Alt Function - Push Pull
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
       

	TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct );
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseInitStruct.TIM_Period = 20000 ; 							//Set to match motor polling frequency
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84; 
	TIM_TimeBaseInit( TIM5, &TIM_TimeBaseInitStruct );

	TIM_OCStructInit( &TIM_OCInitStruct );
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_Pulse = 500;
	//Channel 1-4
	TIM_OC1Init( TIM5, &TIM_OCInitStruct ); 
	TIM_OC2Init( TIM5, &TIM_OCInitStruct ); 
	TIM_OC3Init( TIM5, &TIM_OCInitStruct ); 
	TIM_OC4Init( TIM5, &TIM_OCInitStruct );

	TIM_Cmd( TIM5, ENABLE ); 
}

/**
 * @brief  Moves the motors to the correct angles.
 * @note   Linear relation between angle and torque:
 * 				 Y = mX + c
 * @param  angle: The angle to move the motors to.
 * @return void
 */
void motorMove(angle_data angle){
	tRoll  = 10*angle.roll +1500;  			//Calculate torque for roll
	tPitch = 10*angle.pitch+1500;				//Calculate torque for pitch

	TIM4->CCR1 = tRoll; 	// set roll torque
	TIM5->CCR2 = tPitch; 	// set oitch torque
}
