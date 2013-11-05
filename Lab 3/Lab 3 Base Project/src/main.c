//Given includes
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
//#include "stm32f4_discovery_lis302dl.h"

//Our header files
#include "accelerometer.h"
#include "timers.h"
#include "gpio.h"
#include "filter.h"
#include <math.h>

/* TO DO: UPDATE COMMENTS EVERY TIME YOU CREATE A METHOD.
	- Read values from accelerometer 											(DONE)
		+Configure and initialize accelerometer 						(DONE)
		+Determine correct settings and why they're 				(DONE)
		 correct																						(DONE)
		+Read from accelerometer 														(DONE)
		+Convert reading to G's															(DONE)
	- Set up timer 																				(DONE)
		+Configure and initialize 													(DONE)
			=25 Hz 																						(DONE)
			=Prescaler and divisor can't overflow registers		(DONE)
		+Create interrupt routine 													(DONE)
			=Make it small! 																	(DONE)
		+Modify main to run on interrupt. NO SPINLOCKING 		(DONE)
	- Calibration																					(DONE)
		+Create Second Program															(DONE)
			=Use already written code from main								(DONE)
		+Create code for calibration process								(DONE)
			=See slides and documentation for details					(DONE)
		+Run Calibration process														(DONE)
			=Several times if you want, I don't really				(DONE)
			 care. It will probably be easy to run in the			(DONE)
			 future.																					(DONE)
		+Put results of calibration into main program				(DONE)
			=See slides for equations.												(DONE)
			=Equations should go in read_Accelerometer.				(DONE)
	- Filtering (LAB 2)																		(DONE)
		+Add in code for the filter													(DONE)
		+Matlab analysis must be preformed on filter depth	(DONE)
		 as in Lab 2.																				(DONE)
	- Detect single taps																	(DONE)
		+Generate interrupt on single tap										(DONE)
		+Functionality of accelerometer											(DONE)
			=Look in documentation for process.								(DONE)
		+Switch mode on single tap													(DONE)
			=Think of a better way than a FSM.								(DONE)
	- Hardware PWM																				(DONE)
		+Hard way:																					(DONE)
			=Configure and initialize TIM4										(DONE)
			=Look at example peripheral projects for how			(DONE)
			 to get PWM working																(DONE)
		+Look at config/setup to figure out what settings		(DONE)
		 do																									(DONE)
	-Use LEDs to display angle data												(DONE)
	-Go through driver code to figure out what accel calls(DONE)
	 are doing																						(DONE)
	-Prove with oscilliscope that clock is going at				(Ben & Tanuj)
	 25Hz																									(Ben & Tanuj)
*/

volatile uint_fast16_t tick = 0;
volatile uint_fast16_t tap = 0;



int main(){
	GPIO_Config();
	INTTIM_Config();
	ACCEL_Config();
  
  int cycle_off = 1500;
	int direction = 0;
	int i = 0;
	int CCRN = 0;
	int CCRE = 0;
	int CCRS = 0;
	int CCRW = 0;
	int mode = 0; //0 = display, 1 = PWM;

	angle_data new_vals;
	GPIO_Config();
	INTTIM_Config();
	ACCEL_Config();
	
	while(1) {
		if(tick){
			new_vals = read_Accelerometer();
			tick = 0;
			if(tap > 0){
				if(tap == 13){
					mode = !mode;
					if(mode){
						cycle_off = 1500;
						i = 0;
					}
					else{
						cycle_off = 0;
					}
				}
				Clear_Accel_Int();
				tap--;
			}
		}
		if(mode){
			i++;
			if(i>10000){
					i = 0;

          TIM4->CCR1 = 1500 - (cycle_off); // West
          TIM4->CCR2 = 1500 - (cycle_off); // North
          TIM4->CCR3 = 1500 - (cycle_off); // East
          TIM4->CCR4 = 1500 - (cycle_off); // South
					cycle_off = (direction) ? cycle_off + 1 : cycle_off - 1;
					if(cycle_off <= 0 || cycle_off >= 1500){
						direction = !direction;
						cycle_off = (cycle_off <=0) ? 1 : 1500;
					}
			}
		}
		if(!mode){
			//.2867*90 = 665.98 This means that CCRs vary between 0 and ~666, that is,
			//always on and always off.
			//Pitch and roll vary between -90 and 90. As pitch approaches 90 CCRS 
			//should increase and CCRN should stay at 0. As pitch approaches -90
			//CCRN should increase.
			//As roll -> 90, CCRE should increase. Roll -> -90, CCRW increases.
			CCRS = (new_vals.pitch) > 0 ? pow((0.43033 * new_vals.pitch),2) : 0;
			CCRN = (new_vals.pitch) < 0 ? pow((0.43033 * new_vals.pitch),2) : 0;
			CCRE = (new_vals.roll) > 0 ? pow((0.43033 * new_vals.roll),2) : 0;
			CCRW = (new_vals.roll) < 0 ? pow((0.43033 * new_vals.roll),2) : 0;
			TIM4->CCR4 = CCRS;
			TIM4->CCR2 = CCRN;
			TIM4->CCR1 = CCRW;
			TIM4->CCR3 = CCRE;
			printf("Pitch: %f\t||Roll: %f\n", new_vals.pitch, new_vals.roll);
		}
	}
}

