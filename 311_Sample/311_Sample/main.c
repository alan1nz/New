/*
 * 311_Sample.c
 *
 * Created: 6/09/2017 10:29:36 a.m.
 * Author : spt764
 */ 

#define F_CPU 8000000UL
#define speed_toolow 0
#define speed_toohigh 1
#define input_error 2
#define cur_speed_updated 3


#include <avr/io.h>
#include <stdlib.h>
#include <avr/portpins.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "timer.h"
#include "uart.h"
#include "ADC.h"
#include "PID.h"
#include "detect.h"

volatile char temp[3];
volatile int num = 0;
volatile int status_flag = 0b00000000;
volatile  int request_speed = 1500;

volatile int cur_speed = 0;
volatile uint16_t accum_speed = 0;
volatile int error = 0;

volatile uint16_t rising_edge = 0;
volatile uint16_t falling_edge = 0;
volatile uint16_t  n_over_flow = 0;

volatile int count_rpm = 0;
volatile double cur_pwr = 0;


volatile double total_error = 0;

ISR(PCINT0_vect){
	if (PINA & (1<<PINA0) ) {
		TOCPMCOE |= (1<<TOCC4OE);
		TOCPMCOE &= ~(1<<TOCC2OE);
	}
	else {
		TOCPMCOE |= (1<<TOCC2OE);
		TOCPMCOE &= ~(1<<TOCC4OE);
	}
}

ISR(USART0_TX_vect){
	UCSR0B |= (1<<RXEN0); //Enable UART receiver after transmission
}


ISR(USART0_RX_vect){
	
	temp[num] = UDR0 ;
	num ++;
	if(num == 39){
		num = 0;
		if (!(temp[2] == '3')){
			status_flag |= (1<<input_error);  //Return if the ID is not 3
			uart_rx_error();
		} else {
			request_speed = uart_new_request(temp[21],temp[22],temp[23]); //Decode request speed

			//char y[5] = {0};
			//sprintf(y,"%d",request_speed);
			//uart_transmit(y[0]);
			//uart_transmit(y[1]);
			//uart_transmit(y[2]);
			//uart_transmit(y[3]);
			//uart_transmit(' ');
		}	
	}
	//   {"3":{"spd":{"req": "050"},"clr":"ew"}}
}



ISR(TIMER2_CAPT_vect){

	//if rising edge triggered, update rising_edge value
	if(PINB & (1<<PB2)){
		n_over_flow = 0;
		rising_edge = ICR2;
		falling_edge = 0;

		//Change to falling edge triggered to capture 1/4 of rotation
		TCCR2B ^= (1<<ICES2);
	}else{
		count_rpm++;
		falling_edge = ICR2;
		//Calculate the timer value between rising edge and falling edge,
		//hence calculate temporary rpm for future averaging calculation
		int rpm_temp = 1875000/(n_over_flow*65536 - rising_edge + falling_edge);

		//Calculate the average rpm, taking 12 samples, return error and reset count_rpm
		if(count_rpm < 12) {
			accum_speed += rpm_temp;
		} else {
			cur_speed = accum_speed / 11;
			//char y[5] = {0};
			//sprintf(y,"%d",cur_speed);
			//uart_transmit(y[0]);
			//uart_transmit(y[1]);
			//uart_transmit(y[2]);
			//uart_transmit(y[3]);
			//uart_transmit(' ');
			error = 1000 - cur_speed; //Calculate error
			
			count_rpm = 0;  //Reset all variables to start another calculation
			accum_speed = 0; 
		}
		rising_edge = 0;
		rpm_temp = 0;
		//Toggle bit to change back to rising edge triggered
		TCCR2B ^= (1<<ICES2);
	}
}


ISR(TIMER2_OVF_vect){
	n_over_flow ++;
	//pid(error,total_error, pre_error);
	if (n_over_flow >1){
		cur_speed = 0;
	}
	
}


int main(void)
{
	// Initialize pin
	// Output: PA5, PA3 for fan drivers, PA7 for detection features
	// Input: PA0(Hall Sensor), PB0(ADC), PB1(ADC), PB2(Input Capture) 
	//PA0 corresponds to hall sensor input
	DDRA |= (1<<DDRA5)|(1<<DDRA3)|(1<<DDRA7);
	DDRA &= ~(1<<DDRA0);
	DDRB &= ~((1<<DDRB0)|(DDRB1)|(DDRB2));

	//enable the internal pull-up resistor for the hall sensor input
	PUEA = 0b00000001;

	cli();

	//enable interrupt on PIN7 for hall sensor input
	PCMSK0 |= (1 << PCINT0);
	GIMSK |= (1 << PCIE0);
	

	//Initialize timer, UART and ADC
	timer_init();
	//uart_init();
	//adc_init();
	
	//enable global interrupt
	sei();

	while (1){
				//cur_pwr = pwr_cal();
				//pwr_cal();
				
				pid(error,total_error);
				total_error = total_error + error*i_coefficient; //Add error for the integral term of PID
//
				//if((cur_speed == 0) && (request_speed !=0)){
					//if (PINA & (1<<PINA0) ) {
						////TOCPMCOE |= (1<<TOCC4OE);
						////TOCPMCOE &= ~(1<<TOCC2OE);
						//
							//
					//}
					//else {
						////TOCPMCOE |= (1<<TOCC2OE);
						////TOCPMCOE &= ~(1<<TOCC4OE);
						//
					//
					//}
					//OCR1B = 50 ;
				//}
			}		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
}
