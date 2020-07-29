#include <Windows.h> 

int open_port(LPCWSTR port_name); 
int conf_port(unsigned int, unsigned char, unsigned char, unsigned char);
int set_timeouts();
int write_port(const char*);
int write_char(char character); 
int read_port(char*, size_t); 
int read_char();
int flush_port();
int poll_serial(); 
int close_port(); 
