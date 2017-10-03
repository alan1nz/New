/*
 * uart.c
 *
 * Created: 7/09/2017 1:31:40 p.m.
 *  Author: xcao031
 */ 

 #include "uart.h"


 void uart_init(){
	UBRR0	= UBRR; //Setting baud rate to 9600
	//Enable transmitter and receiver
	UCSR0B |= (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0)|(1<<TXCIE0);
 }


 void uart_transmit(unsigned char data){
	UCSR0B &= ~(1<<RXEN0);
	while(!(UCSR0A&(1<<UDRE0)));
	UDR0 = data;
 }


 void uart_rx_error(){
 }

 int uart_new_request(char a, char b, char c){
	int new_request = (a-48)*1000 + (b-48)*100 + (c-48)*10;
	if (new_request < 300){
		if (new_request == 0) {
			return 0;
		} else {
			//status_flag |= (1 << speed_toolow);
			return 300;
		}
	} else if (new_request > 2700) {
			//status_flag |= (1 << speed_toohigh);
		return 2700;
	} else {
		return new_request;
	}
 }