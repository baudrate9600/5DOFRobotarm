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
#include <math.h>
#include "Usart.h"
#include "timer.h"

#define TACHO_0_P (1 << PORTD2)
#define TACHO_0_M (1 << PORTD5)
#define SERVO_0_EN (1 << PORTD3)
#define SERVO_0_DIR_A (1 << PORTD6)
#define SERVO_0_DIR_B (1 << PORTD7) 

#define SERVO_0_PWM OCR2B

#define SERVO_REGISTER PORTD 
#define M0_STEP (1 << PORTC0)  
#define M0_DIR	(1 << PORTC1)

#define BUFFER_SEND_SIZE = 16  
#define BUFFER_SIZE 32



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

/*Servo Control */ 
void servo_rotate(int enable,int error,volatile uint8_t * pwm, uint8_t dir_a,uint8_t dir_b){
	switch(enable){
		case 1 :
			if (error > 0){
				SERVO_REGISTER |= dir_a  ;
				SERVO_REGISTER &= ~dir_b;
			}else if (error < 0){
				SERVO_REGISTER &= ~dir_a;
				SERVO_REGISTER |= dir_b;
			}
			*pwm = error;
			break; 
		case 0 : 
			* pwm = 0; 
			
	}	
}

ISR(USART_TX_vect){
	//if(usart_fifo_counter > 0){
	//	UDR0 = usart_	
	//}	
}
volatile char received_characters[BUFFER_SIZE]; 
volatile uint8_t received_index = 0; 
volatile bool received_byte= false;
enum parse_fsm {WAIT,MOTOR,SIGN,ANGLE, STOP};	
enum parse_fsm parse_state = WAIT;

struct Motor_status{
	uint8_t motor_select; 
	int16_t angle;
	uint8_t done; 
	};
Motor_status motor_status; 
ISR(USART_RX_vect){
	motor_status.done = 0; 
	char c = UDR0;
	static int counter = 0; 
	static int sign;
	static int angle; 
	/*finit state machine for recieving data frame */
	switch(parse_state){
				case WAIT: 
					if(c == 'M'){
						parse_state =MOTOR;
						angle = 1; 
					}
					break;
				case MOTOR: 
					motor_status.motor_select = c-48; 
					parse_state = SIGN;
					break; 
				case SIGN:
					parse_state = ANGLE;
					if(c == '-'){
						sign = -1; 	
					}else if(c == '+'){
						sign = 1; 		
					}else{
						parse_state = WAIT;
					}
					counter = 3;
					break; 
				case ANGLE: 
					/*5328 is the conversion from ascii to int: 48*100+48*10+48=5328*/
					/*Todo instead of sending the numbers in ascii send it in raw binary*/
					counter--;
					angle += (c-48)*pow(10,counter);  
					if(counter == 0){
						motor_status.angle = angle * sign;
						parse_state = WAIT; 
						motor_status.done = 1; 
					}
					break; 
		}	
		

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
	
	int P     = 1;
	int I   = 5; 
	uint16_t integral_time_base = 0; 
	float integral = 0;
	
	
	
    while (1) 
    {
		/*character FSM */ 
		char buff[50]; 
		if(motor_status.done == 1){
			sprintf(buff,"Motor %d, angle =  %d\n",motor_status.motor_select,motor_status.angle);
			usart_send(buff);
			motor_status.done = 0; 
		}
				
		error = P *(motor_status.angle*10 - tacho);
		if(error > 255){
			error = 255; 
		}
		if (error != 0){
			servo_rotate(1,error,&SERVO_0_PWM,SERVO_0_DIR_A,SERVO_0_DIR_B);
		}else{
			SERVO_0_PWM = 0; 
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

