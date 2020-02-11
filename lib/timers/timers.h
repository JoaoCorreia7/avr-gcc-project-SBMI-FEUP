#include <avr/io.h>
#include <avr/interrupt.h>


#define T1TOP 250 


typedef struct {
  uint16_t value;   // in miliseconds
  uint16_t time;    // number of interrupts
} mili_timer;


void init_mili_timers(void);

void start_timer(mili_timer* timer, uint16_t value_ms);


//===============================================//
// timer has finished => returns 1
// else returns 0
//
uint8_t get_timer(mili_timer* timer);
//
//===============================================//