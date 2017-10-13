/*
 * PID.h
 *
 * Created: 9/26/2017 12:37:41 PM
 *  Author: peng
 */ 


#ifndef PID_H_
#define PID_H_

#define p_coefficient 0.045
#define i_coefficient 0.00016


#include <avr/io.h>

void pid(int error,int sum_error);

#endif /* PID_H_ */
