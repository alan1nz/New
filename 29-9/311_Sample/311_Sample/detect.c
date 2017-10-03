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