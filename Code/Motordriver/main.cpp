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
#include "motordriverdefines.h"

/* UART defines */
#define MAX_BUFFER 16

/* Global variables */
uart_io_t g_uart_io;			
uint8_t   g_direction_register;	// 8 bit vector sent into the shift register containing the directions 
StepperMotor g_stepper_motors[2] = {
									StepperMotor(0,0.0857142,STEPPER0_DIR,STEPPER0_STEP),
									StepperMotor(0,0.0143946,STEPPER1_DIR,STEPPER1_STEP) 
									};
ServoMotor   g_servo_motors[3]   = {
								  ServoMotor(&SERVO0_PWM,&g_direction_register,SERVO0_DIRA,SERVO0_DIRB,1),
								  ServoMotor(&SERVO1_PWM,&g_direction_register,SERVO1_DIRA,SERVO1_DIRB,0),
								  ServoMotor(&SERVO2_PWM,&g_direction_register,SERVO2_DIRA,SERVO2_DIRB,0)
								  };
/* f = 1 / T * 10^-6 *100 [hz] */
#define PID_SAMPLE_PERIOD_100Hz 100
#define VELOCITY_SAMPLE_PERIOD_10HZ 1000

/* Asynchronous block that handles the commands sent from the host 
 * When a newlint 
 e is found it is asserted that the command has been sent 
 * NOTE: It is important that the host never sends more than the max buffer size 
 *       because the motor driver does not check for out of bounds 
 */
ISR(USART_RX_vect){
	g_uart_io.data_input[ g_uart_io.byte_count ] = UDR0;
	
	if( g_uart_io.data_input[ g_uart_io.byte_count ]== '\n'){
		g_uart_io.byte_count = 0; 
		g_uart_io.new_command = true;
	}else{
		g_uart_io.byte_count = g_uart_io.byte_count + 1 ; 
	}
}


/*SPI enables as master with no prescaler */
void spi_init(){
	SHIFT_REGISTER |= SHIFT_MOSI | SHIFT_SCK | SHIFT_SS | SHIFT_REFRESH; //SHIFT_SS has to be high for the spi to work. 
	SPCR |=  (1 << SPE) | (1 << MSTR); //Enable spi as master and enable SPI respectively.
}
/*This function sends the direction vector byte containing the direction signals for the L293D Motor driver */
void spi_shift_in_direction(){
	static uint8_t previous_value = 0; 
	if((previous_value != g_direction_register) == 1){
		SHIFT_PORT &= ~SHIFT_REFRESH;
		SPDR = g_direction_register;
		while(!(SPSR & (1<<SPIF)));
		SHIFT_PORT |= SHIFT_REFRESH;
		previous_value = g_direction_register;
	}
}

void spi_clear_register(){
	SHIFT_PORT &= ~SHIFT_REFRESH;
	SPDR = 0;
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
	TCCR0A |= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00); 
	TCCR0B |= (1 << CS02); 
	/* Servo 1 */ 
	TCCR0A |= (1 << COM0A1); 
	DDRD |= SERVO1;
	
	/* Servo 2 */
	TCCR1A |= (1 << COM1A1) | (1 << WGM12) | (1 << WGM10); 
	TCCR1B |= (1 << CS12);
	DDRB |= SERVO2;
	
	/*Stepper motor */
	DDRD |= (STEPPER0_DIR) | (STEPPER0_STEP ) | (STEPPER1_DIR) | (STEPPER1_STEP);  
	

	
}
void disable_motors(){
	
}
void process_stepper_command(char * command,StepperMotor * stepperMotor){
	char * ptr = &command[3]; 
	char status = 'T';
	switch (command[2])
	{
		case 'A':
			stepperMotor->acceleration = atoi(ptr);
			break;
		case 'N':
			stepperMotor->num_steps = atoi(ptr); 
			break;
		case 'T':
			stepperMotor->duration = atoi(ptr);
			break; 
		case 'P':
			stepperMotor->target_pos = atoi(ptr);
			break; 	
		case 'S':
			stepperMotor->start =atoi(ptr);
			break;
		default:
		status = 'F';
		break;
	}	
	usart_sendln(status);
}

void process_servo_command(char * command, ServoMotor * servoMotor){
	char * ptr = &command[3]; 
	char *ptr2 = &command[4];
	char status = 'T';
	switch(command[2]){
		case 'V':
			servoMotor->set_point_velocity = atoi(ptr);
		break;
		case 'P':
			servoMotor->new_position = atoi(ptr);
		break;
		//Tune the PID of the position and velocity; 
		case 'T':
			switch (command[3])
			{
				case 'A':
					servoMotor->Vp_positive = atoi(ptr2)/1000.0;
				break; 
				case 'B':
					servoMotor->Vi_positive = atoi(ptr2)/1000.0; 
				break; 
				case 'C':
					servoMotor->Vp_negative = atoi(ptr2)/1000.0;
				break;
				case 'D':
					servoMotor->Vi_negative = atoi(ptr2)/1000.0; 
				break;
				case 'E':
					servoMotor->Kp = atoi(ptr2)/1000.0;
				break;
				case 'F':
					servoMotor->Ki = atoi(ptr2)/1000.0;
				break;
			}
		case 'r': 
			servoMotor->reset(); 
		break; 
		default:
		status = 'F';
		break;
	}
	usart_sendln(status);
}

int main(void)
{
	spi_init();
	spi_clear_register();
	
	usart_enable(9600);
	timer_enable();
	//while(1);
	
	for(int i = 0; i < 3; i++){
		g_servo_motors[i].reset();
	}
	sei();
	/* Flush the receive buffer of the uart */
	while (UCSR0A & (1<<RXC0)){
		char c  = UDR0;	
	}


	enum robot_state_t  {
		R_WAIT,
		R_START,
		R_RUNNING,
		R_DONE,
		R_ERROR,
		R_STOP,
		}; 
	robot_state_t state = R_WAIT;

	g_uart_io.new_command = false; 
	g_uart_io.byte_count = 0; 
	
	/*Stores the the previous value of the main counter*/
	uint8_t encoder_positions = 0;
	
	/* Variables used to process the command sent from the host */
    char host_command[MAX_BUFFER];
	int  motor_select;
	int is_done =1 ;
	init_motors();
	while (1){
		switch(state){
			case R_WAIT:
				if(g_uart_io.new_command == true){
					ATOMIC_BLOCK(ATOMIC_FORCEON){
						memcpy(host_command,(void*)g_uart_io.data_input,MAX_BUFFER);
						g_uart_io.byte_count = 0; 
						g_uart_io.new_command = false; 
					}
					if(host_command[0] == 'Z'){
						usart_sendln('T');
						state = R_START;
					}else{
						usart_sendln('F');
					}
				}
			break;
			case R_START:
				if(g_uart_io.new_command == true){
					ATOMIC_BLOCK(ATOMIC_FORCEON){
						memcpy(host_command,(void*)g_uart_io.data_input,MAX_BUFFER);
						g_uart_io.byte_count = 0; 
						g_uart_io.new_command = false; 
					}
					if(host_command[0] == 'G'){
						usart_sendln('R');
						g_servo_motors[0].start = 1;
						state = R_RUNNING;
					}else if(host_command[0] == 'T'){
						motor_select =  host_command[1]-48;
						process_stepper_command(host_command,&g_stepper_motors[motor_select]);
					}else if (host_command[0] == 'S'){
						motor_select = host_command[1]-48;
						process_servo_command(host_command,&g_servo_motors[motor_select]);
					}
				}
			break;
			case R_RUNNING:
			is_done = is_done && g_stepper_motors[0].is_done();
			is_done = is_done && g_stepper_motors[1].is_done();
			is_done = is_done && g_servo_motors[0].is_done();
			if(is_done == 1){
				state = R_DONE;
			}
			is_done = 1;
			break;
			case R_DONE:
				usart_sendln('D');
				state = R_START;
			break;
			case R_STOP:
				disable_motors();	
				break;
			break;
		}
		
		if(state == R_RUNNING){
			g_stepper_motors[0].rotate(timer_50k());
			g_stepper_motors[1].rotate(timer_50k());
		}
		if(state == R_RUNNING || state == R_START || state == R_DONE){
			spi_shift_in_direction(); 
			g_servo_motors[0].move(timer_50k());	
		//	g_servo_motors[1].move(timer_10k());
			if(PINC != encoder_positions){
				encoder_positions = PINC;
				g_servo_motors[0].update_encoder_position(encoder_positions& SERVO0_ENCODER_PLUS,encoder_positions & SERVO0_ENCODER_MIN);
				g_servo_motors[1].update_encoder_position(encoder_positions& SERVO1_ENCODER_PLUS,encoder_positions & SERVO1_ENCODER_MIN);
				g_servo_motors[2].update_encoder_position(encoder_positions& SERVO2_ENCODER_PLUS,encoder_positions & SERVO2_ENCODER_MIN);
			}
		}
			
	}
}


