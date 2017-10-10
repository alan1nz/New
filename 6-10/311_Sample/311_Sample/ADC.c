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
 volatile double power = 0 ;
 
 //volatile double avr_power = 0;

 void adc_init(){
	 
	 //PB0 and PB1 as input, current/voltage calculation
	 DDRB &= ~((1<<DDRB0)|(1<<DDRB1));
	 	 
	 //Enable ADC, set prescaler 64, gives 125kHz
	 ADCSRA |= (1<<ADEN)|(1<<ADPS1)|(1<<ADPS2);
	 //ADCSRA |= (1<<ADATE);
	 ADMUXA = 0;
	 ADMUXA = 11;
	 ADCSRA |= (1<<ADSC);
 }


int read_adc(){
	
	while(!(ADCSRA & (1<<ADIF))); //Wait until conversion is complete
	return ADC;
}


 //This function converts the digital representation of the sampled signal to its analogue value
 int cal_adc() {
	 //conversion from adc to input signal
	 double stepsize = (double)5/(double)1024;
	 int Vin = stepsize * read_adc() * 100;
	 ADC = 0;  //Flush ADC register after reading
	 return Vin; //return Vin
 }

double pwr_cal(){
	
		cur_reading = cal_adc();

		//calculate power
		double temp_power = pre_reading * cur_reading;
		temp_power = temp_power / 10000;
		
		
		
		ADMUXA ^= (1<<0);
		//Update previous reading to the current reading
		pre_reading = cur_reading ;
		
									//char temp[4];
									//dtostrf(temp_power,4,2,temp);
									//uart_transmit('|');
									//uart_transmit(temp[0]);
									//uart_transmit(temp[1]);
									//uart_transmit(temp[2]);
									//uart_transmit(temp[3]);
									//uart_transmit(temp[4]);
									//uart_transmit('|');
									//uart_transmit(' ');
		
		return temp_power*2.667;
}

