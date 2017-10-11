/*
 * uart.h
 *
 * Created: 7/09/2017 1:30:09 p.m.
 *  Author: xcao031
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#define BAUD 9600
#define FOSC 8000000UL
#define UBRR (FOSC/(16UL*BAUD)) - 1
#define speed_toolow 0
#define stop_fan 1
#define check_blocked 4
#define blocked_duct 5
#define check_locked 6
#define locked_motor 7


void uart_init();
void uart_transmit(unsigned char data);
void uart_response(int status_flag, char x, char y, char z, int cur_speed, double cur_power);
int uart_new_request(char a, char b, char c);
int speed_detection(int status_flag, char a, char b, char c);



#endif /* UART_H_ */