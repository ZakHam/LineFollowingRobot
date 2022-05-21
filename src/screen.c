#include "screen.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "spi.h"

// Data / command select pin
#define DC PB0
#define RST PB1
#define SS PB2

#define SCREEN_CMD_SETCOLUMN 0x15      // Set the start and end range for the current column
#define SCREEN_CMD_WRITERAM 0x5c       // Set the screen into data receive mode, until next command
#define SCREEN_CMD_SETROW 0x75         // Set the start and end range for the current row
#define SCREEN_CMD_SETREMAP 0xa0       // Set remapping / colour depth
#define SCREEN_CMD_STARTLINE 0xa1      // Set the starting row
#define SCREEN_CMD_DISPLAYOFFSET 0xa2  // Set the screen offset (row offset, always 0?)
#define SCREEN_CMD_DISPLAYALLON 0xA5   // Turn every pixel on
#define SCREEN_CMD_NORMALDISPLAY 0xa6  // Reset to normal display
#define SCREEN_CMD_FUNCTIONSELECT 0xab // Enable or disable internal VDD regulator
#define SCREEN_CMD_DISPLAYOFF 0xae     // Turn the screen off (sleep mode on)
#define SCREEN_CMD_DISPLAYON 0xaf      // Turn the screen on (sleep mode off)
#define SCREEN_CMD_PRECHARGE 0xb1      // Set the screen waveform phase lengths
#define SCREEN_CMD_CLOCKDIV 0xb3       // Set the screen oscillator frequency and clock divider
#define SCREEN_CMD_SETVSL 0xb4         // Set voltage low segment? (not sure what this does)
#define SCREEN_CMD_SETGPIO 0xb5        // Set the GPIO pins
#define SCREEN_CMD_PRECHARGE2 0xb6     // Screen waveform phase 3 charge length
#define SCREEN_CMD_VCOMH 0xbe          // Set the high voltage of common pins (wrt Vcc)
#define SCREEN_CMD_CONTRASTABC 0xc1    // Set the contrast of ABC (colour channels?)
#define SCREEN_CMD_CONTRASTMASTER 0xc7 // The master contrast scale from 1 -> 16.
#define SCREEN_CMD_MUXRATIO 0xca       // Set the multiplex ratio (how many rows to draw, 128 def.)
#define SCREEN_CMD_COMMANDLOCK 0xfd    // Used to lock / unlock screen from receiving commands

// Forward declarations of local functions
void perform_startup_sequence();
void write_command(uint8_t command_byte);
void send_command(uint8_t command_byte, const uint8_t *data, uint8_t num_data_bytes);
void set_draw_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

void screen_init() {
  // Set SS and DC output high
  DDRB |= _BV(SS) | _BV(DC);
  PORTB |= _BV(SS) | _BV(DC);

  // Initialise the SPI
  spi_init();

  // Initialise the RST pin
  DDRB |= _BV(RST);
  PORTB |= _BV(RST);

  // Write reset low, then high to clear the screen
  _delay_ms(100);
  PORTB &= ~_BV(RST);
  _delay_ms(100);
  PORTB |= _BV(RST);
  _delay_ms(200);

  // Send the sequence of startup commands
  perform_startup_sequence();
}

void screen_start_write() {
  // Write the chip select low to select the screen
  PORTB &= ~_BV(SS);
}

void screen_end_write() {
  // Write the chip select high to deselect the screen
  PORTB |= _BV(SS);
}

void screen_write_pixel(uint8_t x, uint8_t y, uint16_t colour) {
  if ((x >= 0) && (x < 128) && (y >= 0) && (y < 128)) {
    // Set the drawing window
    set_draw_window(x, y, 1, 1);

    // Write the colour, 8 high bits, followed by 8 low bits
    spi_send_byte(colour >> 8);
    spi_send_byte(colour);
  }
}

// Use PROGMEM to specify that this is stored in program space, the startup sequence is ordered as
// follows:
//    command byte
//    number of data bytes for command
//    data byte 0 -- X
static const uint8_t PROGMEM startup_sequence[] = {
    SCREEN_CMD_COMMANDLOCK, // 0x12 unlocks the OLED, and allows receiving commands
    1,
    0x12,
    SCREEN_CMD_COMMANDLOCK, // 0xb1 makes all commands accessible
    1,
    0xb1,
    SCREEN_CMD_DISPLAYOFF, // Turn the screen off
    0,
    SCREEN_CMD_CLOCKDIV, // Results in 0b1111 for osc frequency (max framerate?)
    1,                   //        and 0b0001 for a clock div of 2
    0xf1,
    SCREEN_CMD_MUXRATIO, // Set the mux ratio (how many rows to actually draw)
    1,
    0x7f,
    SCREEN_CMD_DISPLAYOFFSET, // Set the screen row offset to zero
    1,
    0x00,
    SCREEN_CMD_SETGPIO, // Set both GPIO pins to high impedance input
    1,
    0x00,
    SCREEN_CMD_FUNCTIONSELECT, // Enable the internal VDD regulator
    1,
    0x01,
    SCREEN_CMD_PRECHARGE, // Set the screen waveform phase lengths (both as low as possible)
    1,                    // Phase 1: 0b0010 (2) == 5 DCLKs
    0x32,                 // Phase 2: 0b0011 (3) == 3 DCLKs
    SCREEN_CMD_VCOMH,     // Set the common voltage as a fraction of Vcc (5 = 0.82 x Vcc)
    1,
    0x05,
    SCREEN_CMD_NORMALDISPLAY, // Reset the screen to normal display
    0,
    SCREEN_CMD_CONTRASTABC, // Set the contrast of ABC (not sure what ABC are)
    3,
    0xc8, // 0b11001000
    0x80, // 0b10000000
    0xc8, // 0b11001000
    SCREEN_CMD_CONTRASTMASTER,
    1,
    0x0f,
    SCREEN_CMD_SETVSL, // Set VSL, this is just the values from the datasheet
    3,
    0xa0,                  // 0b10100000
    0xb5,                  // 0b10110101
    0x55,                  // 0b01010101
    SCREEN_CMD_PRECHARGE2, // Set the phase 3 length (as low as possible
    1,
    0x01,                 // Phase 3: 0b0001 (1) == 1 DCLKs
    SCREEN_CMD_DISPLAYON, // Turn the screen on
    0,
    SCREEN_CMD_SETREMAP, // Scan bottom up, 65k colours
    1,
    0x74, // 0b01110100, (01) -> 65k colour, (1) -> COM split (odd/even), (1) -> row 128 -> 0
    SCREEN_CMD_STARTLINE,
    1,
    0x80,
    0}; // End of initialisation sequence

void perform_startup_sequence() {
  // Get a pointer to the start of the initialisation list
  const uint8_t *instruction_addr = (const uint8_t *)startup_sequence;
  uint8_t current_cmd = 0;
  uint8_t x = 0;
  uint8_t num_data_bytes = 0;

  // While the current address isn't 0 (end of the list)
  while ((current_cmd = pgm_read_byte(instruction_addr++)) > 0) {
    // Get the number of arguments
    // TODO: Remove this & if it's not necessary
    x = pgm_read_byte(instruction_addr++);
    num_data_bytes = x & 0x7F;

    // 255 is invalid
    if (current_cmd != 255) {
      send_command(current_cmd, instruction_addr, num_data_bytes);
    }

    instruction_addr += num_data_bytes;
  }
}

void write_command(uint8_t command_byte) {
  // Set the screen to command mode
  PORTB &= ~_BV(DC);

  // Write the command byte
  spi_send_byte(command_byte);

  // Set the screen to data mode
  PORTB |= _BV(DC);
}

void send_command(uint8_t command_byte, const uint8_t *data, uint8_t num_data_bytes) {
  // Write the chip select low to select the screen
  PORTB &= ~_BV(SS);

  write_command(command_byte);

  // Send each byte in the provided array
  for (int i = 0; i < num_data_bytes; i++) {
    uint8_t current_data_byte = pgm_read_byte(data++);
    spi_send_byte(current_data_byte);
  }

  // Write the chip select high to deselect the screen
  PORTB |= _BV(SS);
}

void set_draw_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  uint16_t x2 = x + width - 1;
  uint16_t y2 = y + height - 1;

  // Set the column window
  write_command(SCREEN_CMD_SETCOLUMN);
  spi_send_byte(x);
  spi_send_byte(x2);

  // Set the row window
  write_command(SCREEN_CMD_SETROW);
  spi_send_byte(y);
  spi_send_byte(y2);

  // Start a ram write
  write_command(SCREEN_CMD_WRITERAM);
}
