/*
 * PID.c
 *
 * Created: 9/26/2017 12:37:58 PM
 *  Author: peng
 */ 


#include "PID.h"
#include <avr/delay.h>


void pid(int error, int sum_error){
	

	double p_term = error * p_coefficient; //Calculate P term

	double i_term = sum_error; //Set integral term as it is calculated in the main function

	int pid_output = p_term + i_term ; //Add all terms

	if (pid_output > 267){ //Place a limit on PID output
		pid_output = 267; //Place upper limit
	}else if (pid_output < 0){
		pid_output = 9 ; //Place lower limit on duty cycle so the fan does not stop when transitioning from high RPM to low RPM
	}


	_delay_ms(25);
																//char y[5] = {0};
																//sprintf(y,"%d",d_term);
																//uart_transmit(y[0]);
																//uart_transmit(y[1]);
																//uart_transmit(y[2]);
																//uart_transmit(y[3]);
																//uart_transmit(' '); 
	OCR1B = pid_output;
}

