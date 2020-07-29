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
	buffer[6] = sign;
	buffer[9] = angle %10;
	angle /= 10;
	buffer[8] = angle % 10; 
	angle /= 10; 
	buffer[7] = angle % 10;
	angle /= 10; 
}
int main()
{
	/*init the terminal */
	int width = 100;
	int height = 30;
	startup_screen();
	enable_terminal();
	set_window_size(0, 0, width, height);

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
	set_timeouts();
	flush_port();
	std::cout << "Succesfully opened COM5 \n";
	conf_port(9600, 8, 0, 0);
	std::cout << "\n";
	std::cout << "| Baudrate | 9600 | \n";
	std::cout << "| Data bits|    8 | \n";
	std::cout << "| Stopbits |    1 | \n";
	std::cout << "| parity   |   no | \n\n";

	save_position();

	/* PRINT TABlE */
	move_xy(0, 1000);
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 7; i++) {
			printf("|%s[%dG", ESC, i * (width) / 6);
		}
		printf("|");
		printf("%s[1A\r", ESC);
	}
	for (int i = 0; i < width; i++) {
		printf("_");
	}
	printf("%s[1A\r", ESC);
	for (int i = 0; i < 7; i++) {
		printf("|%s[%dG", ESC, i * (width) / 6);
	}
	printf("|");
	//	printf("%s[1A\r", ESC);

		/* FILL LABELS */
	last_position();
	move_xy(0, 1000);
	for (int i = 0; i < 6; i++) {
		printf("%s[%dG", ESC, (i * (width) / 6) + 1);
		printf("%8s:","Status");
	}
	printf("%s[1A\r", ESC);
	for (int i = 0; i < 6; i++) {
		printf("%s[%dG", ESC, (i * (width) / 6) + 1);
		printf("%8s:","ANGLE");
	}
	printf("%s[1A\r", ESC);
    const	char* label_names[6] = { "Accel", "Accel", "P", "P", "P", "open/closed" };
	for (int i = 0; i < 6; i++) {
		printf("%s[%dG", ESC, (i * (width) / 6) + 1);
		printf("%8s:", label_names[i]);
	}
	printf("%s[1A\r", ESC);
	const	char* label_names2[6] = { "Duration", "Duration", "Duration", "Duration", "Duration", "-" };
	for (int i = 0; i < 6; i++) {
		printf("%s[%dG", ESC, (i * (width) / 6) + 1);
		printf("%8s:", label_names2[i]);
	}
	printf("%s[1A\r", ESC);
	printf("%s[1A\r", ESC);
	const	char* label_names3[6] = { "Stepper 1(1)", "Stepper 2(2)", "Servo 1(3)", "Servo 2(4)", "Servo 3(5)", "Endeffector(6)" };
	for (int i = 0; i < 6; i++) {
		printf("%s[%dG", ESC, (i * (width) / 6) + 1);
		printf("%8s", label_names3[i]);
	}
	//printf("%s[1A");
	last_position();

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
	save_position();
	while (1) {
		printf("1. Select motor:  ");
		c = _getch();
		if (c == '1' || c == '2') {
			int index = c - 48 - 1;
			printf("%s", label_names3[index]);
			
			move_xy(0, 1000);
			//Go up 5
			printf("%s[5A\r", ESC);
			printf("%s[%dG", ESC, (index * (width) / 6) + 1);
			printf("\x1b[42m%8s\x1b[0m", label_names3[index]);
			last_position();
			//Go down 1
			printf("%s[1B\r", ESC);
			printf("\n2. Enter duration: ");
			int duration = 0; 
			scanf_s("%d", &duration);
			int acceleration = 0;
			printf("3. Enter Acceleration: ");
			scanf_s("%d", &acceleration);
			int angle;
			printf("4. Enter Angle:  "); 
			scanf_s("%d", &angle); 

			float discriminant = (pow(acceleration * duration, 2) - 4 * acceleration * abs(angle));
			int new_duration = duration; 
			if (discriminant < 0.0) {
				new_duration = sqrt(4 * acceleration * abs(angle)) / acceleration + 1;
				printf("duration is to small, should be atleast %d \n", new_duration);
			}
			else {
				float velocity = 0.5 * (acceleration * duration - sqrt(discriminant));
				if (velocity < 1.0) {
					new_duration = sqrt(4 * acceleration * abs(angle)) / acceleration + 1;
					printf("duration is to small, should be atleast %d \n", new_duration);
				}
			}
			char msg[10];
			stepper_msg(msg, index, angle, new_duration, acceleration);
			for (int i = 0; i < 10; i++) {
				printf("%d ", msg[i]); 
			}
			write_port(msg);
			



		}
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
