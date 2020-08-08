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
public:

/* PID variables */
float Kp,Ki,Kd;
int16_t set_point;
int16_t encoder_position;
int16_t Kp_error, Ki_error , Kd_error,error; 
int16_t max_pwm;
float Ki_saturation; 

protected:
private:
/* Variables which interface the avr output pins to the PID controller */
volatile uint8_t * servo_register;
volatile uint8_t * servo_pwm;
uint8_t dir_a,dir_b;

/* Stays high until the falling edge of the encoder signal */ 
uint8_t encoder_rising_edge ; 

public:
	ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register ,uint8_t dir_a,uint8_t dir_b);
	ServoMotor( const ServoMotor &c );
	void reset();
	void set_pid_paramters(float P, float I, float D);
	void update_encoder_position(uint8_t plus,uint8_t min);
	void rotate();
	void reset_summation();
	~ServoMotor();
protected:
private:
	float pid_output();
	ServoMotor& operator=( const ServoMotor &c );

}; //ServoMotor

#endif //__SERVOMOTOR_H__