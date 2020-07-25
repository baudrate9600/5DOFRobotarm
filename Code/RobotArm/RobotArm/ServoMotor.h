/* 
* ServoMotor.h
*
* Created: 7/15/2020 8:57:58 PM
* Author: sojim
*/


#ifndef __SERVOMOTOR_H__
#define __SERVOMOTOR_H__
#include <avr/io.h>

class ServoMotor
{
//variables
public:
uint16_t P_factor;
uint16_t I_factor; 
uint16_t D_factor;
int32_t summation;
int32_t max_summation;
int32_t max_error; 
int32_t target_pos;
int32_t absolute_position; 
int32_t derivative_term;
int32_t integral_term;
int32_t error; 
protected:
private:

/*PID paramaters */






float old_error; 



uint32_t old_time;
volatile uint8_t * servo_register;
volatile uint8_t * servo_pwm;

uint8_t tacho_state; 

uint8_t dir_a,dir_b;
//functions
public:
	ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register ,uint8_t dir_a,uint8_t dir_b);
	void reset();
	void set_pid(uint16_t P, uint16_t I, uint16_t D);
	void tacho(uint8_t plus,uint8_t min);
	void rotate(uint32_t current_time);
	void reset_summation();
	~ServoMotor();
protected:
private:
	ServoMotor( const ServoMotor &c );
	int16_t pid();
	

	ServoMotor& operator=( const ServoMotor &c );

}; //ServoMotor

#endif //__SERVOMOTOR_H__
