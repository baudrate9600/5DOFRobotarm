/*These are the functions needed to communicate via 
 * the UART on windows 
 * Data modified : 25/06/2020 
 * Author: Olasoji Makiwna 
 */

#include <Windows.h>
#include <stdio.h>
HANDLE hComm;
DCB dcbSerialParams = {0};
COMMTIMEOUTS timeouts = {0};
/*Open the com port */
 int open_port(LPCWSTR port_name){
	hComm = CreateFile(port_name,
			   GENERIC_READ | GENERIC_WRITE, 
			   0,
			   NULL,
			   OPEN_EXISTING, 
			   0, 
			   NULL) ;
	if(hComm == INVALID_HANDLE_VALUE){
		return 0; 
	}else{
		SetCommMask(hComm,EV_RXCHAR);
		return  1; 
	}
	
}
/*Set the UART settings */
int conf_port(unsigned int baud_rate, 
		     unsigned char byte_size,
		     unsigned char stop_bit,
		     unsigned char parity){
	/*Configure the uart with basic settings 
	 * The reset aren't really important  */ 
	dcbSerialParams.DCBlength = sizeof (dcbSerialParams);
	dcbSerialParams.BaudRate = baud_rate;
	dcbSerialParams.ByteSize = byte_size; 
	dcbSerialParams.StopBits = stop_bit;
	dcbSerialParams.Parity   = parity;

	/* Load the configuration */
	SetCommState(hComm,&dcbSerialParams);
	return 0 ;	
}

int set_timeouts(){
	/* Timeouts in milliseconds */
	timeouts.ReadIntervalTimeout         = 50; 
	timeouts.ReadTotalTimeoutConstant    = 50; 
	timeouts.ReadTotalTimeoutMultiplier  = 10; 
	timeouts.WriteTotalTimeoutConstant   = 50; 
	timeouts.WriteTotalTimeoutMultiplier = 10; 
	SetCommTimeouts(hComm,&timeouts);
    	return 0; 
}

/*Write N-Bits via the UART */
int write_port(const char * data){
	unsigned long num_bytes = sizeof(data);
	unsigned long num_bytes_sent;
	WriteFile(hComm,data,num_bytes,&num_bytes_sent,NULL);
	return 0;
}

int write_char(char character) {
	unsigned long numb_bytes_sent; 
	WriteFile(hComm,&character, sizeof(char), &numb_bytes_sent, NULL);
	return 0; 
}


DWORD event;
int status;
char serial_buffer[255];
char rx;
int i = 0; 
int poll_serial() {
	return WaitCommEvent(hComm, &event, NULL);
}
/*Load buffer with data and return how many characters 
 * were sent */
int read_port(char * buffer,size_t size){
		
	DWORD num_bytes;	
	ReadFile(hComm, buffer, size, &num_bytes, NULL);
	return num_bytes;
}
int read_char() {
	DWORD total_bytes_read;
	char c;
	ReadFile(hComm,&c , 1, &total_bytes_read, NULL);
	return c;
}
/*Close the com port */
int close_port(){
	return CloseHandle(hComm);
}
int flush_port() {
		PurgeComm(hComm,PURGE_TXCLEAR|PURGE_RXCLEAR);
		return 0;
}
