/*
 * Uart.h
 *
 * Created: 5/30/2020 4:40:26 PM
 *  Author: sojim
 */ 


#ifndef USART_H_
#define USART_H_

#include <avr/io.h> 
#define MAX_BUFFER 32 
struct{
	volatile char data_input[MAX_BUFFER];
	volatile char data_ouput[MAX_BUFFER];
	volatile bool new_command;	
	volatile uint8_t byte_count;
}uart_io_t;

void usart_enable(uint16_t baudrate) ;
char usart_fifo();
char usart_recieve();
void usart_unblock();
void usart_block();

void usart_send(int); 
void usart_send(char);
void usart_send(float,int);
void usart_send(char *);
void usart_send(int32_t);
void usart_send(uint16_t);
void usart_send(uint32_t);

void usart_sendln(int);
void usart_sendln(char);
void usart_sendln(float);
void usart_sendln(char *);
void usart_sendln(uint16_t); 
void usart_sendln(uint32_t);
void usart_sendln(int32_t);
void usart_newline();
#endif /* USART_H_ */
