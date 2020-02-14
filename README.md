# avr-gcc-project-SBMI-FEUP

The purpose of this project was to develop our skills with AVR microcontrollers, such as the ATMega328p, more commonly known as the Arduino Uno microcontroller. The whole project was made without the use of Arduino libraries.

Our project simulates, on a simplified scale, various systems that we are used to see all around us everyday:
- An ilumination system that activates once there isn't enough brightness;
- A parking lot gate which open and closes (the gate being a servo motor), depending on the available capacity of the parking lot. The gate opens via various RFID cards that can be added to the system at anytime. Duplicate RFID tags are not accepted;
- A ventilation system, which turns a fan on once a predetermined temperature is reached.


The temperature sensor used was the ds18b20 which uses OneWire as the communication protocol. A reset pulse is sent by the Arduino as an initialization command. Once the sensor confirms this signal, a Skip Rom command [CCh] is sent which communicates with every slave using OneWire as its communication protocol. In this case, the ds18b20 is the only sensor using OneWire so the Arduino detects it immediately and asks for a temperature conversion using the [44h] command. The microcontroller then receives the temperature information as a 16 bit value, 4 zeros followed by 12 bits of data. 

The RFID used was the cheap RFIO-RC522. This RFID uses the MFRC522 chip. It communicates at 13,56 MHz with the MIFARE protocol. In this project we used the SPI protocol to communicate with the microcontroller.

The PmodALS reports to the host board when the ADC081S021 is placed in normal mode by bringing the CS pin low, and delivers a single reading in 16 SCLK clock cycles. The PmodALS requires the frequency of the SCLK to be between 1 MHz and 4 MHz. The bits of information, placed on the falling edge of the SCLK and valid on the subsequent rising edge of SCLK, consist of three leading zeroes, the eight bits of information with the MSB first, and four trailing zeroes.

The servo motor used was the sg-90 microservo and it was controlled with PWM. All the info on how to use PWN with AVR is commented on the servo code. 

Important info such as saved RFID cards and current parking lot occupation are saved on the Arduino's eeprom so no information is lost in case of a power loss. 

This project was made on the PlatformIO Extension on VScode, so everything was compiled there and sent to the microcontroller using this extension.

Schematic: 

![Schematic](https://github.com/JoaoCorreia7/avr-gcc-project-SBMI-FEUP/blob/master/Schematic.png)
