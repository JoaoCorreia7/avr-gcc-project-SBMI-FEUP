#include <avr/io.h>
#include "PmodALS.h"
#include "spi.h"

/****************************
The PmodALS reports to the host board when the ADC081S021 is placed in normal mode by bringing the CS pin
low, and delivers a single reading in 16 SCLK clock cycles. The PmodALS requires the frequency of the SCLK to be
between 1 MHz and 4 MHz. The bits of information, placed on the falling edge of the SCLK and valid on the
subsequent rising edge of SCLK, consist of three leading zeroes, the eight bits of information with the MSB first,
and four trailing zeroes.
****************************/

uint8_t PmodALS_read(){

  uint8_t data[2];
  
  ENABLE_CHIP_Pmod();               //PORTB &= ~(1 << PD7);  // activate CS (Slave Select) line
  for (int i=0; i<2; i++){          // receive data
    data[i] = spi_transmit(0x00);
  }
  DISABLE_CHIP_Pmod();              //PORTB |= (1 << PD7);   // disable CS (Slave Select) line
  

  return (data[0]<<3)|(data[1]>>4);
}
