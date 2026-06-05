#ifndef _ADC_H
#define _ADC_H

#include "headfile.h"

#define ADC_Num 5
#define ADC_Avenum 5

extern uint32  max_v[5];
extern float ADC_GYH[ADC_Num];  
extern uint16 ADC_Get[ADC_Num];

void ADC_Sample();
void ADC_Average();
void ADC_Init1(void);
void ADC_Normalizing();
void Quicksort(int array[], int maxlen, int begin, int end) reentrant;
	
#endif
