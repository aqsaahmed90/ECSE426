#include "KEYPAD.h"
#include "cmsis_os.h"

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
}

uint8_t KEYPAD_Check(void){
	uint8_t pressed_button;
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
}
