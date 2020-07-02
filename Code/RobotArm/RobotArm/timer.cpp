/*
 * timer.cpp
 *
 * Created: 5/30/2020 3:29:28 PM
 *  Author: sojim
 */ 

#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

uint32_t timer_counter;

//This snippet was generated with this online program
//https://www.arduinoslovakia.eu/application/timer-calculator
void timer_enable(){
	// Clear registers
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// 1000 Hz (16000000/((249+1)*64))
	OCR1A = 249;
	// CTC
	TCCR1B |= (1 << WGM12);
	// Prescaler 64
	TCCR1B |= (1 << CS11) | (1 << CS10);
	// Output Compare Match A Interrupt Enable
	TIMSK1 |= (1 << OCIE1A);
	timer_counter = 0;
}
//Stop the  timer
void timer_disable(){
	TIMSK1 |= (1 << OCIE1A);
	timer_counter = 0; 
}

//Reset the counter
void timer_reset(){
	timer_counter = 0; 
}

//Return the elapsed time since the counter was enable 
uint32_t timer_ms(){
	return timer_counter;
}

//interrupt vector 
ISR (TIMER1_COMPA_vect) {
	timer_counter++;
}