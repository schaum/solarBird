 /*************************************************************************

   solar frog

   attiny 13 low power experiments

   28.9.08 christoph(at)roboterclub-freiburg.de
   15.9.10 urbanbieri(at)gmx.ch

*************************************************************************/
/*************************************************************************

   Hardware
   
   prozessor:   ATtin13

   PIN5   PORTB0/OC10    piezo controll speaker

*************************************************************************/
#include <avr/io.h>
#include <avr/eeprom.h>


#define PIEZOSPEAKER 	(1<<PINB0)
#define SENSOR 			(1<<PINB3)
#define BUTTON 			(1<<PINB1)

#define LOADSECONDS 60

#define HZ_1706 2
#define HZ_853 4
#define HZ_586 6
#define HZ_426 8

//GLOBAL VARIABLES
	uint16_t tone_d = 100;
	uint16_t pause_d = 500;
	int8_t pitchstep = 1;

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

//EEPROM examples from the Attiny13 manual, page 13.
void EEPROM_write(uint8_t ucAddress, uint8_t ucData) { 
	/* Wait for completion of previous write */ 
	while(EECR & (1<<EEPE)); 
	/* Set Programming mode */ 
	EECR = (0<<EEPM1)|(0>>EEPM0);
	/* Set up address and data registers */ 
	EEARL = ucAddress; 
	EEDR = ucData; 
	/* Write logical one to EEMPE */ 
	EECR |= (1<<EEMPE); 
	/* Start eeprom write by setting EEPE */ 
	EECR |= (1<<EEPE); 
}
uint8_t EEPROM_read(uint8_t ucAddress) { 
	/* Wait for completion of previous write */ 
	while(EECR & (1<<EEPE)); 
	/* Set up address register */ 
	EEARL = ucAddress; 
	/* Start eeprom read by writing EERE */ 
	EECR |= (1<<EERE); 
	/* Return data from data register */ 
	return EEDR; 
}

void init_button(){
	DDRB &= ~BUTTON; 	//set pin as input (BUTTON to 0)
	PORTB |= BUTTON;	//turn on pull up resistor
}

void init_timer()
{
   DDRB |= PIEZOSPEAKER; // Piezo as output   
   

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

void invokePattern1(uint16_t duration, uint16_t pause, int8_t pitchstep){
	OCR0A += pitchstep;
	delay_ms(duration);
	OCR0A -= (pitchstep);
	delay_ms(duration);
	OCR0A += pitchstep;
	delay_ms(duration);
	OCR0A += pitchstep;
	delay_ms(duration);
	OCR0A += pitchstep;
	delay_ms(duration);
	OCR0A += pitchstep;
	delay_ms(duration);
	OCR0A -= (3*pitchstep);
	SPEAKEROFF; //PAUSE!
	delay_ms(pause);
	SPEAKERON;
}
void invokePattern2(uint16_t duration, uint16_t pause, int8_t pitchstep){
	delay_ms(duration);
	OCR0A -= pitchstep;
	delay_ms(duration);
	OCR0A -= pitchstep;
	delay_ms(duration);
	OCR0A -= pitchstep;
	SPEAKEROFF; //PAUSE!
	delay_ms(pause<<3);
	SPEAKERON;
}

int main(void)
{  
	uint8_t i; //working variable
	
   // Initiation
   OCR0A = 255;
   init_timer();
   init_sensor();
   init_button();
	
   while(1)
   {
		for(i=0; i<4; i++){
			invokePattern1(tone_d, pause_d, pitchstep);
			if(0b00000100==4)pitchstep++;
			tone_d += 7;
			pause_d -= 1;
		}
		invokePattern2((tone_d<<1), (pause_d<<2), pitchstep);
		
//		SPEAKEROFF;
//		for(i=0 ; i<5; i++) delay_ms(100000);
//		SPEAKERON;
		
		if(OCR0A<=16)OCR0A=255;
		if(pitchstep<2)pitchstep=31;
		if(tone_d>600)tone_d=40;
		if(pause_d<10)pause_d=40;
		if(pitchstep<8 && OCR0A<40){
			tone_d=1;
			pitchstep = 31;
		}
   }
   return 0;
}
/***************************************************************************
*   
*   (c) 2008 christoph(at)roboterclub-freiburg.de
*       2010 urbanbieri(at)gmx.ch
***************************************************************************
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation version 2 of the License,                *
*   If you extend the program please maintain the list of authors.        *
*   If you want to use this software for commercial purposes and you      *
*   don't want to make it open source, please contact the authors for     *
*   licensing.                                                            *
***************************************************************************/ 