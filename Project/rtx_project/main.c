#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "CC2500.h"
#include "KEYPAD.h"


/*!
 @brief Program entry point
 */
int main (void) {
	CC2500_Init();
	GPIO_DeInit(KEYPAD_GPIO);
	//KEYPAD_Init();
	uint8_t test;
	while(1){
		CC2500_RXData();
// 		test = KEYPAD_Check();
// 		test++;
		osDelay(10);
	}
}
