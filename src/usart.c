#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "usart.h"

#define TX_BUFFER_SIZE 16
#define RX_BUFFER_SIZE 16

typedef unsigned char tx_buffer_index_t;
typedef unsigned char rx_buffer_index_t;

volatile tx_buffer_index_t tx_buffer_head = 0;
volatile tx_buffer_index_t tx_buffer_tail = 0;
unsigned char tx_buffer[TX_BUFFER_SIZE];

volatile rx_buffer_index_t rx_buffer_head = 0;
volatile rx_buffer_index_t rx_buffer_tail = 0;
unsigned char rx_buffer[TX_BUFFER_SIZE];

// This is the interrupt vector for when the USART data register is empty
ISR(USART_UDRE_vect) {
  // Select the current character from the buffer
  unsigned char c = tx_buffer[tx_buffer_tail];

  // Advance the tail
  tx_buffer_tail = (tx_buffer_tail + 1) % TX_BUFFER_SIZE;

  // Set the byte to send
  UDR0 = c;

  // Clear the transmission complete bit
  UCSR0A &= _BV(U2X0) | _BV(TXC0);

  // Disable the interrupt if the write buffer tail has caught up to the head
  if (tx_buffer_tail == tx_buffer_head) {
    UCSR0B &= ~_BV(UDRIE0);
  }
}

// This is the interrupt vector for when a byte has finished being received
ISR(USART_RX_vect) {
  // Check if there was a parity error
  if (!(UCSR0A & _BV(UPE0))) {
    unsigned char c = UDR0;
    rx_buffer_index_t i = (rx_buffer_head + 1) % RX_BUFFER_SIZE;

    // If we aren't about to eat our own tail (overflow) write to the buffer. Otherwise, discard the
    // incoming byte (too bad, it's not worth blocking / enabling nested interrupts to clear).
    if (i != rx_buffer_tail) {
      rx_buffer[rx_buffer_head] = c;
      rx_buffer_head = i;
    }
  } else {
    // We had a parity error, discard the byte
    *(&UDR0);
  }
}

void usart_init() {
  // Set the baud rate divisor to 1 for double speed asynchronous transmission
  UCSR0A |= _BV(U2X0);

  // Enable the reception interrupt, the data empty interrupt, and receive / transmit
  UCSR0B |= _BV(RXCIE0) | _BV(UDRIE0) | _BV(RXEN0) | _BV(TXEN0);

  // USCR0C is set to 8 bits of data, 1 stop bit, no parity by default, so no change needed
  // UCSR0C

  // For a baud rate of 9600, use the following equation:
  // ( F_CPU / 4 / 9600 - 1 ) / 2 = 207 = 0x00CF
  UBRR0H = 0x00;
  UBRR0L = 0xCF;
}

void usart_write_byte(unsigned char data) {
  // Save some time if the tail has already caught the head
  if ((tx_buffer_tail == tx_buffer_head) && (UCSR0A & UDRE0)) {
    // Disable interrupts while setting the data byte
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      UDR0 = data;

      // Clear the transmission complete bit
      UCSR0A &= _BV(U2X0) | _BV(TXC0);
    }

    return;
  }

  // Otherwise, add the bit to the outgoing buffer
  tx_buffer_index_t i = (tx_buffer_head + 1) % TX_BUFFER_SIZE;

  // If there is no room in the output buffer, wait for it to be emptied in the interrupt handler
  while (i == tx_buffer_tail)
    ;

  tx_buffer[tx_buffer_head] = data;

  // Atomically set the head one further along, and enable the interrupt buffer if it has been
  // disabled by the head catching the tail
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    tx_buffer_head = i;
    UCSR0B |= _BV(UDRIE0);
  }
}

void usart_write_string_s(const char *string, int count) {
  for (int i = 0; i < count; i++) {
    usart_write_byte(string[i]);
  }
};

void usart_write_string(const char *string) {
  while (*string) {
    usart_write_byte(*string);
    string++;
  }
}

int usart_read_byte() {
  if (rx_buffer_tail == rx_buffer_head) {
    return -1;
  } else {
    unsigned char c = rx_buffer[rx_buffer_tail];
    rx_buffer_tail = (rx_buffer_tail + 1) % RX_BUFFER_SIZE;
    return c;
  }
}
