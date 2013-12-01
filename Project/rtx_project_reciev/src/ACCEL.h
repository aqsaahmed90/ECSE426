/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 */
 
 
#ifndef ACCELEROMETER_PROTECTION
#define ACCELEROMETER_PROTECTION

#include "stm32f4xx.h"

typedef struct{
	//Data that comes from the accelerometer is in 2;s com-
	//plement, so signed ints.
	int32_t x; /*!< Acceleration for the X axis.*/
	int32_t y; /*!< Acceleration for the Y axis.*/
	int32_t z; /*!< Acceleration for the Z axis.*/
}accel_data;

typedef struct{
	float pitch;
	float roll;
}angle_data;

void ACCEL_Config(void);
void Clear_Accel_Int(void);
angle_data read_Accelerometer(void);

#endif
