/*
 * uart.c
 *
 * Created: 7/09/2017 1:31:40 p.m.
 *  Author: xcao031
 */ 

 #include "uart.h"
 //char a[] = " {\"3\":{\"spd\":{\"req\":\" ";
 //char b[] = " \",\"cur\":\" ";
 //char c[] = " \"},\"ver\":\"001.002.003\",\"pwr\":\" ";
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


 void uart_response(int status_flag, char x, char y, char z, int cur_speed, double cur_power){
	cur_speed /= 10;
	cur_power *= 100;
	int cur_pwr = cur_power;
	int count = 0;
			uart_transmit('\n');
		uart_transmit(13);

		uart_transmit('{{');
			uart_transmit('\n');
		uart_transmit('\"');
		uart_transmit('3');
		uart_transmit('\"');
		uart_transmit(':');
			uart_transmit('\n');
		uart_transmit('{{');
			uart_transmit('\n');
		uart_transmit('\"');
		uart_transmit('s');
		uart_transmit('p');
		uart_transmit('d');
		uart_transmit('\"');
		uart_transmit(':');
		uart_transmit('{{');
		uart_transmit('\"');
		uart_transmit('r');
		uart_transmit('e');
		uart_transmit('q');
		uart_transmit('\"');
		uart_transmit(':');
		uart_transmit('\"');
		uart_transmit(x);
		uart_transmit(y);
		uart_transmit(z);
		uart_transmit('\"');
		uart_transmit(',');
		uart_transmit('\"');
		uart_transmit('c');
		uart_transmit('u');
		uart_transmit('r');
		uart_transmit('\"');
		uart_transmit(':');
		uart_transmit('\"');
		//current speed
		uart_transmit(cur_speed/100 + 48);
		uart_transmit(cur_speed%100 / 10 + 48);
		uart_transmit(cur_speed%100 % 10 + 48);
		uart_transmit('\"');
		uart_transmit('}}');
		uart_transmit(',');
			uart_transmit('\n');
		uart_transmit('\"');
		uart_transmit('v');
		uart_transmit('e');
		uart_transmit('r');
		uart_transmit('\"');
		uart_transmit(':');
		uart_transmit('\"');
		//version
		uart_transmit('0');
		uart_transmit('0');
		uart_transmit('1');
		uart_transmit('.');
		uart_transmit('0');
		uart_transmit('0');
		uart_transmit('2');
		uart_transmit('.');
		uart_transmit('0');
		uart_transmit('0');
		uart_transmit('3');
		uart_transmit('\"');
		uart_transmit(',');
			uart_transmit('\n');
		uart_transmit('\"');
		uart_transmit('p');
		uart_transmit('w');
		uart_transmit('r');
		uart_transmit('\"');
		uart_transmit(':');
		uart_transmit('\"');
		//Power
		uart_transmit(cur_pwr/100 + 48);
		uart_transmit('.');
		uart_transmit(cur_pwr%100 / 10 + 48);
		uart_transmit(cur_pwr%100 % 10 + 48);
		uart_transmit('W');
		uart_transmit('\"');
		uart_transmit(',');
			uart_transmit('\n');
		uart_transmit('\"');
		uart_transmit('c');
		uart_transmit('l');
		uart_transmit('r');
		uart_transmit('\"');
		uart_transmit(':');
			uart_transmit('\n');
		uart_transmit('\"');
		uart_transmit('e');
		uart_transmit('w');
		uart_transmit('\"');
		uart_transmit(',');
	//detection
	if (status_flag & ((1<<0) | (1<<5) | (1<<7))) {
		uart_transmit('\"');
		uart_transmit('e');
		uart_transmit('w');
		uart_transmit('\"');
		uart_transmit(':');
		uart_transmit('[');
		if (status_flag & (1<<0)){
			uart_transmit('\"');
			uart_transmit('r');
			uart_transmit('e');
			uart_transmit('q');
			uart_transmit('T');
			uart_transmit('o');
			uart_transmit('o');
			uart_transmit('L');
			uart_transmit('o');
			uart_transmit('w');
			uart_transmit('\"');
			uart_transmit(',');
			count += 12;
		}
		if (status_flag & (1<<5)){
			uart_transmit('\"');
			uart_transmit('b');
			uart_transmit('l');
			uart_transmit('o');
			uart_transmit('c');
			uart_transmit('k');
			uart_transmit('e');
			uart_transmit('d');
			uart_transmit('D');
			uart_transmit('u');
			uart_transmit('c');
			uart_transmit('k');
			uart_transmit('\"');
			uart_transmit(',');
			count += 14;
		}
		if (status_flag & (1<<7)){
			uart_transmit('\"');
			uart_transmit('l');
			uart_transmit('o');
			uart_transmit('c');
			uart_transmit('k');
			uart_transmit('e');
			uart_transmit('d');
			uart_transmit('R');
			uart_transmit('o');
			uart_transmit('t');
			uart_transmit('o');
			uart_transmit('r');
			uart_transmit('\"');
			count += 13;
			}
		while (!(count == 39)){
			uart_transmit(' ');
			count++;
		}
		count = 0;
		uart_transmit(']');
	} else {
		while (!(count == 58)){
				uart_transmit(' ');
				count++;
		}
		count = 0;
	}
			
		uart_transmit('\n');
	uart_transmit('}}');
		uart_transmit('\n');
	uart_transmit('}}');
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

 int speed_detection(int status_flag, char a, char b, char c){
	int new_request = (a-48)*1000 + (b-48)*100 + (c-48)*10;
	
	if (new_request < 300){
		if (new_request == 0) {
			status_flag &= ~((1 << speed_toolow));
			status_flag |= (1 << stop_fan);
		} else {
			status_flag |= (1 << speed_toolow);
			status_flag &= ~(1 << stop_fan);
		}
	} else {
		status_flag &= ~(1<<speed_toolow);
		status_flag &= ~(1<<stop_fan);
	}

 }