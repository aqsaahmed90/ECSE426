/**
 * @file
 * @author 	Ben Swearingen <benjamin.swearingen@mail.mcgill.ca>
 *
 * @brief 	Implementation of the functionality for the ADC
 */
 
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "filter.h"

Lowpass_Filter Filter_Structure;
/**
 * @brief Configures and initializes ADC1.
 *
 * @note 	First initializes the clock to ADC1. Then resets all
 * 				ADC's to ensure that they're in a known state. Then
 *				sets ADC1 to be independent, sets the frequency of the
 *				clock to 131,027, disables DMA, sets the sampling delay
 *				to 5 cycles, and then initializes the structure.
 *				ADC_InitStructure is then configured. The resolution
 *				is set to 12 bits, ScanConvMode and ContinuousConvMode 
 *				are disabled, the external trigger is disabled, data is
 *				aligned right, and the number of conversions is set to 1.
 *				The ADC is then initialized, the temperature sensor is enabled,
 *				and ADC1 is enabled.
 *
 * @param  *ADC_Common_InitStructure 	An ADC_CommonInitTypeDef structure to
 *																		be configured for ADC1.
 * @param *ADC_InitStructure					An ADC_InitTypeDef structure to be
 *																		configured for ADC1.
 */
void ADC_Configuration(void){
	Filter_Configuration(&Filter_Structure);
	ADC_CommonInitTypeDef ADC_Common_InitStructure;
	ADC_InitTypeDef		ADC_InitStructure;
	//Enable the clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	//Set to known state
	ADC_DeInit();
	
	//ADC_Common_InitStructure Configuration
	//ADC operates in Independent mode
	ADC_Common_InitStructure.ADC_Mode = ADC_Mode_Independent;
	//Frequency of the clock to the ADC. ((uint32_t)0x00020000
	ADC_Common_InitStructure.ADC_Prescaler = ADC_Prescaler_Div6;
	//Disable DMA
	ADC_Common_InitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	//Configures delay between two sampling periods (5 cycles)
	//ADC_Common_InitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	//Initialize it
	ADC_CommonInit(&ADC_Common_InitStructure);
	
	//ADC_InitStructure Configuration
	//Resolution of ADC = 12b
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	//Conv occurs in single channel 
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	//Single conversion occurs at a time
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//No external event triggers the conversion
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//Align data right
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//1 conversion will be done
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	//Initialize it
	ADC_Init(ADC1, &ADC_InitStructure);
	//Enable the temp sensor
	ADC_TempSensorVrefintCmd(ENABLE);
	//Enable the ADC
	ADC_Cmd(ADC1, ENABLE);
}


/**
 * @brief Reads the conversion from ADC1 and converts to a temperature in degrees C.
 *
 * @note	Sets up the channel that the conversion is going to occur on. It is going
 *				to occure on ADC1, on the channel that contains the TempSensor, for 480 cycles.
 *				As there are no other ADCs active, it has rank 1 in the sequencer.
 *				The conversion is then started, and runs until the End Of Conversation(EOC) flag
 *				is set to something other than RESET.
 *				After the conversion is done, the flags are reset and the value is read from the
 *				ADC. It is then divided by 2^12-1 and multiplied by 3V so that it is a voltage
 *				that represents the temperature. It is then run through the formula from the data
 *				sheets that converts it into the correct(ish) temperature.
 *
 * @return The current temperature value.
 */
float read_Temp(void){
	uint16_t read_Value;
	float return_Val;
	//Configures for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_480Cycles);
	//Starts the conversion
	ADC_SoftwareStartConv(ADC1);
	//EOC Flag = end of conversion flag
	//While EOC is set to RESET value, the conversion is occuring.
	//When it's changed from that, conversion has ended 
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	//Clear the flag
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	//Get the value
	read_Value = ADC_GetConversionValue(ADC1);
	//Value read from ADC is a 12 bit representation of voltage. 0000 0000 0000 0000 => 0V, 1111... =>3V
	//Need to multiply the value read by 3V, and divide by 2^12. Rest of equation taken from data sheets
	return_Val = ((((float)read_Value * 3000 / 4096) - 760)/2.5) + 25;
	read_Value = filter_Value(&Filter_Structure, read_Value);
	return return_Val;
}
