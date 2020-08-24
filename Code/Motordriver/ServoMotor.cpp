/* 
* ServoMotor.cpp
*
* Created: 7/15/2020 8:57:58 PM
* Author: sojim
*/

/************************************************************************/
/* PID FLOAT IMPLEMENTATION                                             */
/************************************************************************/
#include "ServoMotor.h"
#include "Usart.h"
#include <avr/io.h>	
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Usart.h"

#define POSITION_LOOP_SAMPLE_TIME 5000 
#define VELOCITY_LOOP_SAMPLE_TIME 10000
#define BRAKING_RANGE 20
extern int16_t g_delta_time_encoder;
ServoMotor::ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register_ ,uint8_t dir_a_,uint8_t dir_b_,int polarity)
{
	servo_pwm = pwm; 
	servo_register = servo_register_;	
	dir_a = dir_a_;
	dir_b = dir_b_;
	Vi_saturation = 255;
	Ki_saturation = 255;
	servo_state = SE_WAIT;
	if(polarity == 0){
		a = SE_POSITIVE;
		b = SE_NEGATIVE;
	}else if(polarity == 1){
		a = SE_NEGATIVE;
		b = SE_POSITIVE;
	}
} //ServoMotor


/* this function is called at fixed intervals and computes the output of the control */
float ServoMotor::pid_position(){
	int16_t error = set_point_position - encoder_position;
	 /* Proportional error */
	float Kp_error = error * Kp;
	Ki_error += error;  
	if(Ki_error > Ki_saturation){
		Ki_error = Ki_saturation;
	}else if(Ki_error < -Ki_saturation){
		Ki_error = -Ki_saturation;
	}
	Ki_error = Ki_error * Ki;
	float Kd_error = (error - last_error) * Kd;
	last_error = error;
	return Kp_error + Ki_error + Kd_error;
	
}

float ServoMotor::pi_velocity(){
	update_encoder_speed();
	int16_t velocity_error = set_point_velocity-encoder_velocity;
	/* Proportional gain */
	float Vp_error = velocity_error * Vp;

	/* Integral gain */
	Vi_error += velocity_error;

	/* Anti windup */
	if(Vi_error > Vi_saturation){
	Vi_error = Vi_saturation;
	}else if(Vi_error < -Vi_saturation){
	Vi_error = -Vi_saturation;
	}

	Vi_error = Vi_error * Vi ;
	float output = Vp_error + Vi_error + last_pi_velocity_output;
	last_pi_velocity_output = output;	
	return output ;
	
}


void ServoMotor::set_pwm(float val){
		int16_t output = (int16_t)val; 
	
		/*Change direction if output is less than zero */
		if(output > 0){
			*servo_register |= dir_a  ;
			*servo_register &= ~dir_b;
		}else{
			*servo_register &= ~dir_a;
			*servo_register |= dir_b;
			output = output * -1;
		}
		/* Limit the maximum output */ 	
		if(output > 255){
			output = 255;
		}	
		*servo_pwm = output;
}

bool ServoMotor::sample_position_loop(uint32_t current_time){
	if(current_time - sample_time_position > POSITION_LOOP_SAMPLE_TIME){
		sample_time_position = current_time;
		return true;
	}else{
		return false; 
	}
}
bool ServoMotor::sample_velocity_loop(uint32_t current_time){
	if(current_time - sample_time_velocity > VELOCITY_LOOP_SAMPLE_TIME){
		sample_time_velocity = current_time;
		return true;
	}else{
		return false; 
	}
}
/* When the joint has the oppose the force of gravity more torque is required 
   so different gains need to be used */
void ServoMotor::gain_schedule(){
	if(servo_state == a){
		Vp =  Vp_positive;
		Vi =  Vi_positive;		
	}else if(servo_state == b){
		Vp =  Vp_negative;
		Vi =  Vi_negative;		
	}
}

//Feed foward loop
void ServoMotor::move(uint32_t current_time ){	
	switch (servo_state)
	{
		case SE_WAIT:
			if(sample_position_loop(current_time) == true){
				set_pwm(pid_position());
			}
			done = true;
			if(start == 1){
				start = 0;
				done = false;
				/* Determine the direction the robot joint should turn and 
				   set the polarity of the motor */
				if(new_position > encoder_position){
					*servo_register |= dir_a  ;
					*servo_register &= ~dir_b;
					servo_state = SE_POSITIVE;
				}else{
					*servo_register &= ~dir_a  ;
					*servo_register |= dir_b;
					servo_state = SE_NEGATIVE;
				}
				gain_schedule();
			}
		break;
		case SE_POSITIVE:
			if(encoder_position >= (new_position-BRAKING_RANGE)){
				set_point_position = new_position;
				servo_state = SE_WAIT;
				break;
			}
			if(sample_velocity_loop(current_time) == true){
				float output = pi_velocity();
				*servo_register |= dir_a  ;
				if(output > 255){
					 output = 255;
				}else if(output < 0){
					//brake 
					*servo_register &= dir_a  ;
					output = 0;
				}
				*servo_pwm = (uint8_t) output;
			}	
		
		break; 
		case SE_NEGATIVE:
			if(encoder_position <= (new_position+BRAKING_RANGE)){
				set_point_position = new_position;
				servo_state = SE_WAIT;	
				break;
			}
			if(sample_velocity_loop(current_time) == true){
				float output = pi_velocity();
				*servo_register |= dir_b  ;
				if(output > 255){
					 output = 255;	
				}else if(output < 0){
					//brake 
					*servo_register &= ~dir_b  ;
					 output = 0; 	
				}
				*servo_pwm = (uint8_t) output;
			}	
			
		break;
	}
}
/*Updates the absolute position of the encoder in the motor*/
void ServoMotor::update_encoder_position(uint8_t plus, uint8_t min){
	
	if(encoder_rising_edge  == 0){
		/* If plus goes high and min is also high it went fowards 
		 * else it went backwards */
		if(plus){
			if(min){
				encoder_position--;
			}else{
				encoder_position++;
			}
			encoder_rising_edge = 1; 
		}
	}else{
		if(!plus){
			encoder_rising_edge  = 0; 
		}
	}
}

void ServoMotor::update_encoder_speed() {
	 encoder_velocity = abs(encoder_position -  last_encoder_position)*5;
	 last_encoder_position = encoder_position;
}
void ServoMotor::reset(){
	set_point_position = 0; 
	encoder_position = 0; 
	set_point_velocity = 0;
	servo_state = SE_WAIT;
}
bool ServoMotor::is_done(){
	return done;
}
// default destructor
ServoMotor::~ServoMotor()
{
} //~ServoMotor
