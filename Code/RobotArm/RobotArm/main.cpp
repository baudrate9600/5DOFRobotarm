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

#define STEPPER_REGISTER PORTB
#define M0_STEP (1 << PORTB0)  
#define M0_DIR	(1 << PORTB1)

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
enum parse_fsm {WAIT,MOTOR,SIGN,ANGLE, STOP};	
enum parse_fsm parse_state = WAIT;

struct Motor_status{
	uint8_t motor_select; 
	int angle;
	uint8_t done; 
	};
Motor_status motor_status; 
ISR(USART_RX_vect){
	motor_status.done = 0; 
	char c = UDR0;
	static int counter = 0; 
	static int sign;
	static float angle; 
	/*finit state machine for recieving data frame */
	switch(parse_state){
				case WAIT: 
					if(c == 'M'){
						parse_state =MOTOR;
						angle = 0; 
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
struct Stepper_motor{
	uint8_t start;
	int16_t pos; 
	int16_t target_pos;
	int16_t diff;
	uint8_t dir;
	float f_pos;
	int state; 
	float gear_train;
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
void stepper_rotate(Stepper_motor *motor,uint8_t dir, uint8_t step){
	switch (motor->state){
		case 0:
			if(motor->start == 1){
				motor->state = 1; 
			}	
			break;
		case 1:
			motor->diff = (motor->target_pos - motor->pos)/motor->gear_train;
			motor->state = 2; 
			if(motor->diff > 0){
				STEPPER_REGISTER |= dir;
				motor->dir = 1;
			}else if(motor->diff < 0){
				STEPPER_REGISTER &= ~dir; 
				motor->dir = -1; 
			}
			STEPPER_REGISTER &= ~step; 
			motor->diff = abs(motor->diff); 
			usart_sendln(motor->diff);
			usart_sendln(motor->pos);
			motor->f_pos = motor->pos;
			break; 
		case 2: 
			if(STEPPER_REGISTER & step){
				
				motor->diff--;	
				motor->f_pos=motor->f_pos + motor->gear_train*motor->dir;
				motor->pos = (float)motor->f_pos;
				if(motor->diff == 0){
					motor->state = 0; 
					motor->start = 0;
					motor->pos=motor->target_pos;
				}
			}
			STEPPER_REGISTER ^= step;
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
	int16_t tacho = 0;
	DDRD &= ~TACHO_0_M;
	DDRD &= ~TACHO_0_P; 

	/* Initialize servo motors */
	DDRD |= SERVO_0_EN | SERVO_0_DIR_A | SERVO_0_DIR_B;
	TCCR2A |= (1 << COM2B1 ) | (1 << WGM21) | (1 << WGM20); /*Fast PWM */
	TCCR2B |= (1 << CS20); /*No prescaling */
	/* PID */
	int16_t error = 0;	
	
	Servo_motor servo0 ={0};
	servo0.P= 5; 		
	servo0.I= 0.1; 
	servo0.max_integral = 255; 
	int output = 0; 	
	
	uint32_t curtime = 0; 	
	uint32_t pid_timer = 0; 
	uint32_t stepper_timer = 0;
	
	DDRB |= M0_DIR | M0_STEP;
	Stepper_motor stepper0 = {0};
	stepper0.gear_train = 0.04 ;
	stepper0.pos = 0; 
    while (1) 
    {
		/*character FSM */ 
		char buff[50]; 
		if(motor_status.done == 1){
			sprintf(buff,"Motor %d, angle =  %d\n",motor_status.motor_select,motor_status.angle);
	//		usart_send(buff);
			motor_status.done = 0; 
			switch(motor_status.motor_select){
				case 0 : 
					servo0.target_pos = motor_status.angle; 
					break;
				case 1 :
					stepper0.target_pos = motor_status.angle; 
					stepper0.start = 1; 
					break; 
			}
		}

		if(timer_ms()-curtime > 500){
			curtime = timer_ms(); 
			//sprintf(buff,"Current pos %d, Target pos =  %d, P = %d, I = %d\n",servo0.current_pos,servo0.target_pos,servo0.proportional,servo0.integral);
			//usart_send(buff);
			//usart_sendln(servo_0.current_pos);

			usart_sendln(stepper0.pos);

		}
		//servo0.target_pos=stepper0.pos;		
		if(timer_ms() - pid_timer > 1){
			pid_timer = timer_ms();
			output = servo_pid(&servo0);
			stepper_rotate(&stepper0,M0_DIR,M0_STEP);
		}	
		if(output > 255){
			output = 255; 
		}else if(output < -255){
			output = -255;
		}
		//usart_sendln(temp);	
		servo_rotate(output,&SERVO_0_PWM,SERVO_0_DIR_A,SERVO_0_DIR_B);
		

		tacho_values= PIND;
		if (tacho_state == 0){
			if (PIND & TACHO_0_P){
				if(PIND & TACHO_0_M){
					servo0.current_pos++;
				}else{
					servo0.current_pos--;
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


