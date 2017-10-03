/*
 * ADC.c
 *
 * Created: 12/09/2017 11:14:15 a.m.
 *  Author: jyan617
 */ 

 #include "ADC.h"


 void adc_init(){
	 
	 //PB0 and PB1 as input, current/voltage calculation
	 DDRB &= ~((1<<DDRB0)|(1<<DDRB1));
	 
	 //internal 1.1V as reference
	// ADMUXB |= (1<<REFS0);
	 
	 //Enable ADC, set prescaler 64, gives 125kHz
	 ADCSRA |= (1<<ADEN)|(1<<ADPS1)|(1<<ADPS2)|(1<<ADIE);
	 ADCSRA |= (1<<ADATE);
	 //ADMUXA = 0;
	 ADMUXA = 11;
	 //ADCSRA |= (1<<ADSC);
	 ADC = 0;
 }

 int cal_adc(unsigned int adc) {
	 //conversion from adc to input signal
	 double stepsize = (double)5/(double)1024;
	 int Vin = stepsize * adc * 100;

	 return Vin; //return Vin
 }

 float pwr_cal(){


	 //float power = 0;
	 //int count = 0;

	 /*read 100 values
	 if (count < 100){
		 voltage += cal_adc(read_adc(10));
		 current += cal_adc(read_adc(11));
		 count++;
	 }else{
		
	 }

	 //average from 100 values


	 //power calculation
	 //power = voltage * current;
	 return 0;
	 */
 }