#include "KEYPAD.h"
#include "cmsis_os.h"

extern const osThreadId BUTTON_tid;

void KEYPAD_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(KEYPAD_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = KEYPAD_COL_1 | KEYPAD_COL_2 | KEYPAD_COL_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(KEYPAD_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEYPAD_ROW_4 | KEYPAD_ROW_1 | KEYPAD_ROW_2 | KEYPAD_ROW_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(KEYPAD_GPIO, &GPIO_InitStructure);
	
	//Enable SysCfg clock
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//   
// 	//Connect GPIOD to EXTI_0
// 	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource0);
// 	
// 	//Configure EXTI_Line0
// 	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
// 	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
// 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
// 	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
// 	EXTI_Init(&EXTI_InitStructure);
//       
// 	//Same process as in timers.c
// 	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);
}

uint8_t KEYPAD_Check(void){
	uint8_t pressed_button = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(!GPIO_ReadInputDataBit(KEYPAD_GPIO, KEYPAD_ROW_4)) pressed_button = 4;
	if(!GPIO_ReadInputDataBit(KEYPAD_GPIO, KEYPAD_ROW_3)) pressed_button = 3;
	if(!GPIO_ReadInputDataBit(KEYPAD_GPIO, KEYPAD_ROW_2)) pressed_button = 2;
	if(!GPIO_ReadInputDataBit(KEYPAD_GPIO, KEYPAD_ROW_1)) pressed_button = 1;
	
	
	GPIO_InitStructure.GPIO_Pin = KEYPAD_ROW_4 | KEYPAD_ROW_1 | KEYPAD_ROW_2 | KEYPAD_ROW_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(KEYPAD_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEYPAD_COL_1 | KEYPAD_COL_2 | KEYPAD_COL_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(KEYPAD_GPIO, &GPIO_InitStructure);
	
	if(!GPIO_ReadInputDataBit(KEYPAD_GPIO, KEYPAD_COL_1)) pressed_button += 10;
	if(!GPIO_ReadInputDataBit(KEYPAD_GPIO, KEYPAD_COL_2)) pressed_button += 20;
	if(!GPIO_ReadInputDataBit(KEYPAD_GPIO, KEYPAD_COL_3)) pressed_button += 30;
	
	KEYPAD_Init();
	
	switch(pressed_button){
		case 11:
			return 1;
		case 12:
			return 4;
		case 13:
			return 7;
		case 14:
			return 14;
		case 21:
			return 2;
		case 22:
			return 5;
		case 23:
			return 8;
		case 24:
			return 0;
		case 31:
			return 3;
		case 32:
			return 6;
		case 33:
			return 9;
		case 34:
			return 34;
	}
	
	return 99;
}

/**
 * @brief The EXTI_Line0 interrupt handler.
 *
 * @note	The interrupt handler checks whether tap is greater than 0. If
 *				it is, the value remains the same. If it is not, then tap is
 *				set to 13. Combined with the logic in main.c and a 25Hz clock,
 *				this creates a timeout of about half a second during which
 *				additional taps are not detected.
 */
void EXTI0_IRQHandler(void){
// 	tap = (tap > 0) ? tap : 13;
// 	osSignalSet(BUTTON_tid, 0x1);
// 	
// 	EXTI_DeInit();
}

angle_data KEYPAD_buttonToAngle(uint8_t button){
	angle_data return_val;
	switch(button){
		case 1:
			return_val.pitch = 90;
			return_val.roll = -90;
			break;
		case 4:
			return_val.pitch = 0;
			return_val.roll = -90;
			break;
		case 7:
			return_val.pitch = 90;
			return_val.roll = -90;
			break;
		case 14:
			return_val.pitch = 0;
			return_val.roll = 0;
			break;
		case 2:
			return_val.pitch = 90;
			return_val.roll = 0;
			break;
		case 5:
			return_val.pitch = 0;
			return_val.roll = 0;
			break;
		case 8:
			return_val.pitch = -90;
			return_val.roll = 0;
			break;
		case 0:
			return_val.pitch = -333;
			return_val.roll = -333;
			break;
		case 3:
			return_val.pitch = 90;
			return_val.roll = 90;
			break;
		case 6:
			return_val.pitch = 0;
			return_val.roll = 90;
			break;
		case 9:
			return_val.pitch = -90;
			return_val.roll = 90;
			break;
		case 34:
			return_val.pitch = -666;
			return_val.roll = -666;
			break;
	}
	return return_val;
}
