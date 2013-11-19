#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "CC2500.h"


/*!
 @brief Program entry point
 */
int main (void) {
	CC2500_Init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
	while(1){
		osDelay(osWaitForever);
	}
}
