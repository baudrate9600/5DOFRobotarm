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

extern int16_t g_delta_time_encoder;
ServoMotor::ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register_ ,uint8_t dir_a_,uint8_t dir_b_)
{
	servo_pwm = pwm; 
	servo_register = servo_register_;	
	dir_a = dir_a_;
	dir_b = dir_b_;
	max_pwm = 255; 
	STATE = SE_WAIT;
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
	Ki_error = Ki*error*0.01+Ki_error; //100hz 
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
	 encoder_velocity = (encoder_position -  last_encoder_position);
	 last_encoder_position = encoder_position;
}
float ServoMotor::pid_velocity(){
	static int16_t last_error; 
	update_encoder_velocity();
	velocity_error =  +set_point_velocity-encoder_velocity;
	Vp_error = velocity_error * Vp;
	Vi_error = Vi_error + velocity_error*0.1;
	if(Vi_error > Vi_saturation){
	Vi_error = Vi_saturation;
	}else if(Vi_error < -Vi_saturation){
	Vi_error = -Vi_saturation;
	}
	Vd_error = (velocity_error-last_error)*100;
	last_error = velocity_error;
	return Vp*Vp_error + Vi * Vi_error;
	
}

void ServoMotor::rotate(){
		int16_t output = (int16_t)pid_position(); 
	
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
void ServoMotor::speed(){
		int16_t output = (int16_t)pid_velocity(); 
	
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

void ServoMotor::move(uint32_t current_time ){
	switch (STATE)
	{
		case SE_WAIT:
			if (current_time - last_time > 100){
				rotate();	
				last_time = current_time;
			}
			if (start ==1){
				STATE = SE_ACCEL;
				last_velocity_time=0;
				usart_sendln(encoder_position);
				if(new_position-set_point_position < 0){
					set_point_velocity = speed_val*-1;
				}else{
					set_point_velocity = speed_val; 
				}
		}
		break;
		case SE_ACCEL:
			
			if(current_time - last_velocity_time > 1000){
				last_velocity_time = current_time;
				speed(); 
			}
			if(encoder_position == new_position){
				set_point_position = new_position;
				start = 0;
				STATE = SE_WAIT;
				usart_sendln(encoder_position);
				usart_sendln("DONE");
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

void ServoMotor::reset(){
	set_point_position = 0; 
	encoder_position = 0; 
	set_point_velocity = 0;
	Ki_error = 0; 
}

// default destructor
ServoMotor::~ServoMotor()
{
} //~ServoMotor
