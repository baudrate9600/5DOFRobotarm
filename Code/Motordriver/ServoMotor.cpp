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

#define POSITION_LOOP_SAMPLE_TIME 100 
#define VELOCITY_LOOP_SAMPLE_TIME 2000
extern int16_t g_delta_time_encoder;
ServoMotor::ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register_ ,uint8_t dir_a_,uint8_t dir_b_)
{
	servo_pwm = pwm; 
	servo_register = servo_register_;	
	dir_a = dir_a_;
	dir_b = dir_b_;
	Vi_saturation = 255;
	max_pwm = 255; 
	STATE = SE_WAIT;
	PWM_UP = 200;
	PWM_DOWN = 100;
} //ServoMotor

void ServoMotor::set_pid_paramters(float Kp_, float Ki_, float Kd_){
	Kp = Kp_;
	Ki= Ki_; 
	Kd = Kd_; 	
}

/* this function is called at fixed intervals and computes the output of the control */
float ServoMotor::pid_position(){
	static int16_t last_error = 0;
	 error = set_point_position - encoder_position;
	Kp_error = error * Kp;
	Ki_error += error;  
	if(Ki_error > Ki_saturation){
		Ki_error = Ki_saturation;
	}else if(Ki_error < -Ki_saturation){
		Ki_error = -Ki_saturation;
	}

	Kd_error = (error-last_error)*Kd*100; 
	last_error = error;	
	return Kp_error + Ki_error +Kd_error;
	
}
void ServoMotor::update_encoder_velocity() {
	static int16_t last_encoder_position = 0;
	 encoder_velocity = (encoder_position -  last_encoder_position)*5;
	 last_encoder_position = encoder_position;
}
float ServoMotor::pid_velocity(){
	static int16_t last_error; 
	update_encoder_velocity();
	velocity_error =  +set_point_velocity-encoder_velocity;
	Vp_error = velocity_error * Vp;
	Vi_error += velocity_error;
	if(Vi_error > Vi_saturation){
	Vi_error = Vi_saturation;
	}else if(Vi_error < -Vi_saturation){
	Vi_error = -Vi_saturation;
	}
	Vd_error = (velocity_error-last_error);
	last_error = velocity_error;
	return Vp*Vp_error + Vi * Vi_error + Vd_error*Vd + sign*base_velocity;
	
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
		if(output > max_pwm){
			output = max_pwm;
		}	
		*servo_pwm = output;
}
/*
bool ServoMotor::ramp_step(uint32_t current_time){
	static uint32_t ramp_time = 0;
	if(current_time - ramp_time > delta_time){
		ramp_time = current_time;
		return true;
	}else{
		return false;
	}
}
*/
bool ServoMotor::sample_position_loop(uint32_t current_time){
	static uint32_t sample_time = 0; 
	if(current_time - sample_time > POSITION_LOOP_SAMPLE_TIME){
		sample_time = current_time;
		return true;
	}else{
		return false; 
	}
}
bool ServoMotor::sample_velocity_loop(uint32_t current_time){
	static uint32_t sample_time = 0; 
	if(current_time - sample_time > VELOCITY_LOOP_SAMPLE_TIME){
		sample_time = current_time;
		return true;
	}else{
		return false; 
	}
}

void ServoMotor::move(uint32_t current_time ){	
	int16_t pwm;
	switch (STATE)
	{
		case SE_WAIT:
			if(sample_position_loop(current_time) == true){
				set_pwm(pid_position());
			}
			done = true;
			if(start == 1){
				if(new_position > encoder_position){
					sign = 1;
					set_point_velocity = speed_val;
					pwm = PWM_DOWN;
				}else{
					sign = 0; 
					set_point_velocity = speed_val * -1;
					pwm = -PWM_UP; 
				}
				done = false;
				STATE = SE_CONSTANT;
				Vi_error = 0;
				start = 0;
				set_pwm(pwm);
			}
		break;
		case SE_CONSTANT:
			if(sign){
				if(encoder_position >= new_position){
				STATE = SE_WAIT;
				set_point_position = new_position;
				}
			}else{
				if(encoder_position <= new_position){
				STATE = SE_WAIT;
				set_point_position = new_position;
				}
			}
				
		break; 
		
		default:
		/* Your code here */
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

void ServoMotor::reset(){
	set_point_position = 0; 
	encoder_position = 0; 
	set_point_velocity = 0;
	Ki_error = 0; 
}
bool ServoMotor::is_done(){
	return done;
}
// default destructor
ServoMotor::~ServoMotor()
{
} //~ServoMotor
