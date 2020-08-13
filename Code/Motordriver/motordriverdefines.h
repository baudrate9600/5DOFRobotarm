/*
 * motordriverdefines.h
 *
 * Created: 8/6/2020 11:20:56 PM
 *  Author: sojim
 */ 


/* Shift register defines */
#define SHIFT_REGISTER DDRB //No pun intended.. 
#define SHIFT_PORT  PORTB
#define SHIFT_MOSI (1 << PORTB3)
#define SHIFT_SS   (1 << PORTB2)
#define SHIFT_SCK  (1 << PORTB5) 
#define SHIFT_REFRESH (1 << PORTB0)

#define ENDEFF_DIRA (1 << 0); 
#define ENDEFF_DIRB (1 << 1);
#define ENDEFF_PWM  OCR1B
/* Servo Motor defines */
#define SERVO0		(1 << PORTD5)
#define SERVO0_DIRA (1 << 2)
#define SERVO0_DIRB (1 << 3) 
#define SERVO0_ENCODER_PLUS (1 << PORTC0)
#define SERVO0_ENCODER_MIN  (1 << PORTC1)
#define SERVO0_PWM	OCR0B

#define SERVO1		(1 << PORTD6)
#define SERVO1_DIRA (1 << 4) 
#define SERVO1_DIRB (1 << 5) 
#define SERVO1_ENCODER_PLUS (1 << PORTC2)
#define SERVO1_ENCODER_MIN  (1 << PORTC3)
#define SERVO1_PWM	OCR0A

#define SERVO2      (1 << PORTB1 )
#define SERVO2_DIRA (1 << 6) 
#define SERVO2_DIRB (1 << 7) 
#define SERVO2_ENCODER_PLUS (1 << PORTC4)
#define SERVO2_ENCODER_MIN  (1 << PORTC5)
#define SERVO2_PWM OCR1AL

/* Stepper motor defines */ 

#define STEPPER0_DIR (1 << PORTD2)
#define STEPPER0_STEP (1 << PORTD4)

#define STEPPER1_DIR (1 << PORTD7) 
#define STEPPER1_STEP (1 << PORTD3)


