/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 */
 
 
#include "accelerometer.h"
#include "filter.h"
#include "stm32f4_discovery_lis302dl.h"
#include <math.h>
#ifndef OS_PROTECT
#define OS_PROTECT
#include "cmsis_os.h"
#endif

void Accel_Interrupt_Config(void);

extern const osThreadId tap_tid;

Lowpass_Filter Filter_Structure_X;
Lowpass_Filter Filter_Structure_Y;
Lowpass_Filter Filter_Structure_Z;

/**
 * @brief Configures and initializes the accelerometer.
 *
 * @note 	This method configures and initializes the accelerometer. Power
 *			 	mode is set to active, data rate to 100, all axes are enabled,
 *				full scale is set to 2_3, and self test is set to normal.
 */
void ACCEL_Config(void){
	LIS302DL_InitTypeDef LIS302DL_InitStructure;
	LIS302DL_InterruptConfigTypeDef LIS302DL_InterruptStruct;
	
	//These four parts are what make up the structure.
	//Defined in the driver.
	
	//Power_Mode can either be LIS302DL_LOWPOWERMODE_ACTIVE or 
	//LIS302DL_LOWPOWERMODE_POWERDOWN. Active seems to be the obvious choice
	LIS302DL_InitStructure.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	
	//Data rate is either 100Hz or 400Hz. As we're only polling at
	//25Hz, the slower rate is fine.
	LIS302DL_InitStructure.Output_DataRate = LIS302DL_DATARATE_100;
	//Enable all the axes. Obvious choice
	LIS302DL_InitStructure.Axes_Enable = LIS302DL_XYZ_ENABLE;
	//Either 2_3 or 9_2. 2_3 (0x00) seems to be a scale capable of +-2g. This
	//is appropriate because we are going to be measuring a stable tilt
	LIS302DL_InitStructure.Full_Scale = LIS302DL_FULLSCALE_2_3;
	//SELFTEST_NORMAL = 0x00. This means that the self test mode is off.
	//Self test applies a known force (electromagnetic) to the sensor
	//so that the user can ensure it's working properly. This should be off.
	LIS302DL_InitStructure.Self_Test = LIS302DL_SELFTEST_NORMAL;
	
	LIS302DL_Init(&LIS302DL_InitStructure);
	
	//Enable single taps on the Z axis
	LIS302DL_InterruptStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_Z_ENABLE;
	//Disable double taps
	LIS302DL_InterruptStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_XYZ_DISABLE;
	//Interrupt will latch to high until LIS302DL_CLICK_SRC_REG_ADDR is read.
	LIS302DL_InterruptStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_LATCHED;
	LIS302DL_InterruptConfig(&LIS302DL_InterruptStruct);

	Accel_Interrupt_Config();
	Filter_Configuration(&Filter_Structure_X);
	Filter_Configuration(&Filter_Structure_Y);
	Filter_Configuration(&Filter_Structure_Z);
}

/**
 * @brief Configures the accelerometer tap interrupt.
 *
 * @note 	Configures the external interrupt and NVIC so that the
 *				accelerometer acts as an external interrupt. NVIC
 *				configuration is similar to the same process in timers.c.
 *				The external interrupt is configured to happen on Line0.
 */
void Accel_Interrupt_Config(void){
	EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
	uint8_t ctrl;
        
	//Enable interrupt 1. Value taken from documentation
  ctrl = 0x07;
  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);
  
  //Enable interrupt generation
  ctrl = 0x70;
  LIS302DL_Write(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);
  
  //Configure X/Y axis threshold. Wouldn't work without this line
	//even though no X/Y clicks
  ctrl = 0xFF;
  LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSY_X_REG_ADDR, 1);
  
  //Configure Z threshold. Highest to help with the button/tap
	//avoidance.
  ctrl = 0x0F;
  LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSZ_REG_ADDR, 1);
  
  //Set the time limit to the lowest.
  ctrl = 0x00;
  LIS302DL_Write(&ctrl, LIS302DL_CLICK_TIMELIMIT_REG_ADDR, 1);
	
	//Enable SysCfg clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
	//Connect GPIOE to EXTI_0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
	
	//Configure EXTI_Line0
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
      
	//Same process as in timers.c
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//Make sure that the interrupt isn't latched to high
	Clear_Accel_Int();
}

/**
 * @brief Reads from LIS302DL_CLICK_SRC_REG_ADDR to unlatch the interrupt.
 */
void Clear_Accel_Int(void){
	uint8_t junk[2];
	LIS302DL_Read( junk, LIS302DL_CLICK_SRC_REG_ADDR, 2);
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
	int push = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
	if(!push){
		osSignalSet(tap_tid, 0x1);
	}
	else{
	}
	Clear_Accel_Int();
	EXTI_ClearFlag(LIS302DL_SPI_INT1_EXTI_LINE);
}

/**
 * @brief Reads data from the accelerometer and returns an accel_data.
 *
 * @note	Reads data from the accelerometer and returns three signed
 *				integers contained within an accel_data packet. It starts
 *				reading at address 0x29, which corresponds to Out_X, and
 *				reads the next five registers that represent the three axes.
 *				The results must be casted to a signed integer, and then
 *				multiplied by 18 as each LSB represents 18mgs.
 *
 * @return the pitch and roll in degrees.
 */
angle_data read_Accelerometer(void){
	uint8_t Buffer[5];
	accel_data returned_vals;
	angle_data angle_vals;
	
	//0x29 is the first register for Out_X. Followed
	//by Out_Y and Out_Z, each separated by a "junk" register.
	LIS302DL_Read(Buffer, 0x29, 5);
	returned_vals.x = (int8_t)(Buffer[0]) * 18;
	returned_vals.y = (int8_t)(Buffer[2]) * 18;
	returned_vals.z = (int8_t)(Buffer[4]) * 18;
	
	returned_vals.x += 8.1;
	returned_vals.y += 29.3;
	returned_vals.z += -10.2;
	
	returned_vals.x = filter_Value(&Filter_Structure_X, returned_vals.x);
	returned_vals.y = filter_Value(&Filter_Structure_Y, returned_vals.y);
	returned_vals.z = filter_Value(&Filter_Structure_Z, returned_vals.z);
	
	angle_vals.pitch = (atan(returned_vals.x/sqrt(pow(returned_vals.y,2) + pow(returned_vals.z, 2))))*(180/3.14);
	angle_vals.roll = (atan(returned_vals.y/sqrt(pow(returned_vals.x,2) + pow(returned_vals.z, 2))))*(180/3.14);
	
	return angle_vals;
}
