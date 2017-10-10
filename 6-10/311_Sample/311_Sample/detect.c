/*
 * detect.c
 *
 * Created: 27/09/2017 2:42:12 p.m.
 *  Author: jyan617
 */ 
 #include "detect.h"

 int locked(int status_flag, double pwr_reading, int rpm_reading){
	 if (rpm_reading == 0){
		 if (pwr_reading = 0){
			//when the fan is actually stop, do nothing
			status_flag &= ~(1<<7);
			return status_flag;
			 } else {
			 //when the motor is locked by foreign objects, stop the motor and raise the flag
			 TOCPMCOE &= ~(1<<TOCC2OE);
			 TOCPMCOE &= ~(1<<TOCC4OE);
			 status_flag |= (1<<7);
			 return status_flag;
		 }
		 } else {
			//when the fan is actually stop, do nothing
			status_flag &= ~(1<<7);
			return status_flag;
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
 