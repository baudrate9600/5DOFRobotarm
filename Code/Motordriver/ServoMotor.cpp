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
#define MAX_INT 100
#define MAX_SUMMATION 5
ServoMotor::ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register_ ,uint8_t dir_a_,uint8_t dir_b_)
{
	servo_pwm = pwm; 
	servo_register = servo_register_;	
	dir_a = dir_a_;
	dir_b = dir_b_;

} //ServoMotor

void ServoMotor::set_pid_paramters(float Kp_, float Ki_, float Kd_){
	Kp = Kp_;
	Ki= Ki_; 
	Kd = Kd_; 	
}

/* this function is called at fixed intervals and computes the output of the control */
float ServoMotor::pid_output(){
	static int16_t last_error = 0;
	 error = set_point - encoder_position;
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

void ServoMotor::rotate(){
		int16_t output = (int16_t)pid_output(); 
	
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
	set_point = 0; 
	encoder_position = 0; 
	Ki_error = 0; 
}

// default destructor
ServoMotor::~ServoMotor()
{
} //~ServoMotor
