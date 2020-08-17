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
float Kp,Ki,Kd;
int16_t set_point_position;
float Vp,Vi,Vd;
int16_t set_point_velocity;

int16_t encoder_position;
int16_t encoder_velocity;
int16_t speed_val;
int16_t Kp_error, Ki_error , Kd_error,error; 
int16_t Vp_error, Vi_error , Vd_error;
int16_t velocity_error; 
int16_t max_pwm;
float Ki_saturation; 
float Vi_saturation;

int16_t PWM_UP, PWM_DOWN;
int16_t start; 
uint32_t last_time;
uint32_t last_velocity_time;
int16_t new_position  ;
servo_fsm STATE;
protected:
private:
/* Variables which interface the avr output pins to the PID controller */
volatile uint8_t * servo_register;
volatile uint8_t * servo_pwm;
uint8_t dir_a,dir_b;

/* Stays high until the falling edge of the encoder signal */ 
uint8_t encoder_rising_edge ; 

bool done;
public:
	ServoMotor(volatile uint8_t * pwm,volatile uint8_t * servo_register ,uint8_t dir_a,uint8_t dir_b);
	ServoMotor( const ServoMotor &c );
	void reset();
	void set_pid_paramters(float P, float I, float D);
	void update_encoder_position(uint8_t plus,uint8_t min);
	void update_encoder_velocity();
	void move(uint32_t current_time);
	void reset_summation();
	bool is_done();
	~ServoMotor();
protected:
private:
	float pid_position();
	float pid_velocity();
	void  set_pwm(float val);
	bool  ramp_step(uint32_t current_time);
	bool  sample_position_loop(uint32_t current_time);
	bool  sample_velocity_loop(uint32_t current_time);
	ServoMotor& operator=( const ServoMotor &c );
	uint8_t base_velocity;
	int sign;
}; //ServoMotor

#endif //__SERVOMOTOR_H__
