#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

/* Initialise the uart at 9600 baud rate */
void usart_init();

/* Write a single byte to the output of the uart */
void usart_write_byte(uint8_t data);

/* This is the safe version of write string, use for unterminated strings */
void usart_write_string_s(const char *string, int count);

/* This is the unsafe version of write string, only use this when it is known the string is null
 * terminated */
void usart_write_string(const char *string);

/* Read a byte from the incoming buffer, returns -1 if none are present */
int16_t usart_read_byte();

#endif // __USART_H__
