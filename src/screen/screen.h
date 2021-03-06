#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <stdbool.h>
#include <stdint.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

/* Initialise the screen, including sending startup commands */
void screen_init();

/* Signal to the screen that data will be written */
void screen_start_write();

/* Signal to the screen data is finished being written */
void screen_end_write();

/* Write a single pixel */
void screen_write_pixel(uint8_t x, uint8_t y, uint16_t colour);

/* Fill the defined rectangle with the given colour */
void screen_write_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour);

/* Write the given character at the given position */
void screen_write_character(uint8_t x, uint8_t y, uint8_t character, uint16_t colour);

/* Write the given string, cutting off if it exits the screen, or wrapping if wrap is true */
void screen_write_string(uint8_t x, uint8_t y, const char *string, uint16_t colour, bool wrap);

#endif // __SCREEN_H__
