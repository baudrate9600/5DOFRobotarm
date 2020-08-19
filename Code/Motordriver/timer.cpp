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
uint32_t timer_counter_2;

//This snippet was generated with this online program
//https://www.arduinoslovakia.eu/application/timer-calculator
void timer_enable(){
	// Clear registers
	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2 = 0;

	// 10000 Hz (16000000/((24+1)*64))
	OCR2A = 4;
	// CTC
	TCCR2A |= (1 << WGM21);
	// Prescaler 64
	TCCR2B |= (1 << CS22);
	// Output Compare Match A Interrupt Enable
	TIMSK2 |= (1 << OCIE2A);
	
	timer_counter = 0;
	timer_counter_2  = 0;
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
uint32_t timer_10k(){
	return timer_counter;
}
uint32_t timer_50k(){
	return timer_counter_2;
}

//interrupt vector 
ISR (TIMER2_COMPA_vect) {
	timer_counter++;
}

ISR (TIMER2_COMPB_vect){
	timer_counter_2++;
}