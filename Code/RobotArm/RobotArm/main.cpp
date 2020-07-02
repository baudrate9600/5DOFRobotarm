/*
 * RobotArm.cpp
 * This code controlles stepper motors for a robot arm
 * Created: 6/12/2020 8:42:41 PM
 * Author : Olasoji Makinwa 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "Usart.h"
#include "timer.h"

#define TACHO_0_P (1 << PORTD2)
#define TACHO_0_M (1 << PORTD5)
#define SERVO_0_EN (1 << PORTD3)
#define SERVO_0_DIR_A (1 << PORTD6)
#define SERVO_0_DIR_B (1 << PORTD7) 

#define SERVO_0_PWM OCR2B

#define M0_STEP (1 << PORTC0)  
#define M0_DIR	(1 << PORTC1)

#define BUFFER_SEND_SIZE = 16  


ISR(USART_RX_vect){
	UDR0;
}

/* Offloads all the characters stored in character register and sends
 * them via the uart */
volatile char usart_send_buffer = 0; 
volatile char usart_send_counter = 0; 

/*(Static)Linked list to implement buffer register for the send and recive*/
struct buffer_element{
	char data; 
	buffer_element *next;
	};
buffer_element * buffer_head = NULL;
buffer_element * tail = NULL;

void buffer_set_head(char c){
	buffer_head = (buffer_element*)malloc(sizeof(buffer_element));	
	buffer_head->data = c; 
	tail = buffer_head;
}
void buffer_add(char c){
	buffer_element * temp = (buffer_element*)malloc(sizeof(buffer_element));	
    tail->next = temp ; 
	temp->data = c; 
	tail = temp;
}
char buffer_read(){
	char c = buffer_head->data; 
	buffer_element * temp = buffer_head; 
	buffer_head = buffer_head->next;
	free(temp);
	return c;
}

ISR(USART_TX_vect){
	//if(usart_fifo_counter > 0){
	//	UDR0 = usart_	
	//}	
}

int main(void)
{
	/* initialize component */ 
	sei();
	timer_enable();	
	usart_enable(9600);
	/* Initialize motors */ 
//	Stepper_motor motor0(M0_STEP,M0_DIR,0.23,&DDRD);
	
    /* Replace with your application code */
	uint8_t tacho_values;
	uint8_t tacho_state = 0;
	int tacho = 0;
	DDRD &= ~TACHO_0_M;
	DDRD &= ~TACHO_0_P; 

	/* Initialize servo motors */
	DDRD |= SERVO_0_EN | SERVO_0_DIR_A | SERVO_0_DIR_B;
	TCCR2A |= (1 << COM2B1 ) | (1 << WGM21) | (1 << WGM20); /*Fast PWM */
	TCCR2B |= (1 << CS20); /*No prescaling */
	/* PID */
	int error = 0;	
	int target_position = 0; 
	int P     = 1;
	int I   = 5; 
	uint16_t integral_time_base = 0; 
	float integral = 0;
	
	
	int target_vector[] = {0, 20, -40, 20, -50};
	int index = 0; 
	
    while (1) 
    {
		
		
	//	usart_sendln((int)timer_ms()); 
		integral_time_base = timer_ms();
		error = target_vector[index] - tacho;
		SERVO_0_PWM = P * error ; 				
		if(error > 0){
			PORTD |= SERVO_0_DIR_A;
			PORTD &= ~SERVO_0_DIR_B;
		}else if (error < 0){
			PORTD &= ~SERVO_0_DIR_A;
			PORTD |= SERVO_0_DIR_B;
		}else{
		}
		/*1 millisecond has passed */
		if(integral_time_base){
			integral += 0.001 * error; 
			integral_time_base = 0; 
		}

	    if(timer_ms() > 2000){
			timer_reset();
			index++; 
			if(index == 4){
				index = 0;
			}
		}


		


		tacho_values= PIND;
		if (tacho_state == 0){
			if (PIND & TACHO_0_P){
				if(PIND & TACHO_0_M){
					tacho++;
				}else{
					tacho--;
				}
				tacho_state = 1; 
			}
		}else{
			if (!(PIND & TACHO_0_P)){
				tacho_state = 0; 
			}
		}
		}
}

