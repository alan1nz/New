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
#define stop_fan 1
#define input_error 2
#define ten 3
#define check_blocked 4
#define blocked_duct 5
#define check_locked 6
#define locked_motor 7


volatile char temp[3];
volatile int num = 0;
volatile int status_flag = 0b00000000;
volatile  int request_speed = 1000;

volatile int cur_speed = 0;
volatile uint16_t accum_speed = 0;
volatile int error = 0;

volatile uint16_t rising_edge = 0;
volatile uint16_t falling_edge = 0;
volatile uint16_t  n_over_flow = 0;

volatile int count_rpm = 0;
volatile double cur_pwr = 0;


volatile int blocked_d_counter = 0;
volatile int count_locked = 0;
volatile int ten_sec = 0 ;
volatile int one_sec = 0 ;

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
			//uart_response(status_flag);
		} else {
			//if no request speed, send back current fan info
			if (temp[21] == ' '){
				uart_response(status_flag, temp[21], temp[22], temp[23], cur_speed, cur_pwr);
			} else {
				
				request_speed = uart_new_request(temp[21],temp[22],temp[23]); //Decode request speed
				status_flag = speed_detection(status_flag,temp[21],temp[22],temp[23]); //Raise error flags if any
				
				// check if user wants to clear the error
				if (temp[26] == 44) {
					status_flag &= ~((1<<speed_toolow)|(1<<blocked_duct)|(1<<locked_motor));
				}
				
				//transmit the Json package
				uart_response(status_flag, temp[21], temp[22], temp[23], cur_speed, cur_pwr);
			}		
		}	
	}
	//clear temp[28] to reenable writing error to the flag
	temp[28] = ' ';
	//   {"3":{"spd":{"req": "050"},"clr":"ew"}}
	//   {"3":{"spd":{"req": "050"}           }}
	//   {"3":{                   }           }}
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
			cur_speed = (accum_speed / 11);
			//char y[5] = {0};
			//sprintf(y,"%d",cur_speed);
			//uart_transmit(y[0]);
			//uart_transmit(y[1]);
			//uart_transmit(y[2]);
			//uart_transmit(y[3]);
			//uart_transmit(' ');
			error = request_speed - cur_speed; //Calculate error

			//if( (error > 270)||(error < -270)){
				//status_flag &= ~(1<<speed_steady); //Speed is not steady if error is too large
			//}else{
				//status_flag |= (1<<speed_steady);
			//}

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
	n_over_flow ++;  //number of overflow used in speed calculation
	count_locked ++; //Timer for running blocked duct check
	blocked_d_counter ++;
	one_sec++;

	if(blocked_d_counter > 59){					//Raise a flag every 30 seconds
		status_flag |= (1<<check_blocked);    //Check to see if the duct is blocked in the main code
		blocked_d_counter = 0;				//Reset counter
	}
	
	if(status_flag &(1<<check_blocked)){
		ten_sec ++;  //Start counting for 10 seconds;
	}
	
	if(ten_sec > 10){
		status_flag |= (1<<ten);  // Raise the flag and start calculating for steady state error for blocked duct testing
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
	
	
	int store_speed = 0 ;

	while (1){
				//locked(pwr_cal(),cur_speed);
				//uart_transmit('x\"');
				
				ADCSRA |= (1<<ADIF); //Clear the ADC interrupt flag
				ADCSRA |= (1<<ADSC); //Manually start another conversion
				
				if (request_speed != 0){
					pid(error,total_error);
					total_error = total_error + error*i_coefficient; //Add error for the integral term of PID
					if(total_error < (-1)){
						total_error = 0;
					}
				} else {
					OCR1B = 0;
				}
				
				

				
				if(((cur_speed == 0)&&(request_speed !=0)) && (status_flag & ~(1<<locked_motor))){ //Start the fan in when it is stationary
					restart_fan();
				}
				
				
				
				if( (status_flag & (1<<check_blocked))  && (request_speed != 0)){ //Check for blocked duct
					
					store_speed = request_speed ;
					OCR1B = 267 ; //Make the fan to run at 100% duty cycle
					request_speed = 2700 ;  // Make the fan to run to max RPM
					
					
					while(!(status_flag&(1<<ten)));     // while 10 seconds hasn't passed yet, do nothing
					
					if(error > 150){
						status_flag |= (1<<blocked_duct);
					}else{
						status_flag &= ~(1<<blocked_duct);
					}
					
					ten_sec = 0 ;				//Reset counter
					status_flag &= ~(1<<ten);  //Clear the 10 second counter and check for blocked duct
					status_flag &= ~(1<<check_blocked); 
					
				
					request_speed = store_speed;
				}



				//constantly check if the fan is locked
				if(count_locked > 3){
					status_flag = locked(status_flag, cur_pwr, cur_speed, request_speed);
					count_locked = 0;
				}
				
				//Attemp to restart the fan if it's blocked
				if((status_flag & (1<<locked_motor))&&(n_over_flow > 10)){
					if (status_flag & ~(1<<stop_fan)){
						restart_fan();
					}
				}
				
				//Send fan's info back every one sec
				if (one_sec > 1) {
					uart_response(status_flag, temp[21], temp[22], temp[23], cur_speed, cur_pwr);
					one_sec = 0;
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
