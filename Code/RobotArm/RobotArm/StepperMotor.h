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
#define STEPPER_REGISTER PORTB

/* Finite state machine of the stepper motor
 * WAIT   : wait until start is true 
 * Compute: Compute the amount of steps for the stepper motor 
 * Turn   : each time it is called it makes 1 step until it has completely turned 
 */
 	typedef enum  {S_WAIT,S_COMPUTE,S_TURN}stepper_fsm;
class StepperMotor
{
//variables
public:
	/* Keeps track of where the stepper is and where it should go to*/
	int16_t current_pos; 
	int16_t target_pos; 
	int16_t num_steps,t0,t1;
	uint8_t start; 
	uint16_t vmax;
	uint16_t acceleration; 
	uint16_t time; 
	int16_t angle;
	uint32_t step_time; 
	uint16_t counter;
	char bufx[30];
	
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
	stepper_fsm rotate(uint32_t current_time);
	int step(); 
	~StepperMotor();
protected:
private:
	StepperMotor( const StepperMotor &c );
	StepperMotor& operator=( const StepperMotor &c );

}; //StepperMotor

#endif //__STEPPERMOTOR_H__
