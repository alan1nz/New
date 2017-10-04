/*
 * ADC.c
 *
 * Created: 12/09/2017 11:14:15 a.m.
 *  Author: jyan617
 */ 

 #include "ADC.h"
 volatile int count = 0;
 volatile uint16_t cur_reading = 0;
 volatile uint16_t pre_reading = 0;
 volatile double power = 0;
 volatile double avr_power = 0;

 void adc_init(){
	 
	 //PB0 and PB1 as input, current/voltage calculation
	 DDRB &= ~((1<<DDRB0)|(1<<DDRB1));
	 
	 //internal 1.1V as reference
	// ADMUXB |= (1<<REFS0);
	 
	 //Enable ADC, set prescaler 64, gives 125kHz
	 ADCSRA |= (1<<ADEN)|(1<<ADPS1)|(1<<ADPS2);
	 ADCSRA |= (1<<ADATE);
	 //ADMUXA = 0;
	 ADMUXA = 11;
	 ADCSRA |= (1<<ADSC);
	 ADC = 0;
 }

 int cal_adc(unsigned int adc) {
	 //conversion from adc to input signal
	 double stepsize = (double)5/(double)1024;
	 int Vin = stepsize * adc * 100;

	 return Vin; //return Vin
 }

void pwr_cal(){

	if(count < 100){
		cur_reading = cal_adc(ADC);

		//calculate power
		double temp_power = pre_reading * cur_reading;
		temp_power = temp_power / 10000;
		power = power + temp_power;

		//Increment the number of power values and switch channel
		count++;
		ADMUXA ^= (1<<0);

		//Update previous reading to the current reading
		pre_reading = cur_reading ;

	}else{
		
		//take the average value
		avr_power = (power/99)/0.375;
		
		//return avr_power;
		
		//convert power into a string using the double to string function
		//Transmit power
		//char temp[5];
		//dtostrf(avr_power,5,2,temp);
		//uart_transmit('|');
		//uart_transmit(temp[0]);
		//uart_transmit(temp[1]);
		//uart_transmit(temp[2]);
		//uart_transmit(temp[3]);
		//uart_transmit(temp[4]);
		//uart_transmit('|');
		//uart_transmit(' ');
		

		//Reinitialize everything to start another power reading
		pre_reading = 0;
		cur_reading  = 0;
		count = 0;
		power = 0;
	}
}
