#ifndef FILTER_PROTECT
#define FILTER_PROTECT
#define D  13
///extern const int D;

typedef struct{
	float average;			/*!< Specifies the average of the filter.*/
	float buff[D];			/*!< Specifies buffer of the filter.*/
	int new_placement;	/*!< Specifies placement of the next value.*/
}Lowpass_Filter;		

float Filter_Configuration(Lowpass_Filter *Filter_Structure);
float filter_Value(Lowpass_Filter *Filter_Structure, float new_Value);
#endif
