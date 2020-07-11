/* 
* StepperMotor.cpp
*
* Created: 7/7/2020 7:52:01 PM
* Author: sojim
*/


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Usart.h"
#include "StepperMotor.h"

/* Iniatialize the stepper motor */
StepperMotor::StepperMotor(int16_t current_pos, float step_to_angle, uint8_t dir_pin, uint8_t step_pin)
{
	state = S_WAIT; 
	this->current_pos	= current_pos;
	/*this value should ideally be an integer */
	this->step_to_angle	= 1.0f/step_to_angle; 
	this->dir_pin		= dir_pin;
	this->step_pin		= step_pin;		
	stepper_time = 0;
	step_time = 0;
} 



/*
   v
	|       ________________________________   - - - - - vmax 
	|      /|							   |\
	|     /	|							   | \
	|    /	|							   |  \
	|   /	|							   |   \ --------- slop=a
	|  /	|							   |	\
	| /		|							   |	 \
	0       t0                             t1     stop 	
	 ------------------------------------------------ t
*/

int StepperMotor::rotate(uint32_t current_time){
	if(start == 1){
		state = S_ACCEL;
		angle=(target_pos - current_pos);
		if(angle> 0){
			STEPPER_REGISTER |= dir_pin; 
		}else if(angle <0){
			STEPPER_REGISTER &= ~dir_pin; 
			angle = angle * -1;
		}else if(angle == 0){
			state = S_WAIT;
			start = 0; 
		}
		/*Compute parameters */
			acceleration = 10;
			num_steps = angle*step_to_angle;
			time = 3;
			vmax= 0.5*(acceleration*time-sqrt(pow(acceleration*time,2)-(angle*acceleration*4)));
			t0=(pow(vmax,2)*num_steps)/(2*angle*acceleration);
			t1 =num_steps-t0;
			step_time =1000* sqrt((2*step_to_angle)/acceleration);
			//sprintf(buff,"vmax=%d,#n=%d,t0=%d,t1=%d,st=%d",vmax,num	
			counter  = 0;
			velocity_counter = 1; 
			start = 2; 
			usart_sendln("####################"); 
			usart_send("vmax ");usart_sendln((int)vmax); 			
			usart_send("#n ");usart_sendln((int)num_steps); 			
			usart_send("t0 ");usart_sendln((int)t0); 			
			usart_send("t1 ");usart_sendln((int)t1); 			
			usart_send("Step time ");usart_sendln((int)step_time);
			usart_sendln("####################"); 	
	}else if(start == 2){
			fsm(current_time); 
	}

}
stepper_fsm StepperMotor::fsm(uint32_t current_time)
{
	STEPPER_REGISTER |= step_pin;
	/*Generate pulses at a frequency of step time*/
	if(current_time - stepper_time > step_time){
		stepper_time = current_time;
		switch(state){
			case S_ACCEL:
				step_time=(int)step_time-(2*(int)step_time)/(4.0*velocity_counter +1);		
				if(counter ==  t0){
					usart_send("STATE : ACCEL ");usart_sendln((int)counter);
					usart_send("Pulse time ");usart_sendln((int)step_time); 
					state = S_CONSTANT;
				}	
				velocity_counter++;
				break; 
			case S_CONSTANT:
				if(counter == t1){
					usart_send("STATE : CONSTANT ");usart_sendln((int)counter); 
					usart_send("Pulse time ");usart_sendln((int)step_time); 
					state = S_DECEL;
					velocity_counter=velocity_counter*-1; 
					usart_sendln(velocity_counter);
				}
				break;
			case S_DECEL:
				step_time=(step_time-(2*step_time)/(4*velocity_counter +1));		
				if(counter == num_steps){
					usart_send("STATE : Decel ");usart_sendln((int)counter);
					usart_send("Pulse time ");usart_sendln((int)step_time); 
					start = 0; 
				}
				velocity_counter++;
				break;
		}	
		STEPPER_REGISTER &= ~step_pin;
		counter++; 
	}
	
	
	return state;
}

// default destructor
StepperMotor::~StepperMotor()
{
} //~StepperMotor
