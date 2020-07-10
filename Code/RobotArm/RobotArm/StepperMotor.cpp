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

stepper_fsm StepperMotor::rotate(uint32_t current_time)
{
	switch (state)
	{
		case S_WAIT: 
			if(start == 1){
				state = S_COMPUTE; 
			}
			break; 
		/* Compute the amount of steps for the stepper motor to make */
		case S_COMPUTE: 
			state = S_TURN;
			angle=(target_pos - current_pos);
			 
			if(angle> 0)
			{
				STEPPER_REGISTER |= dir_pin; 
			}else if(angle <0)
			{
				STEPPER_REGISTER &= ~dir_pin; 
				angle = angle * -1;
			}else if(angle == 0)
			{
				state = S_WAIT;
				start = 0; 
			}	
			num_steps = angle*step_to_angle; 
			STEPPER_REGISTER &= ~step_pin;
			stepper_time = current_time;
			/*Computations */
			acceleration = 10;
			time = 5;
			vmax= 0.5*(acceleration*time-sqrt(pow(acceleration*time,2)-(angle*acceleration*4)));
			t1=(pow(vmax,2)*num_steps)/(2*angle*acceleration);
			t0 =num_steps-t1;
			step_time =1000* sqrt((2*step_to_angle)/acceleration);
			/*End Computation */
			counter = 0; 
	    	break; 
		case S_TURN:
		//	usart_sendln("test");
			state = S_TURN;
			if ((current_time - stepper_time) > step_time){
				STEPPER_REGISTER |= step_pin;
				stepper_time = current_time; 
					counter++; 
		//			usart_sendln(num_steps);
					if(counter == num_steps){
						current_pos = target_pos; 
						state = S_WAIT;
						start = 0; 
						}
					if(counter < t1){
						step_time=step_time-(2*step_time)/(4*counter +1);	
					}else if(num_steps < t1){
					}	
					
				STEPPER_REGISTER &= ~step_pin;
			}
			break;	
	}
	return state;
}

// default destructor
StepperMotor::~StepperMotor()
{
} //~StepperMotor
