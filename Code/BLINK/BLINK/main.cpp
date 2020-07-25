/*
 * BLINK.cpp
 *
 * Created: 7/20/2020 12:31:42 PM
 * Author : sojim
 */ 
#include <avr/io.h>
#include <avr/delay.h>
#define F_CPU 16000000UL

int main(void)
{
	DDRB |= (1 << PORTB1);
    /* Replace with your application code */
    while (1) 
    {
		PORTB ^= (1 << PORTB1); 
		_delay_ms(100);
		_delay_ms(100);
		_delay_ms(100);
		
    }
}

