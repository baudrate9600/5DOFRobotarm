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


#define DEBUG_
#define S_SCALER 1000UL

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
	long_pulse_width = 0;
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

		/* Compute parameters */
		num_steps = angle*step_to_angle;
		uint16_t vmax= 0.5*(acceleration*duration-sqrt(pow(acceleration*duration,2)-(angle*acceleration*4)));
		t0=(pow(vmax,2)*num_steps)/(2*angle*acceleration);
		t1 =num_steps-t0;
		long_pulse_width =10000* sqrt(2/(acceleration*step_to_angle)) * S_SCALER;
		/* End  computations */

		step_counter  = 0;
		pulse_width_counter  = 1; 
		pulse_width = long_pulse_width / S_SCALER;
		start = 2; 
		#ifdef DEBUG_
		
		usart_sendln("####################"); 
		usart_send("Angle ");usart_sendln((int)angle);  
		usart_send("duration ");usart_sendln((int)duration);
		usart_send("Acceleration ");usart_sendln((int)acceleration);
		usart_send("vmax ");usart_sendln((int)vmax); 			
		usart_send("#n ");usart_sendln((int)num_steps); 			
		usart_send("t0 ");usart_sendln((int)t0); 			
		usart_send("t1 ");usart_sendln((int)t1); 			
		usart_send("Long Step time ");usart_sendln(long_pulse_width);
		usart_send("Step time ");usart_sendln(pulse_width);
		usart_sendln("####################"); 		
		#endif // DEBUG
		stepper_time = current_time; 
	}else if(start == 2){
		fsm(current_time); 
	}

}
stepper_fsm StepperMotor::fsm(uint32_t current_time)
{
	STEPPER_REGISTER |= step_pin;
	/*Generate pulses at a frequency of step time*/
	if((current_time - stepper_time) > pulse_width){
		stepper_time = current_time;
		switch(state){
			/*Linearly accelerate */
			case S_ACCEL:
				long_pulse_width=(long_pulse_width-(2UL*long_pulse_width)/(4UL*pulse_width_counter  +1UL));	
				pulse_width = long_pulse_width / S_SCALER;
			//	usart_sendln(step_counter);	
			
				if( step_counter >=  t0){
					#ifdef DEBUG_
						usart_send("S_ACCEL");
						usart_sendln(pulse_width);
					#endif
					state = S_CONSTANT;
				}	
				pulse_width_counter ++;
				break; 
			/*Rotate at a constant velocity */
			case S_CONSTANT:
				if(step_counter >= t1){
					state = S_DECEL;
					pulse_width_counter =pulse_width_counter *-1; 
					#ifdef DEBUG_
						usart_send("S_CONSTANT ");
						usart_sendln(pulse_width);
					#endif
					}
				break;
			/*Linearly decelerate */
			case S_DECEL:
				long_pulse_width=(long_pulse_width-(2*long_pulse_width)/(4*pulse_width_counter  +1));		
				pulse_width = long_pulse_width / S_SCALER;
			//	usart_sendln(step_counter);	
				if(step_counter >= num_steps){
							start = 0; 
					#ifdef DEBUG_
						usart_send("S_DECEL ");
						usart_sendln(pulse_width);
					#endif
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
