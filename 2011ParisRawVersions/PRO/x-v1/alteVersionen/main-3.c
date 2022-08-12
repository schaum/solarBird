 /*************************************************************************

   solar frog

   attiny 13 slow clock low power experiments

   28.9.08 christoph(at)roboterclub-freiburg.de
   06.10.11 urbanbieri(at)gmx.ch

*************************************************************************/
/*************************************************************************

   Hardware
   
   prozessor:   ATtin13
   clock:      128Khz internal oscillator

   PIN1   RESET         
   PIN2   PORTB3/ADC3    ADC- light sensitive sensor  
   PIN3   PORTB4/ADC2      
   PIN4   GND

   PIN5   PORTB0/OC10    piezo controll speaker
   PIN6   PORTB1         record button
   PIN7   PORTB2/ADC1      
   PIN8   VCC

*************************************************************************/
#include <avr/io.h>
#include <avr/eeprom.h>

#define INITIAL_RHYTHM 0 //16 Patterns: choose between 0 and 15

#define PIEZOSPEAKER 	(1<<PINB0)
#define SENSOR 			(1<<PINB3)
#define BUTTON 			(1<<PINB1)
//#define BUTTON2			(1<<PINB4)

#define LOADSECONDS 60

#define HZ_1706 2
#define HZ_853 4
#define HZ_586 6
#define HZ_426 8

//GLOBAL VARIABLES
uint8_t pattern[16] = {0x5b, 0xc7, 0xcd, 0xe7, 0x97, 0x59, 0x55, 0x55,
						 0xaa, 0xaa, 0xa5, 0xa5, 0x5a, 0x5a, 0xff, 0xfa};
uint8_t noise[33] = {240, 128, 33, 151, 163, 144, 0, 233, 102, 70, 106, 
0, 97, 209, 154, 34, 174, 172, 6, 84, 13, 222, 78, 107, 139, 119, 
23, 4, 67, 163, 173, 234, 120};

void init_sensor(void){
	
	DDRB &= ~(SENSOR); //set SENSOR pin as input
	// Enable ADC: ADEN to 1.
	// Prescaler 128: ADPS2-0: set to 1
	// enable the Conversion Complete Interrupt: ADIE to 1
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// sets LED-PWM value from LSR value (light sensitive resistor)
uint8_t start_conversion(void){
// for 8-bit precision I select left Adjustet representation (ADLAR) so that I only have
// to read the ADCH byte. REFS0=0: reference voltage VCC
	ADMUX = (1 << ADLAR) | (0 << REFS0) | 0x11; // 0x01 is PB2, 0x10 is PB4, 0x11 is PB3.
	ADCSRA |= (1 << ADSC);			// Start conversion
	while (!(ADCSRA & (1 << ADIF)));	// wait for conversion complete	
	ADCSRA |= (1 << ADIF);			// clear ADCIF
	return(ADCH);
}


void init_button(){
	DDRB &= ~(BUTTON); 	//set pin as input (BUTTON to 0)
	PORTB |= (BUTTON);	//turn on pull up resistor
}

void init_timer()
{
   DDRB |= PIEZOSPEAKER; // Piezo as output   
   
   /* frequency example
   TCCR0B=0x02; // internal clock source devided by 8
    OCR0A=9; // Compare value
   // calculated frequency at 9.6MHz: 9.6e6/(2*8*(9+1))= 60kHz , Division by 2 due to toogle
   //TCCR0A=0x02; // CTC-Mode (clear timer on compare match)
   */

   TCCR0A=(1<<COM0A0) | 0x02; //CTC mode and toogle OC0A port on compare match
   TCCR0B=(1<<CS00) ; // no prescaling,
   OCR0A=255; // in CTC Mode the counter counts up to OCR0A
   // CTC-Modefrequency calculation: f=IO_CLC/(2*N*(1+OCR0A)) N:Prescaler
   // e.g. IO_CLK=128KHz, N=1; OCR0A=1 ==> 64Khz
   // e.g. IO_CLK=128KHz, N=1; OCR0A=10 ==> f=6095Hz
   // e.g. IO_CLK=128KHz, N=1; OCR0A=100 ==> f=633Hz
   // e.g. IO_CLK=128KHz, N=1; OCR0A=200 ==> f=318Hz
   // e.g. IO_CLK=128KHz, N=1; OCR0A=255 ==> f=318Hz

   TCCR0B=(1<<CS00) ; // no prescaling,
      
   ///FAST PWM mode and set OC0A ( PB0 ) port on compare match   
   //TCCR0A=(1<<COM0A0) | (1<<COM0A1) | (1<< WGM00 ) | (1<< WGM01 );

   //   TCCR0B=(1<<CS00) | (1<< WGM02 ); // no prescaling, PWM OCR0A update
   //TCCR0B=(1<<CS00) ; // no prescaling,

   //TCCR0B=0x02; // internal clock source devided by 8
    //OCR0A=9; // Compare value

    OCR0A=255; // Counter end   
   // caldulated frequency at 9.6MHz: 1/9.6e6/(8*(124+1))= 9600 Baud
   // bit time = 104,17us
   //TCCR0A=0x02; // CTC-Mode (clear timer on compare match)
}

// duration in 1ms
void delay_ms(uint16_t duration)
{
   uint16_t d;
   uint8_t n;
   uint16_t counter;

   counter=(3*duration);
   // periode duration 586us
   for(d=0;d<counter;d++)
   {
      for(n=0;n<1;n++) PORTB &= ~(PIEZOSPEAKER);
      for(n=0;n<1;n++) PORTB &= ~(PIEZOSPEAKER);
   }
}

#define SPEAKEROFF	TCCR0A=(0x02)
#define SPEAKERON	TCCR0A=((1<<COM0A0) | 0x02)


void invokeGlissando(uint8_t start, uint8_t stop, uint16_t duration){
	uint8_t j; //working variable
	SPEAKERON;
	if (start<= stop){
		for(j=start;j<stop;j++){
			OCR0A = j;
			delay_ms(duration);
		}
	}else{
		for(j=start;j>stop;j--){
			OCR0A = j;
			delay_ms(duration);
		}
	}
}


void delay_vibrato(uint8_t with, uint8_t time, uint8_t count){
	uint8_t tone = OCR0A;
	uint8_t i;
	for(i=0; i<count; i++){
		invokeGlissando(tone, tone-with, time);
		invokeGlissando(tone-with, tone, time);
	}
}
// @pre section<62!
void invokeRhythm(uint8_t section, uint16_t duration){
	uint8_t j;
	uint16_t sDelay;
	for(j=0; j<4; j++){
		sDelay = pattern[section]>>(j*2);
		sDelay = sDelay & 0x03;
//		sDelay = j;
		SPEAKEROFF;
		delay_ms(duration * sDelay);
		SPEAKERON;
		delay_ms(duration * sDelay);
	}
}

uint8_t magnet(uint8_t value, uint8_t amount, uint8_t magnetvalue){
	if(value<magnetvalue){
		if((value+amount) < magnetvalue){
			value += amount;
		} else {
			value = magnetvalue;
		}
	} else {
		if((value-amount) > magnetvalue){
			value -= amount;
		} else {
			value = magnetvalue;
		}
	}
	return value;
}


void playNoise(uint8_t pitch, uint16_t hold, uint16_t pause){
	uint16_t i;
	for(i=255; i>0; i--){
		OCR0A = magnet(noise[i%32], i, pitch);
		delay_ms(12);
	}
	
// hold	
	for(i=hold; i>0; i--){
		OCR0A = noise[i%32];
	}
	SPEAKEROFF;
	delay_ms(pause);
	SPEAKERON;
}

int main(void)
{
//	uint8_t i=INITIAL_RHYTHM; // rhythm pattern nr.
	uint8_t magnetvalue = 127, k = 0;
	uint16_t pause = 0;// , j, tone = 3023;
   // Initiation
	OCR0A = 255;
	init_timer();
	init_sensor();
//	init_button();
	SPEAKERON;
	
   while(1)
   {
   		
   		SPEAKERON;

		magnetvalue=noise[k];
		playNoise(magnetvalue, 1400, 1000);
			
		SPEAKEROFF;
		delay_ms(pause);
		
		if(k==15){
   			k=0;
   		} else {
   			k++;
   		}
		noise[k] = (noise[k] + start_conversion() + noise[k+1])%256;
		
		
   }
   return 0;
}
/**************************************************************************
*   (c) 2008 christoph(at)roboterclub-freiburg.de
*		2011 urbanbieri(at)gmx.ch
***************************************************************************
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation version 2 of the License,                *
*   If you extend the program please maintain the list of authors.        *
*   If you want to use this software for commercial purposes and you      *
*   don't want to make it open source, please contact the authors for     *
*   licensing.                                                            *
***************************************************************************/ 