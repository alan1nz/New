
#include "timer.h"

void timer_init(){
	
	//Setting fast PWM mode OCR1B to operate in non-inverting mode.
	TCCR1B |= (1<<CS10)|(1 << WGM12)|(1<<WGM13)|(1<<ICES1);
	TCCR1A |= (1<< WGM11)|(1<<WGM10)|(1<<COM1B1);
	
	//route PWM to switches
	TOCPMSA1 |= (1<<TOCC4S0);
	TOCPMSA0 |= (1<<TOCC2S0);

	TOCPMCOE |= (1<<TOCC4OE);

	//Initilise timer 2 for input capture
	TIMSK2 |= (1<<ICIE2)|(1<<TOIE2);
	TCCR2B |= (1<<CS21)|(1<<ICES2)|(1<<CS20);
	TCCR2A |= (1<<COM2B1);
	
	
	TCNT0 = 0;

	//Setting 30k Hz PWM
	OCR1A = 266;

	//Setting duty cycle
	OCR1B = 267;
}

