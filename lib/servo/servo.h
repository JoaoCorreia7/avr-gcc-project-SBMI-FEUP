#include <avr/io.h>

//=========================================//
//  PWM - 1.0ms/20ms - 0 degrees
//  PWM - 1.5ms/20ms - 90 degrees
//  PWM - 2.0ms/20ms - 180 degrees
//=========================================//


void init_servo (void);

uint8_t rotate_to_90degrees (void);

uint8_t rotate_to_0degrees (void);
