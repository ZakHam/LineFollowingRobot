#ifndef __UART_H__
#define __UART_H__

/* Initialise the uart at 9600 baud rate */
void init_serial_uart();

/* Write a single byte to the output of the uart */
void write_byte(unsigned char data);

/* This is the safe version of write string, use for unterminated strings */
void write_string_s(const char * string, int count);

/* This is the unsafe version of write string, only use this when it is known the string is null
 * terminated */
void write_string(const char * string);

/* Read a byte from the incoming buffer, returns -1 if none are present */
int read_byte();

unsigned char * get_buffer();

#endif // __UART_H__
