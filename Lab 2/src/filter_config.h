/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 */
 
#ifndef FILTER_HEADER
#define FILTER_HEADER
extern const uint16_t D;

typedef struct{
	float average;			/*!< Specifies the average of the filter.*/
	float buff[6];			/*!< Specifies buffer of the filter.*/
	int new_placement;	/*!< Specifies placement of the next value.*/
}Lowpass_Filter;			

float Filter_Configuration(Lowpass_Filter *Filter_Structure);
float filter_Value(Lowpass_Filter *Filter_Structure, float new_Value);
#endif
