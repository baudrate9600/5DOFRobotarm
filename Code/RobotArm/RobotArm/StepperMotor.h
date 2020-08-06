/* 
* StepperMotor.h
*
* Created: 7/7/2020 7:52:01 PM
* Author: sojim
*/

#include <avr/io.h>
#ifndef __STEPPERMOTOR_H__
#define __STEPPERMOTOR_H__


/*ideally the stepper motor should all be connected to the same register*/
#define STEPPER_REGISTER PORTD

/* Finite state machine of the stepper motor
 * S_ACCEL   : accelerate until t_0 
 * S_CONSTANT: rotate at fixed speed [t0, t1]
 * S_DECEL   : decelerate until stop
 */
 /*
   v
	|       ________________________________   - - - - - vmax 
	|      /|							   |\
	|     /	|							   | \
	|    /	|							   |  \
	|   /	|							   |   \ --------- slop=a
	|  /	|							   |	\
	| /		|							   |	 \
	0       t0                             t1     stop 	
	 ------------------------------------------------ t
*/

 	typedef enum  {S_WAIT,
				   S_COMPUTE,
				   S_ACCEL,
				   S_CONSTANT,
				   S_DECEL,S_TURN}
				   stepper_fsm;
class StepperMotor
{
//variables
public:
	/* Start the rotation of the stepper motor */ 	
	uint8_t start; 
	
	/* Keeps track of where the stepper is and where it should go to*/
	int16_t current_pos; 
	int16_t target_pos; 
	
	/* Total number of steps , and the treshholds values */
	uint16_t num_steps,t0,t1;
	
	/* Rate at which the stepper motor accelerates */
	uint16_t acceleration; 
	/*Duration of the rotation*/
	uint16_t duration; 

	int32_t long_pulse_width;
	uint32_t pulse_width;

	int16_t pulse_width_counter ;
	uint16_t step_counter;
	
protected:
private:
	int16_t velocity;
	uint32_t stepper_time;
	/* first bit is start or stop, second bit is turn clockwise or anti clockwise */

	/*A4988 signals */
	uint8_t dir_pin, step_pin; 
	
	/* this variable determines how many steps correspond with a change in angle*/	
	float step_to_angle;


    stepper_fsm state;
	
//functions
public:
	StepperMotor(int16_t current_pos, float gear_train, uint8_t dir, uint8_t step);
	StepperMotor( const StepperMotor &c );
	int rotate(uint32_t current_time);
	stepper_fsm fsm(uint32_t current_time);
	int step(); 
	void reset();
	~StepperMotor();
protected:
private:
	StepperMotor& operator=( const StepperMotor &c );

}; //StepperMotor

#endif //__STEPPERMOTOR_H__
