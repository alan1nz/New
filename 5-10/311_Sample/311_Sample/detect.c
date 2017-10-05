/*
 * detect.c
 *
 * Created: 27/09/2017 2:42:12 p.m.
 *  Author: jyan617
 */ 
 #include "detect.h"

 void locked(double pwr_reading, int rpm_reading){
	 if (rpm_reading == 0){
		 if (pwr_reading = 0){
			 PORTA &= ~(1<<PORTA7);
			 } else {
			 PORTA |= (1<<PORTA7);
		 }
		 } else {
		 PORTA &= ~(1<<PORTA7);
	 }
 }
 
 
 void restart_fan(){ 
		 if (PINA & (1<<PINA0) ) {
			 TOCPMCOE |= (1<<TOCC4OE);
			 TOCPMCOE &= ~(1<<TOCC2OE);
		 }
		 else {
			 TOCPMCOE |= (1<<TOCC2OE);
			 TOCPMCOE &= ~(1<<TOCC4OE);
		 }
		 OCR1B = 50 ;
 }
 