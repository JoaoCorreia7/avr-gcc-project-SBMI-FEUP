#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"
#include "serial_printf.h"


void init_mili_timers(void) {
  TCCR1B = 0;                // Stop TC1
  TIFR1 = (7<<TOV1)          // Clear pending intr
        | (1<<ICF1);
  TCCR1A = 0;                // Mode 2: CTC
  TCNT1 = 0;                 // Load BOTTOM value
  OCR1A = T1TOP;             // Load TOP value
  TIMSK1 = (1<<OCIE1A);      // Enable COMPA intr
  TCCR1B = 11;               // Start TC1 in CTC mode (TP=64)
}

void start_timer(mili_timer* timer, uint16_t value_ms){
  cli();
  timer->value = value_ms;
  timer->time = value_ms;
  sei();
}

uint8_t get_timer(mili_timer* timer){
  if (timer->time == 0) return 1;
  else return 0;
}