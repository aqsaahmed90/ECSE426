#include "stm32f4xx.h"

#ifndef KEYPAD
#define KEYPAD

#define KEYPAD_GPIO GPIOD
#define KEYPAD_GPIO_CLK RCC_AHB1Periph_GPIOD
#define KEYPAD_ROW_4 GPIO_Pin_0
#define KEYPAD_PIN_4 GPIO_Pin_4
#define KEYPAD_ROW_1 GPIO_Pin_1
#define KEYPAD_ROW_2 GPIO_Pin_2
#define KEYPAD_ROW_3 GPIO_Pin_3
#define KEYPAD_COL_1 GPIO_Pin_5
#define KEYPAD_COL_2 GPIO_Pin_6
#define KEYPAD_COL_3 GPIO_Pin_7


void KEYPAD_Init(void);
uint8_t KEYPAD_Check(void);
#endif
