/*
 * detect.h
 *
 * Created: 27/09/2017 2:41:58 p.m.
 *  Author: jyan617
 */ 


#ifndef DETECT_H_
#define DETECT_H_
#define locked_motor 7

#include <avr/io.h>

int locked(int status_flag, double pwr_reading, int rpm_reading, int request_speed);
void restart_fan();


#endif /* DETECT_H_ */