// Controlpanel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <conio.h>
#include "serial.h"
#include "terminal.h"
#include <math.h>

#define ESC "\u001b"
void startup_screen() {
	std::cout << "###############################";
	std::cout << "\nMotor driver control panel\n"; 
	std::cout << "###############################\n\n";
}
int columns, rows;
void sizeof_screen() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
void stepper_msg(char* buffer, int motor_select, int angle, int duration, int acceleration) {
	buffer[0] = 'm';
	buffer[1] = motor_select;
	//Duration
	buffer[3] = duration % 10;
	duration /= 10; 
	buffer[2] = duration % 10;
	//Acceleration 
	buffer[5] = acceleration % 10;
	acceleration /= 10; 
	buffer[4] = acceleration % 10; 
	//Angle 
	char sign = angle > 0 ? '+' : '-';
	angle = abs(angle);
	buffer[6] = sign;
	buffer[9] = angle %10;
	angle /= 10;
	buffer[8] = angle % 10; 
	angle /= 10; 
	buffer[7] = angle % 10;
	angle /= 10; 
}
void servo_msg(char* buffer,int motor_select, int pwm, int angle,int lock) {
	buffer[0] = 'm';
	buffer[1] = motor_select;

	buffer[2] = lock;
	buffer[5] = pwm %10;
	angle /= 10;
	buffer[4] = pwm % 10; 
	angle /= 10; 
	buffer[3] = pwm % 10;
	char sign = angle > 0 ? '+' : '-';
	angle = abs(angle);
	buffer[6] = sign;
	buffer[9] = angle %10;
	angle /= 10;
	buffer[8] = angle % 10; 
	angle /= 10; 
	buffer[7] = angle % 10;

		//Angle 
	char sign = angle > 0 ? '+' : '-';
	angle = abs(angle);
	buffer[6] = sign;
	buffer[9] = angle %10;
	angle /= 10;
	buffer[8] = angle % 10; 
	angle /= 10; 
	buffer[7] = angle % 10;
	angle /= 10; 

}

void servo_msg()
#define NUM_ROWS 4 
#define NUM_COLUMNS 6

struct steppermotor {
		int angle;
		int duration;
		int acceleration;
};
void draw_table( int width) {
	int stride = width / NUM_COLUMNS;
	move_xy(0, 1000);
	printf("%s[%dA", ESC,NUM_ROWS);
	for (int j = 0; j < NUM_ROWS; j++) {

		for (int i = 0; i < width; i++) {
			if (i % stride == 0) {
				printf("|");
			}
			else {
				printf(" ");
			}
		}
		printf("\n");
	}
}
/*Add elements to the table*/
void add_element(int width,int x, int y, const char *str) {
	int stride = width / NUM_COLUMNS;
	move_xy(0, 1000);
	printf("%s[%dA%s[%dC%s", ESC, NUM_ROWS-y,ESC,stride * x + 1,str);

}
/*Update stepper column and rows */
void update_stepper(int width,steppermotor motor, int column) {
		char buffer[50];
		
		sprintf_s(buffer, "\u001b[%dm %s%d (%d) \u001b[0m ",41+column,"Stepper", column,column+1);
		add_element(width, column, 0, buffer);
		sprintf_s(buffer, "%7s:%4d", "ACCEL", motor.acceleration);
		add_element(width, column, 1, buffer);
		sprintf_s(buffer, "%7s:%4d", "TIME", motor.duration);
		add_element(width, column, 2, buffer);
		sprintf_s(buffer, "%7s:%4d", "ANGLE", motor.angle);
		add_element(width, column, 3, buffer);
}
int set_stepper_values(steppermotor * motor, int duration, int absolute_angle ) {
		int delta_angle = absolute_angle - motor->angle;

	
		if (delta_angle == 0) {
			//Do not update the stepper motor values 	
			return 0;
		}
		else {
			/*Compute the discriminat to check if solution is valid*/
			float discriminant = (pow(motor->acceleration * duration, 2) -
				4 * motor->acceleration * abs(delta_angle));
			/*If the discriminant is invalid compute the smallest newest duration for the stepper motor*/
			int new_duration = new_duration = sqrt(4 * motor->acceleration *
				abs(delta_angle)) / motor->acceleration + 1;
			if (discriminant < 0.0) {
			//	printf("duration is to small, should be atleast %d \n", new_duration);
				motor->duration = new_duration;
			}
			else {
				float velocity = 0.5 * (motor->acceleration * duration - sqrt(discriminant));
				if (velocity < 1.0) {
			//		printf("duration is to small, should be atleast %d \n", new_duration);
					motor->duration = new_duration;
				}
				else {
					motor->duration = duration;
				}
			}
			motor->angle = absolute_angle;
			return 1;
		}
}

int main()
{
	/*init the terminal */
	int width = 100;
	int height = 30;
	startup_screen();
	enable_terminal();
	set_window_size(0, 0, width, height);

	steppermotor steppers[2];
	/*Open the comport*/
	int status;
	status = open_port(L"\\\\.\\COM5");
	while (status == 0) {
		std::cout << "Couldn't open COM5, Try again?\n";
		std::cout << "(Y)es , (N)o \n";
		char response;
		std::cin >> response;
		if (response == 'Y') {
			//Continue
			status = open_port(L"\\\\.\\COM5");
		}
		else if (response == 'N') {
			return 0;
		}
		else {
			std::cout << "I assume you meant N so goodbye \n";
			return 0;
		}
	}
	flush_port();
	set_timeouts();
	std::cout << "Succesfully opened COM5 \n";
	conf_port(9600, 8, 0, 0);
	std::cout << "\n";
	std::cout << "| Baudrate | 9600 | \n";
	std::cout << "| Data bits|    8 | \n";
	std::cout << "| Stopbits |    1 | \n";
	std::cout << "| parity   |   no | \n\n";

	save_position();

	draw_table(width);
	for (int i = 0; i < 2; i++) {
		steppers[i].acceleration = 1; 
		steppers[i].duration = 0; 
		steppers[i].angle = 0; 
	}
	update_stepper(width, steppers[0], 0);
	update_stepper(width, steppers[1], 1);
	last_position();
//	return 0; 
	/*start with a reset */ 
	write_char('r'); 
	while (poll_serial() == 0);
	int total_read = 0;
	char buffer[8];
	char c = 0; 
	c = read_char();
	if (c == 'k') {
		std::cout << "MCU: OK \n";
	}
	else {
		std::cout << "MCU: Did not respond with 'k', something is wrong \n\n";
		return 0; 
	}



	/*Start interactive terminal*/
	char input;
	//save_position();

	while (1) {
		save_position();
		printf("%s[2K", ESC);
		printf("%s[1B %s[2K", ESC, ESC);
		printf("%s[1B %s[2K", ESC, ESC);
		printf("%s[1B %s[2K", ESC, ESC);
	//	printf("%s[1B %s[2K", ESC, ESC);
	//	printf("%s[1B %s[2K", ESC, ESC);
		last_position();
		printf("(a) reset motors | (b) rotate motor | (c) set acceleration \n ");
		c = _getch();
		switch (c)
		{
		case 'a':
			write_char('r');
			break;
		case 'b': 
			printf("- Select motor: ");
			scanf_s("%d", &c);
			if (c == 1 || c == 2) {
				int angle;
				int index = c - 1;
				printf("Set angle: ");
				scanf_s("%d", &angle);
				if (set_stepper_values(&steppers[index], 0, angle) == 1) {
					char msg[10];
					stepper_msg(msg, index,steppers[index].angle, steppers[index].duration, steppers[index].acceleration);
					for (int i = 0; i < 10; i++) {
						write_char(msg[i]);
						printf("%d", msg[i]);
					}
					_getch();
					update_stepper(width, steppers[index], index);
				}
				
				
			}
			break;
		case 'c':
			int acceleration;
			printf("- Select motor: ");
			scanf_s("%d", &c);
			if (c == 1 || c == 2) {
				printf("- Set acceleration: ");
				scanf_s("%d",&acceleration);
				steppers[c - 1].acceleration = acceleration;
				update_stepper(width, steppers[c - 1], c - 1);
			}
			break;
		default:
			break;
		}
	
		last_position();
	}
	close_port();
	
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
