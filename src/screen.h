#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <stdint.h>

void screen_init();

void screen_start_write();

void screen_end_write();

void screen_write_pixel(uint8_t x, uint8_t y, uint16_t colour);

#endif // __SCREEN_H__
