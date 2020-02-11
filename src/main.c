#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <math.h>

#include "serial_printf.h"
#include "utils.h"
#include "tsl2561.h"
#include "spi.h"
#include "mfrc522.h"
#include "i2c_master.h"
#include "PmodALS.h"
#include "timers.h"
#include "servo.h"
#include "ds18b20.h"
#include "onewirenun.h"



#define SERVO_PIN PB1
#define LED_WHITE_PIN PC0
#define LED_GREEN_PIN PC1
#define LED_RED_PIN PC2
#define BUZZER_PIN PD7
#define RST_PIN PD5
#define MOTOR_PIN PD6
#define BUTTON_PIN PD2
#define MAX_COUNT 5

#define LUMINOSITY_VALUE 1
#define TEMP_VALUE_CELSIUS 25           // 25 degrees Celsius

uint16_t EEMEM count_eeprom;   // eeprom
uint8_t EEMEM str_bff_m[10][16];
uint8_t EEMEM n_cards_m;
uint8_t str_bff[10][16];
uint8_t n_cards;

void io_init(void){
  printf_init();  
  init_servo();      
  spi_init();
  mfrc522_init();
  
  DDRD &= ~(1 << RST_PIN);
  DDRB |= (1 << SERVO_PIN);
  DDRC |= (1 << LED_WHITE_PIN);  // set as output
  DDRC |= (1 << LED_GREEN_PIN);  // set as output
  DDRC |= (1 << LED_RED_PIN);    // set as output
  DDRD |= (1 << BUZZER_PIN);     // set as output
  DDRD |= (1 << MOTOR_PIN);      // set as output
  

  printf("Init done\n");
}

//===========================================================//
//===========================================================//
uint8_t search_rfid_card(void){  
  
  uint8_t byte;
  uint8_t str[MAX_LEN];

  byte = mfrc522_request(PICC_REQALL,str);
  // printf("reading: %u\n", byte);
  _delay_ms(500);
  if (byte == 1) return 1;                // returns 1 if card was found
  else return 0;                          // returns 0 if card was not found
  
} 
//===========================================================//
//===========================================================//
uint16_t get_temperature(void){

  gpin_t sensor ={ &PORTD, &PIND, &DDRD, PD3 };

  onewire_reset(&sensor);
  onewire_skiprom(&sensor);
  onewire_write(&sensor,0x44);
  _delay_ms(250);

  return ds18b20_read_single (&sensor);
}
//===========================================================//
//===========================================================//
void beep(void){
  for (int i=0; i<15; i++){
    PORTD |= (1 << BUZZER_PIN);
    _delay_ms(1);
    PORTD &= ~(1 << BUZZER_PIN);
    _delay_ms(1);
  }
}
//===========================================================//
//===========================================================//
int isButtonPressed(void){
  if (!(PIND & (1 << BUTTON_PIN)))
    return 1;
  else return 0;
}
//===========================================================//
//===========================================================//
uint16_t temp_value_converter(){
  uint16_t binNum[16];
  uint16_t vfinal=0;
  uint16_t mm = TEMP_VALUE_CELSIUS;

  int i = 0;

  for (i=0; mm>0; i++){
     binNum[i] = mm%2;
     mm = mm/2;
  }

  int a=8;
  for(int z=0; z<i; z++){
    a=a*2;
    vfinal= vfinal + binNum[z]*a;
  }

  return vfinal;
}
//===========================================================//
//===========================================================//
uint8_t checkCard(){     // checks if a specific card is in the database
  
  uint8_t str[16]={0};
  uint8_t byte = mfrc522_request(PICC_REQALL,str);
  
  if (byte == CARD_FOUND){	
    byte = mfrc522_get_card_serial(str);
		if (byte == CARD_FOUND)
		{
      for (int n=0;n<n_cards;n++){
        int aux=0;
        for (int z=0; z<MAX_LEN; z++){
          if(str[z]==str_bff[n][z]){
            aux++;
          }
        }
        if(aux==MAX_LEN) return 1;
      }
		}
  }
  return 0;
}
//===========================================================//
//===========================================================//
void cards_EEPROM (int i){      // if i=1 loads eeprom data
  if (i==1){
  n_cards=eeprom_read_word(&n_cards_m);

  for(int i=0; i<n_cards; i++){
    for(int z=0; z<16;z++){
    str_bff[i][z]=eeprom_read_word(&str_bff_m[i][z]);
    }
  }
}
  
  
  if(i==2){         // if i=2 updates eeprom data
    eeprom_update_word(&n_cards_m, n_cards);
    for(int i=0; i<n_cards; i++){
      for(int z=0; z<16;z++){
      eeprom_update_word(&str_bff_m[i][z], str_bff[i][z]);
      }
    }
  }
}
//===========================================================//
//===========================================================//
void reset_EEPROM(){    // can be used to clear eeprom
  eeprom_update_word(&count_eeprom, 0);
  eeprom_update_word(&n_cards_m, 0);

  for(int i=0; i<10; i++){
    for(int z=0; z<16;z++){
    eeprom_update_word(&str_bff_m[i][z], 0);
  }
 }
}
//===========================================================//
//===========================================================//
int main() {

  io_init();
  _delay_ms(2000);
  uint8_t count = eeprom_read_word(&count_eeprom);
  
  uint8_t state_rfid = 1;
  uint8_t state_LED_WHITE = 0;
  uint8_t state_temp = 0;
  uint8_t state_count = 0;
  
  uint8_t s1 = 0;
  uint8_t ls1 = 0;
  uint8_t rising_edge = 0;

  uint16_t TEMP_VALUE = temp_value_converter();

  uint8_t str[MAX_LEN] = {0};
  uint8_t byte;
  
  cards_EEPROM(1);

  while (!(isButtonPressed())){
    if(search_rfid_card()){	
      byte = mfrc522_get_card_serial(str);
      if(byte==1 && n_cards < 10){ //allows up to 10 stored cards
        // checks card several times to make sure the card is not already stored in the buffer
			  if((((checkCard()==0) && (checkCard()==0) && (checkCard()==0)) || (n_cards==0)))
			  {
          for(int s=0;s<MAX_LEN;s++){
            str_bff[n_cards][s]=str[s];
          }
          n_cards++;
          beep();
          printf ("Novo cartao guardado\n");
			  }
      }
    }
  }
  cards_EEPROM(2);

  printf("\n\n\n----------------CARTÕES GUARDADOS: %d----------------\n\n\n", n_cards);

  while (1){
    
    switch (state_count){
      case 0:
      {
        if (count < MAX_COUNT){
          PORTC |= (1 << LED_GREEN_PIN);     // TURN ON GREEN LED
          PORTC &= ~(1 << LED_RED_PIN);      // TURN OFF RED LED
          printf ("Green Led ON & Red Led OFF\n");
          state_count = 1;
        }
      } break;
      case 1:
      {
        if (count >= MAX_COUNT){
          PORTC |= (1 << LED_RED_PIN);       // TURN ON RED LED
          PORTC &= ~(1 << LED_GREEN_PIN);    // TURN OFF GREEN LED
          printf ("Red Led ON & Green Led OFF\n");
          state_count = 0;
        }
      } break;
      default: state_count = 0;
    }


    s1 = isButtonPressed();
    if (ls1 == 0 && s1 == 1) rising_edge = 1;
      else rising_edge = 0;
    ls1 = s1;
    



    switch (state_rfid){
      // state_rfid = 1 if card is found
      case 0:                                      
      {
        if (checkCard() && count < MAX_COUNT){
          beep();
          state_rfid = 1;
          printf ("Card found\n");
          rotate_to_90degrees();
          count++;
          eeprom_update_word(&count_eeprom, count);
          printf("Servo Rotated to top position (90 degrees)\n");
          printf("Count: %d\n", count);
        }
      } break;
      
      // check if no card is detected 3 times to make sure.
      // state_rfid = 0 if there's no detection 3 times in a row.
      case 1:                                      
      {                                             
        if (checkCard() == 0){
          if (checkCard() == 0){
            if (checkCard() == 0){
              state_rfid = 0;
              printf ("Waiting for rfid card\n");
              rotate_to_0degrees();
              printf("Servo Rotated to horizontal position (0 degrees)\n");
            } else break;
          } else break;
        } else break;
      } break;
      default: state_rfid = 0;
    }




    switch (state_LED_WHITE){
      case 0:
      {
        if (PmodALS_read() < LUMINOSITY_VALUE){
          PORTC |= (1 << LED_WHITE_PIN);     // TURN ON WHITE LED
          state_LED_WHITE = 1;
          printf ("Luminosity Led ON\n");
        }
      } break;
      case 1:
      {
        if (PmodALS_read() >= LUMINOSITY_VALUE){
          PORTC &= ~(1 << LED_WHITE_PIN); // TURN OFF WHITE LED
          state_LED_WHITE = 0;
          printf ("Luminosity Led OFF\n");
        }
      } break;
      default: state_LED_WHITE = 0;
    } 





    switch (state_temp){
      case 0:
      {
        if (get_temperature() > TEMP_VALUE){
          PORTD |= (1 << MOTOR_PIN);
          state_temp = 1;
          printf ("Fan ON\n");
        }
      } break;
      case 1:
      {
        if (get_temperature() <= TEMP_VALUE){
          PORTD &= ~(1 << MOTOR_PIN);
          state_temp = 0;
          printf ("Fan OFF\n");
        }
      } break;
      default: state_temp = 0;
    }


    if (rising_edge && count>0){
      count--;
      eeprom_update_word(&count_eeprom, count);
      printf("Count: %d\n", count);
    }

    

    _delay_ms(10);
  
  }
}