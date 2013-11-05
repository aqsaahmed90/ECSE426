/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 *
 * @brief 	Implementation of the filter
 */
 
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "filter_config.h"
#include "adc_config.h"

extern const uint16_t D;

struct Lowpass_Filter;


/**
 * @brief Initializes the filter.
 *
 * @notes Initializes the filter by setting all values to 0.
 */
float Filter_Configuration(Lowpass_Filter *Filter_Structure){
	int i;
	Filter_Structure->average = 0;
	Filter_Structure->new_placement = 0;
	for(i=0; i<D; i++){
		Filter_Structure->buff[i] = 0;
	}
	Filter_Structure->average /= D;
	return Filter_Structure->average;
}

/**
 * @brief Reads in a value and returns the filtered value.
 * 
 * @note 	Takes in a value, inserts it into the correct place
 * 				into the array, computes the average of the group,
 *				and returns the filtered value. It also updates the
 *				filter structure so that the next value is places in
 *				the next buffer slot.
 *
 * @param *Filter_Structure 	The filter structure that contains
 *														the buffer and all other elements.
 * @param new_Value						The unfiltered value to be inserted
 *														into the filter and processed.
 * @return The filtered value.
 */
float filter_Value(Lowpass_Filter *Filter_Structure, float new_Value){
	int i;
	//Put the new value where it belongs
	Filter_Structure->buff[Filter_Structure->new_placement] = new_Value;
	Filter_Structure->average = 0;
	for(i=0; i<D; i++){
		//Add up all values in buffer
		Filter_Structure->average += Filter_Structure->buff[i];
	}
	//Divide by how many there are
	Filter_Structure->average /= D;
	//If the last number in the filter is 0, just return the value read in, filter
	//hasn't read in D numbers yet.
	if(Filter_Structure->buff[D-1] == 0) Filter_Structure->average = -100;
	//Update where new numbers will be placed. Modulus handles wrap around.
	Filter_Structure->new_placement = (Filter_Structure->new_placement + 1) % D;
	return Filter_Structure->average;
}
