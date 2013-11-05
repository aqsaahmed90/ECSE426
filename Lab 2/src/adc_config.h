#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#ifndef ADC_CONFIG
#define ADC_CONFIG

void ADC_Configuration(ADC_CommonInitTypeDef *ADC_Common_InitStructure, ADC_InitTypeDef *ADC_InitStructure);
float read_Temp(void);
#endif
