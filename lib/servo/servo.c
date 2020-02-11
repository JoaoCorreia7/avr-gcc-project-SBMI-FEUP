#include <avr/io.h>
#include "servo.h"
#include "timers.h"
#include <util/delay.h>
#include "serial_printf.h"

#define SERVO_PIN PB1

void init_servo(void){

  DDRB |= (1 << SERVO_PIN);           // Changes PB1 to output so servo can use it
  TCCR1B = 0;                         // Stop TC1
  TIFR1 = (7<<TOV1)                   // Clear pending intr
        | (1<<ICF1);

  TCCR1A |= 1<<WGM11;                 // Mode 14 Waveform Generation Mode
  TCCR1B |= 1<<WGM13 | 1<<WGM12;      // More info on ATMega328 Datasheet page 171
  TCCR1B |= 1<<CS11 | 1<<CS10;        // clk/64 prescaler (Because servo works at 50Hz and we 
                                      //   can only count maximum of 65536 times, or 2^16)
                                      //   16MHz / 64 = 250000  and   250000/50Hz < 65536
  
  ICR1 = 4950;                        // Set TOP Value (Period Reset) (250000/50HZ)

  TCCR1A |= 1<<COM1A1 | 1<<COM1A0;    // Inverted mode. Starts the PWM signal low at the 0 timer counter. 
                                      
                                      // The OCR1A is where we will put the value for the pulse to start.
  OCR1A = ICR1 - 375;                 // The PWM pulse will be at the ending of the period.
                                      // 250000/50=5000,  5000/20ms = 250 per ms

  DDRB &= ~(1 << SERVO_PIN);          // Changes PB1 back to input so RFID can use it
  printf("PWM setup completed\n");
  //=========================================//
  //  PWM - 1.0ms/20ms - 0 degrees    (OCR1A = ICR1 - 250)  Values might slightly change in practice
  //  PWM - 1.5ms/20ms - 90 degrees   (OCR1A = ICR1 - 375)
  //  PWM - 2.0ms/20ms - 180 degrees  (OCR1A = ICR1 - 500)
  //=========================================//
}

uint8_t rotate_to_90degrees (void){

  TCCR1A |= 1<<COM1A1 | 1<<COM1A0; // Set OC1A/OC1B on Compare Match, clear OC1A/OC1B at BOTTOM (inverting mode)
  OCR1A = ICR1 - 375;
  _delay_ms(250);
  TCCR1A &= ~(1<<COM1A1);          // Change PIN 9 to normal operation again so RFID can use it
  TCCR1A &= ~(1<<COM1A0);          //
  return 1;
}


uint8_t rotate_to_0degrees (void){

  TCCR1A |= 1<<COM1A1 | 1<<COM1A0; // Set OC1A/OC1B on Compare Match, clear OC1A/OC1B at BOTTOM (inverting mode)
  OCR1A = ICR1 - 150;
  _delay_ms(500);
  TCCR1A &= ~(1<<COM1A1);          // Change PIN 9 to normal operation again so RFID can use it
  TCCR1A &= ~(1<<COM1A0);          //
  return 1;
}
