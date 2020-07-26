/*
 * RobotArm.cpp
 * Description:
 *	This is the motor driver of the robot arm. The motor driver controls 6 motors of which 2 are 
 *  stepper motors, 3 are servo motors and 1 is for the end effector. 
 *  	
 * Created: 6/12/2020 8:42:41 PM
 * Author : Olasoji Makinwa 
 *			 _______
 * RESET ---|  |_|	|--- PC5            | PD0	| RX			|
 * PD0   ---|		|--- PC4			| PD1   | TX			|
 * PD1   ---|		|--- PC3			| PD2	| DIR stepper 0 |
 * PD2   ---| Atmega|--- PC2			| PD3   | OC2B motor 0  | 
 * PD3   ---|328p   |--- PC1			| PD4   | Step stepper 0|
 * PD4   ---|		|--- PC0			| PB6	| Crystal		|
 * VCC   ---|		|--- GND			| PB7   | Crystal		|
 * GND   ---|       |--- AREF			| PD5	| 0C0B servo 1  |
 * PB6   ---|		|--- AVCC			| PD6	| 0C0A servo 2  |
 * PB7   ---|		|--- PB5			| PD7   | RCLK			|
 * PD5   ---|		|--- PB4			| PB0	| DIR stepper 1 | 
 * PD6   ---|		|--- PB3			| PB1   | OC1A servo 3  |
 * PD7   ---|		|--- PB2			| PB2	| Step stepper 1|
 * PB0   ---|_______|--- PB1			| PB3	| MOSI			| 
										| PB4   | MISO			|
 *										| PB5   | SCK			| 
										| PC0	| TACHO_A servo0|
										| PC1   | TACHO_B servo0|
										| PC2   | TACHO_A servo1|
									    | PC3   | TACHO_B servo1| 
										| PC4   | TACHO_A servo2|
										| PC5   | TACHO_B servo2|
|shift register
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
#include <math.h>
#include "Usart.h"
#include "timer.h"
#include "StepperMotor.h"
#include "ServoMotor.h"

/*Shift register defines */
#define SHIFT_MOSI (1 << PORTB3)
#define SHIFT_SS   (1 << PORTB2)
#define SHIFT_SCK  (1 << PORTB5) 
#define SHIFT_REFRESH (1 << PORTD7)

/*Servo motor defines */
#define ENDEFF_DIRA (1 << 0); 
#define ENDEFF_DIRB (1 << 1);
#define SERVO0_DIRA (1 << 2); 
#define SERVO0_DIRB (1 << 3); 
#define SERVO1_DIRA (1 << 4); 
#define SERVO1_DIRB (1 << 5); 
#define SERVO2_DIRA (1 << 6); 
#define SERVO2_DIRB (1 << 7); 







#define SERVO0_PWM OCR2B

#define SERVO_REGISTER PORTD 

#define M0_STEP (1 << PORTB0)  
#define M0_DIR	(1 << PORTB1)
#define M1_STEP (1 << PORTB2) 
#define M1_DIR  (1 << PORTB3)


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
ISR(USART_RX_vect){
	motor_status.done = 0; 
	char c = UDR0;
	static int counter = 0; 
	static int sign;
	/*finite state machine for receiving data frame */
	switch(receive_state){
				case RECEIVE_WAIT : 
					if(c == 'M'){
						receive_state = RECEIVE_MOTOR_SELECT;
					}else if(c== 'R'){
						receive_state = RECEIVE_RESET;
					}
					break;
				case RECEIVE_MOTOR_SELECT: 
					motor_status.motor_select = c-48; 
					receive_state = RECEIVE_DATA;
					counter = 0;
					break; 
				case RECEIVE_DATA:
					motor_status.data[counter] = c;
					counter++;
					if(counter == 8){
						receive_state = RECEIVE_WAIT ; 
						motor_status.done = 1;
					}
				
					break; 
				case RECEIVE_RESET:
							
					break;
		}	
}

/*Struct to keep the current state of the directions and the previous, this is used so that the 
 * shift register only has to be updated if direction_vector XOR previous_direction > 0 */
struct Direction_signal{
	uint8_t direction_vector;
	uint8_t previous_direction;
	};
Direction_signal  direction_signal;

void spi_init(){
	DDRB |= SHIFT_MOSI | SHIFT_SCK | SHIFT_SS; //SHIFT_SS has to be high for the spi to work. 
	DDRD |= SHIFT_REFRESH; //Enable shift refresh pin 
	SPCR |=  (1 << SPE) | (1 << MSTR); //Enable spi as master and enable SPI respectively.
}
/*This function sends the direction vector byte containing the direction signals for the L293D Motor driver */
void spi_send_direction(){
	PORTD &= ~SHIFT_REFRESH;
	SPDR = direction_signal.direction_vector;
	while(!(SPSR & (1<<SPIF)));
	PORTD |= SHIFT_REFRESH;
}
int main(void)
{
	/* Initialize SPI:	
	 *	the SPI is used to send the direction signals to the shift register */
	 spi_init();
	/* Initialize servo motors: 
	/* Servo 0 0C2B */
	TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); //Enable 0C2B pin as pwm and 
	TCCR2B |= (1 << CS20); // No prescaling 

	
	while (1){
    
		
		

		
	}
}


