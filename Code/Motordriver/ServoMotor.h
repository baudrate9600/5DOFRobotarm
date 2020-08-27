/* 
* ServoMotor.h
*
* Created: 7/15/2020 8:57:58 PM
* Author: sojim
*/


#ifndef __SERVOMOTOR_H__
#define __SERVOMOTOR_H__
#include <avr/io.h>
typedef enum  {    SE_WAIT,
				   SE_POSITIVE,
				   SE_NEGATIVE,
				   SE_ACCEL,
				   SE_CONSTANT,
				   SE_DECEL
				   }
				   servo_fsm;
class ServoMotor
{
public:

/* PID variables */

float Vp_positive,Vi_positive,Vp_negative, Vi_negative;
int16_t set_point_position;
int16_t set_point_velocity;

int16_t encoder_position, encoder_velocity;
float Ki_saturation, Vi_saturation;
float Ki_error, Vi_error;
float Kp,Ki,Kd;


int16_t start; 
int16_t new_position  ;
servo_fsm servo_state;
int brake_plus, brake_min;
int16_t offset_positive,offset_negative;
protected:
private:
/* Variables which interface the avr output pins to the PID controller */
volatile uint8_t * servo_register;
volatile uint8_t * servo_pwm;
uint8_t dir_a,dir_b;
int16_t last_error;
uint32_t sample_time_position;
uint32_t sample_time_velocity;
int16_t last_encoder_position ;
/* Stays high until the falling edge of the encoder signal */ 
uint8_t encoder_rising_edge ; 

float last_pi_velocity_output;
float Vp,Vi;
int a,b;
bool done;
public:
	ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register ,uint8_t dir_a,uint8_t dir_b, int polarity);
	ServoMotor( const ServoMotor &c );
	void reset();
	void update_encoder_position(uint8_t plus,uint8_t min);
	void update_encoder_speed();
	void move(uint32_t current_time);
	void reset_summation();
	bool is_done();
	~ServoMotor();
protected:
private:
	float pid_position();
	float pi_velocity();
	void  set_pwm(float val);
	bool  ramp_step(uint32_t current_time);
	bool  sample_position_loop(uint32_t current_time);
	bool  sample_velocity_loop(uint32_t current_time);
	void gain_schedule();
	ServoMotor& operator=( const ServoMotor &c );
}; //ServoMotor

#endif //__SERVOMOTOR_H__
