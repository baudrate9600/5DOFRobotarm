/*
 * timer.h
 * 
 * Created: 5/30/2020 3:12:28 PM
 *  Author: sojim
 */ 


#ifndef TIMER_H_
#define TIMER_H_
#include <avr/io.h>

//global variable storing the elapsed milliseconds passed 

void timer_enable();
void timer_disable();
void timer_reset();
uint32_t timer_10k() ;



#endif /* TIMER_H_ */