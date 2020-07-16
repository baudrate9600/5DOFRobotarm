/*
 * RobotArm.cpp
 * This code controlles stepper motors for a robot arm
 * Created: 6/12/2020 8:42:41 PM
 * Author : Olasoji Makinwa 
 */ 
//#define DEBUG_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Usart.h"
#include "timer.h"
#include "StepperMotor.h"
#include "ServoMotor.h"

#define SERVO_TACHO_0_P (1 << PORTD2)
#define SERVO_TACHO_0_M (1 << PORTD5)
#define SERVO_0_EN (1 << PORTD3)
#define SERVO_0_DIR_A (1 << PORTD6)
#define SERVO_0_DIR_B (1 << PORTD7) 

#define SERVO0_PWM OCR2B

#define SERVO_REGISTER PORTD 

#define M0_STEP (1 << PORTB0)  
#define M0_DIR	(1 << PORTB1)
#define M1_STEP (1 << PORTB2) 
#define M1_DIR  (1 << PORTB3)

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



ISR(USART_TX_vect){
	//if(usart_fifo_counter > 0){
	//	UDR0 = usart_	
	//}	
}
volatile char received_characters[BUFFER_SIZE]; 
volatile uint8_t received_index = 0; 
volatile bool received_byte= false;
typedef enum  {P_WAIT,
			   P_MOTOR_SELECT,
			   P_DATA,
			   P_DONE }parse_fsm;	
parse_fsm parse_state = P_WAIT;

//int parse_data[16] = 0; 
struct Motor_status{
	uint8_t motor_select; 
	int data[8];
	int done;
	};
Motor_status motor_status; 
ISR(USART_RX_vect){
	motor_status.done = 0; 
	char c = UDR0;
	static int counter = 0; 
	static int sign;
	/*finit state machine for recieving data frame */
	switch(parse_state){
				case P_WAIT: 
					if(c == 'M'){
						parse_state = P_MOTOR_SELECT;
					}
					break;
				case P_MOTOR_SELECT: 
					motor_status.motor_select = c-48; 
					parse_state = P_DATA;
					counter = 0;
					break; 
				case P_DATA:
					motor_status.data[counter] = c;
					counter++;
					if(counter == 8){
						parse_state = P_WAIT; 
						motor_status.done = 1;
					}
				
					break; 
				case P_DONE:
					parse_state = P_WAIT;
						

		}	
		

}

struct Servo_motor{
	char id;
	int current_pos;
	int target_pos;
	int16_t max_integral;
	int32_t integral;
	//int16_t error;
	float derror; 
	int16_t proportional;
	int P; 
	float I; 
	int D; 
	};
float servo_pid(Servo_motor * motor){
		float error = (motor->target_pos- 0.2f*motor->current_pos);


		int16_t derivative =  (int)(error - motor->derror);
		motor->derror = error;
		int32_t temp = motor->integral;
		temp+= error; 
		if(abs(temp)<motor->max_integral){
			motor->integral = temp; 
		}
		motor->proportional = motor->P*error;
		
		return motor->P*error +motor->I*motor->integral;
}
/*Servo Control */
void servo_rotate(float val,volatile uint8_t * pwm, uint8_t dir_a,uint8_t dir_b){
	int speed = (float)val;
	if (speed > 0){
		SERVO_REGISTER |= dir_a  ;
		SERVO_REGISTER &= ~dir_b;
		}else{
		SERVO_REGISTER &= ~dir_a;
		SERVO_REGISTER |= dir_b;
		speed *= -1;
	}
	*pwm = speed;
}

/*some comment*/
int main(void)
{

	
	/* initialize component */ 
	sei();
	timer_enable();	
	usart_enable(9600);
	/* Initialize motors */ 
	
    /* Replace with your application code */
	uint8_t tacho_values;
	uint8_t old_tacho_values;
	DDRD &= ~SERVO_TACHO_0_M;
	DDRD &= ~SERVO_TACHO_0_P; 

	/* Initialize servo motors */
	DDRD |= SERVO_0_EN | SERVO_0_DIR_A | SERVO_0_DIR_B;
	TCCR2A |= (1 << COM2B1 ) | (1 << WGM21) | (1 << WGM20); /*Fast PWM */
	TCCR2B |= (1 << CS20); /*No prescaling */
	/* PID */
		
	DDRB |= M0_DIR | M0_STEP | M1_STEP | M1_DIR;
	StepperMotor stepper0(0,0.043182,M0_DIR,M0_STEP);
	StepperMotor stepper1(0,0.2571426, M1_DIR,M1_STEP); 
	ServoMotor  servo0(&SERVO0_PWM,&SERVO_REGISTER,SERVO_0_DIR_A,SERVO_0_DIR_B);
	servo0.target_pos = 0;
	servo0.set_pid(10,0,0);
	uint32_t temp = 0 ;
    while (1) 
    {
		
		/*character FSM */ 
		char buff[50]; 
		if(motor_status.done == 1){
	//		sprintf(buff,"Motor %d, angle =  %d\n",motor_status.motor_select,motor_status.angle);
	//		usart_send(buff);
			motor_status.done = 0; 
			uint16_t duration = (motor_status.data[0]-48)*10 + (motor_status.data[1]-48);
			uint16_t acceleration = (motor_status.data[2]-48)*10 + (motor_status.data[3]-48);
			int16_t angle = (motor_status.data[5]-48)*100 + (motor_status.data[6]-48)*10 + (motor_status.data[7]-48);

			if(motor_status.data[4] == '-'){
				angle = angle * -1;
			}
			switch(motor_status.motor_select){
				case 0 : 
					//usart_send("hello");
					servo0.target_pos = angle; 
					break;
				case 1 :
					stepper0.target_pos = angle; 
					stepper0.duration = duration;
					stepper0.acceleration = acceleration;
					stepper0.start = 1; 
					break; 
				case 2 : 
					stepper1.target_pos = angle; 
					stepper1.duration = duration;
					stepper1.acceleration = acceleration;
					stepper1.start = 1;
					break;
			}
		}

			//usart_sendln(temp);	
		
		stepper0.rotate(timer_10k());
		stepper1.rotate(timer_10k());
		servo0.rotate(timer_10k());
	
		tacho_values= PIND; 
		servo0.tacho(PIND & SERVO_TACHO_0_P, PIND & SERVO_TACHO_0_M);
		/*Check if any bits have toggled */
		if(tacho_values ^ old_tacho_values){
			old_tacho_values = tacho_values;
			
		}
		if(timer_10k()-temp > 100){
		//	usart_sendln()
		//usart_sendln(servo0.absolute_position);
		}
		
	}
}


