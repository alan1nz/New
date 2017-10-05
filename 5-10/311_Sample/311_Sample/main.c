/*
 * 311_Sample.c
 *
 * Created: 6/09/2017 10:29:36 a.m.
 * Author : spt764
 */ 

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

#define F_CPU 8000000UL

#define speed_toolow 0
#define speed_toohigh 1
#define input_error 2
#define new_speed_request 3
#define check_blocked 4
#define speed_steady 5


volatile char temp[3];
volatile int num = 0;
volatile int status_flag = 0b00000000;
volatile  int request_speed = 2700;

volatile int cur_speed = 0;
volatile uint16_t accum_speed = 0;
volatile int error = 0;

volatile uint16_t rising_edge = 0;
volatile uint16_t falling_edge = 0;
volatile uint16_t  n_over_flow = 0;

volatile int count_rpm = 0;
volatile double cur_pwr = 0;


volatile int count_blocked = 0;


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
		} else {
			request_speed = uart_new_request(temp[21],temp[22],temp[23]); //Decode request speed
			
			status_flag |= (1<<new_speed_request); //Update that there is a new speed request pending
			status_flag &= ~(1<<speed_steady); //Speed is not steady when a new speed is requested
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
			error = request_speed - cur_speed; //Calculate error
			
			if(error > 270){
				status_flag &= ~(1<<speed_steady); //Speed is not steady if error is too large
			}else{
				status_flag |= (1<<speed_steady);
			}
			
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

	if((status_flag &(1<<new_speed_request))||(status_flag &(1<<speed_steady))){ //Count if there is a new speed request (Because the speed in the transient state) or the fan speed is steady
		count_blocked ++ ;
	}
	
	
	if(count_blocked > 10){					//Make the counter count to 30s
		status_flag |= (1<<check_blocked); //Check to see if the duct is blocked in the main code
		count_blocked = 0;				//Reset counter
		status_flag &= ~(1<<new_speed_request); //Clear so there is no current speed request
	}
	
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
	uart_init();
	adc_init();
	
	//enable global interrupt
	sei();
	
	double temp_power  = 0;
	int power_count =  0 ;
	
	

	while (1){
				//locked(pwr_cal(),cur_speed);
								
				ADCSRA |= (1<<ADIF); //Clear the ADC interrupt flag
				ADCSRA |= (1<<ADSC); //Manually start another conversion
	
				
				pid(error,total_error); 
				
				total_error = total_error + error*i_coefficient; //Add error for the integral term of PID
				if(total_error < (-1)){
					total_error = 0;
				}
				
				

				
				if((cur_speed == 0) && (request_speed !=0)){ //Start the fan in when it is stationary
					restart_fan();
				}
				
				
				
				
				if(status_flag & (1<<check_blocked)){ //Check for blocked duct
					uart_transmit('x');
					status_flag &= ~(1<<check_blocked); //Clear after checking
				}
				
				
				
				//This section of the code obtains 100 samples of power and calculate the average power
				if(power_count < 100){ 
					temp_power = temp_power + pwr_cal();
					power_count ++;
				}else{
					cur_pwr = temp_power / 99;
					power_count = 0;
					temp_power = 0;
				}
	}
}
