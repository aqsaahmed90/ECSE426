#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "CC2500.h"


/*!
 @brief Program entry point
 */
int main (void) {
	CC2500_Init();
	while(1){
		osDelay(osWaitForever);
	}
}
