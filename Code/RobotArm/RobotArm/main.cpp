/*
 * RobotArm.cpp
 * Description:
 *	This is the motor driver of the robot arm. The motor driver controls 6 motors of which 2 are 
 *  stepper motors, 3 are servo motors and 1 is for the end effector. 
 *  	
 * Created: 6/12/2020 8:42:41 PM
 * Author : Olasoji Makinwa 
 *			_________
shift register
|QA	| DIR_A servo 0|
|QB | DIR_B servo 0|
|QC | DIR_A servo 1| 
|QD | DIR_B servo 1|
|QE	| DIR_A servo 2|
|QF | DIR_B servo 2| 
|QG | DIR_A servo 3| 
|QH | DIR_B servo 3|

 */ 
//#define DEBUG_
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/atomic.h>
#include <string.h>
#include <math.h>
#include "Usart.h"
#include "timer.h"
#include "StepperMotor.h"
#include "ServoMotor.h"

/* Shift register defines */
#define SHIFT_REGISTER DDRB //No pun intended.. 
#define SHIFT_PORT  PORTB
#define SHIFT_MOSI (1 << PORTB3)
#define SHIFT_SS   (1 << PORTB2)
#define SHIFT_SCK  (1 << PORTB5) 
#define SHIFT_REFRESH (1 << PORTB0)

#define ENDEFF_DIRA (1 << 0); 
#define ENDEFF_DIRB (1 << 1);
#define ENDEFF_PWM  OCR1B
/* Servo Motor defines */
#define SERVO0		(1 << PORTD2)
#define SERVO0_DIRA (1 << 2)
#define SERVO0_DIRB (1 << 3) 
#define SERVO0_TACHO_PLUS (1 << PORTC0)
#define SERVO0_TACHO_MIN  (1 << PORTC1)
#define SERVO0_PWM	OCR0B

#define SERVO1_DIRA (1 << 4) 
#define SERVO1_DIRB (1 << 5) 
#define SERVO2_DIRA (1 << 6) 
#define SERVO2_DIRB (1 << 7) 

/* Stepper motor defines */ 

#define STEPPER0_DIR (1 << PORTD2)
#define STEPPER0_STEP (1 << PORTD4)

#define STEPPER1_DIR (1 << PORTD7) 
#define STEPPER1_STEP (1 << PORTD3)



/* UART defines */
#define MAX_BUFFER 16




/* States of the receive FSM 
 * */
typedef enum  {
				RECEIVE_WAIT ,
				RECEIVE_MOTOR_SELECT,
				RECEIVE_DATA,
				RECEIVE_RESET,
				RECEIVE_DONE 
				}receiver_fsm;	
receiver_fsm receive_state = RECEIVE_WAIT ;

//int parse_data[16] = 0; 
struct Motor_status{
	uint8_t motor_select; 
	int data[8];
	int done;
	};
Motor_status motor_status; 
/*Handle the commands */
volatile  char data_rx[MAX_BUFFER];
volatile uint8_t rx_count;
volatile bool new_command = false;
/* Receive all the data until a newline is found */
ISR(USART_RX_vect){
	data_rx[rx_count] = UDR0;
	if(data_rx[rx_count] == '\n'){
		rx_count = 0; 
		new_command = true;
	}else{
		rx_count++;
	}
}

/*Struct to keep the current state of the directions and the previous, this is used so that the 
 * shift register only has to be updated if direction_vector XOR previous_direction > 0 */
struct Direction_signal{
	uint8_t direction;
	uint8_t previous_direction;
	};
Direction_signal  direction_signal;

/*SPI enables as master with no prescaler */
void spi_init(){
	SHIFT_REGISTER |= SHIFT_MOSI | SHIFT_SCK | SHIFT_SS | SHIFT_REFRESH; //SHIFT_SS has to be high for the spi to work. 
	SPCR |=  (1 << SPE) | (1 << MSTR); //Enable spi as master and enable SPI respectively.
}
/*This function sends the direction vector byte containing the direction signals for the L293D Motor driver */
void spi_send_direction(){
	SHIFT_PORT &= ~SHIFT_REFRESH;
	SPDR = direction_signal.direction;
	while(!(SPSR & (1<<SPIF)));
	SHIFT_PORT |= SHIFT_REFRESH;
}

void init_motors(){
		/* Initialize motors: 
	/* End effector */
	TCCR1A |= (1 << COM1B1) | (1 << WGM11) | (1 << WGM10); //Enable 0C2B pin as pwm and 
	TCCR1B |= (1 << CS12); //PWM frequency of 62,500 hz 
	/* Servo 0 */
	DDRD |= SERVO0;
	TCCR0A |= (1 << COM0B0) | (1 << WGM01) | (1 << WGM00); 
	TCCR0B |= (1 << CS02); 
	/* Servo 1 */ 
	TCCR0A |= (1 << COM0A0) | (1 << WGM01) | (1 << WGM00); 
	/* Servo 2 */
	TCCR1A |= (1 << COM1A1) | (1 << WGM12) | (1 << WGM10); 
	
	/*Stepper motor */
	DDRD |= (STEPPER0_DIR) | (STEPPER0_STEP ) | (STEPPER1_DIR) | (STEPPER1_STEP);  

}
void process_stepper_command(char * command,StepperMotor * stepperMotor){
	char * ptr = &command[2]; 
	switch (command[1])
	{
		case 'a':
			stepperMotor->acceleration = atoi(ptr);
			usart_sendln(stepperMotor->acceleration); 
			break;
		default:
		/* Your code here */
		break;
	}	
}
int main(void)
{
	/* Initialize SPI:	*/
	 spi_init();
	
	/* Init motors */	
	init_motors();
	ServoMotor servo0(&SERVO0_PWM,&direction_signal.direction,SERVO0_DIRA,SERVO0_DIRB);
	servo0.set_pid(1,1,0);

	StepperMotor stepper_motors[2] = { StepperMotor(0,0.2571426,STEPPER0_DIR,STEPPER0_STEP),
									   StepperMotor(0,0.043182,STEPPER1_DIR,STEPPER1_STEP) };

	/* Clear shift register */
	direction_signal.direction = SERVO0_DIRB;
	direction_signal.previous_direction = 0; 	
	spi_send_direction();	

	usart_enable(9600);
	timer_enable();
	sei();

	uint32_t oldtime=0;
	uint8_t capture_tacho;
    char command[MAX_BUFFER];

	while (1){
		/*Read the command and assert that new commands can be read */
		if(new_command == true){
			ATOMIC_BLOCK(ATOMIC_FORCEON){
				memcpy(command,(void*)data_rx,MAX_BUFFER);
			}
			rx_count = 0; 
			new_command = false; 
			switch (command[0])
			{
				//s(T)epper motor 
				case 'T':
					process_stepper_command(command,&stepper_motors[0]);					
					break;
				case 'a':
					break;
				case 'd':
					break;
				
				break;
			}
		}
			
			
		
	
			
			
	//	servo0.rotate(timer_10k());
		
		capture_tacho = PINC;
		servo0.tacho(capture_tacho & SERVO0_TACHO_PLUS,capture_tacho & SERVO0_TACHO_MIN);
		
		
		if(direction_signal.previous_direction != direction_signal.direction){
			spi_send_direction();
			direction_signal.previous_direction = direction_signal.direction;
		}
		if(timer_10k() - oldtime > 100){
			oldtime= timer_10k();
		}	
	}
}


