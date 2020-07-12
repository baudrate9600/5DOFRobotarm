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
	pulse_width = 0;
} 




int StepperMotor::rotate(uint32_t current_time){
	if(start == 1){
		state = S_ACCEL;
		int16_t angle=(target_pos - current_pos);
		current_pos = target_pos;
		if(angle> 0){
			STEPPER_REGISTER |= dir_pin; 
		}else if(angle <0){
			STEPPER_REGISTER &= ~dir_pin; 
			angle = angle * -1;
		}else if(angle == 0){
			state = S_WAIT;
			start = 0; 
		}

		acceleration = 1;
		duration = 10;
		/* Compute parameters */
		num_steps = angle*step_to_angle;
		uint16_t vmax= 0.5*(acceleration*duration-sqrt(pow(acceleration*duration,2)-(angle*acceleration*4)));
		t0=(pow(vmax,2)*num_steps)/(2*angle*acceleration)-1;
		t1 =num_steps-t0-2;
		num_steps--;
		pulse_width =10000* sqrt(2/(acceleration*step_to_angle));
		/* End  computations */

		step_counter  = 0;
		pulse_width_counter  = 1; 
		start = 2; 
		
		usart_sendln("####################"); 
		usart_send("vmax ");usart_sendln((int)vmax); 			
		usart_send("#n ");usart_sendln((int)num_steps); 			
		usart_send("t0 ");usart_sendln((int)t0); 			
		usart_send("t1 ");usart_sendln((int)t1); 			
		usart_send("Step time ");usart_sendln((int)pulse_width);
		usart_sendln("####################"); 	
	}else if(start == 2){
		fsm(current_time); 
	}

}
stepper_fsm StepperMotor::fsm(uint32_t current_time)
{
	STEPPER_REGISTER |= step_pin;
	/*Generate pulses at a frequency of step time*/
	if(current_time - stepper_time > pulse_width){
		stepper_time = current_time;
		switch(state){
			case S_ACCEL:
				pulse_width=(int)pulse_width-(2*(int)pulse_width)/(4.0*pulse_width_counter  +1);		
				if(step_counter ==  t0){
					//usart_send("STATE : ACCEL ");usart_sendln((int)step_counter);
					//usart_send("Pulse time ");usart_sendln((int)pulse_width); 
					state = S_CONSTANT;
				}	
				pulse_width_counter ++;
				break; 
			case S_CONSTANT:
				if(step_counter == t1){
					//usart_send("STATE : CONSTANT ");usart_sendln((int)step_counter); 
					//usart_send("Pulse time ");usart_sendln((int)pulse_width); 
					state = S_DECEL;
					pulse_width_counter =pulse_width_counter *-1; 
					usart_sendln(pulse_width_counter );
				}
				break;
			case S_DECEL:
				pulse_width=(pulse_width-(2*pulse_width)/(4*pulse_width_counter  +1));		
				if(step_counter == num_steps){
					//usart_send("STATE : Decel ");usart_sendln((int)step_counter);
					//usart_send("Pulse time ");usart_sendln((int)pulse_width); 
					start = 0; 
				}
				pulse_width_counter ++;
				break;
		}	
		STEPPER_REGISTER &= ~step_pin;
		step_counter++; 
	}
	
	
	return state;
}

// default destructor
StepperMotor::~StepperMotor()
{
} //~StepperMotor
