/* 
* ServoMotor.cpp
*
* Created: 7/15/2020 8:57:58 PM
* Author: sojim
*/


#include "ServoMotor.h"
#include "Usart.h"
#include <avr/io.h>	
// default constructor
#define MAX_INT 100
#define MAX_SUMMATION 100
ServoMotor::ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register ,uint8_t dir_a,uint8_t dir_b)
{
	this->tacho_state = 0;
	this->target_pos = 0; 
	this->integral_term = 0; 
	this->old_error = 0; 
	this->dir_a = dir_a;
	this->dir_b = dir_b;
	this->servo_register = servo_register;
	this->servo_pwm = pwm; 
	

} //ServoMotor
void ServoMotor::set_pid(uint16_t P, uint16_t I, uint16_t D){
	P_factor = P;
	I_factor = I; 
	D_factor = D; 
	
	max_error = MAX_INT / (P_factor + 1);
	max_summation = MAX_SUMMATION /(I_factor +1); 
}

/* this function is called at fixed intervals and computes the 
 * output of the transfer function */
int16_t ServoMotor::pid(){
	error = (target_pos -absolute_position);
	uint16_t pterm;
	if(error > max_error){
		pterm = MAX_INT;
	}else if(error < -max_error){
		pterm = -MAX_INT;	
	}else{
		pterm = P_factor * error;		
	}
	return pterm;
}

void ServoMotor::rotate(uint32_t current_time){
	int16_t output;
	/*Update the PID at a frequency of 1khz */
	if(current_time - old_time > 10){
		old_time = current_time;
		output = pid(); 
	
		/*Change direction if output is less than zero */
	//	usart_sendln(output);
		if(output > 0){
			*servo_register |= dir_a  ;
			*servo_register &= ~dir_b;
		}else{
			*servo_register &= ~dir_a;
			*servo_register |= dir_b;
			output = output * -1;
		}
		/* Limit the maximum output */ 	
	
		*servo_pwm = output;
		
		
	}
}
/*Updates the absolute position from the tachometer in the motor*/
void ServoMotor::tacho(uint8_t plus, uint8_t min){
	if(tacho_state == 0){
		/* If plus goes high and min is also high it went fowards 
		 * else it went backwards */
		if(plus){
			if(min){
				absolute_position++;
			}else{
				absolute_position--;
			}
			tacho_state=1; 
		}
	}else{
		if(!plus){
			tacho_state = 0; 
		}
	}

}
// default destructor
ServoMotor::~ServoMotor()
{
} //~ServoMotor
