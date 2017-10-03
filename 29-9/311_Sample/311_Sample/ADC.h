/*
 * ADC.h
 *
 * Created: 12/09/2017 11:14:01 a.m.
 *  Author: jyan617
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

void adc_init();
int cal_adc(unsigned int adc);
float pwr_cal();


#endif /* ADC_H_ */