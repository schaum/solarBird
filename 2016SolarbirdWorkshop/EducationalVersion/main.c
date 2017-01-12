 /*************************************************************************

   solar bird

   attiny13 power experiments

   28.9.08 christoph(at)roboterclub-freiburg.de
   08.9.15 urbanbieri(at)gmx.ch

*************************************************************************/
/*************************************************************************

   Hardware
   
   prozessor:   ATtin13
   clock:      9.6 Mhz internal oscillator

   PIN5, PORTB0/OC10: piezo speaker

*************************************************************************/
#include <avr/io.h>

#define PIEZOSPEAKER 	(1<<PINB0)

#define SPEAKEROFF	TCCR0A=(0x02) // PIN to normal port operation
#define SPEAKERON	TCCR0A=((1<<COM0A0) | 0x02) // toggle PIN

// global variables
uint8_t tone_global = 101;
uint16_t delay_global = 16;
uint8_t cycles_global = 10;

void init_timer(){
   DDRB |= PIEZOSPEAKER; // piezo as output   

   TCCR0A=(1<<COM0A0) | 0x02; //CTC mode and toogle OC0A port on compare match
   TCCR0B=(1<<CS00) ; // no prescaling
   OCR0A=255; // in CTC Mode the counter counts up to OCR0A     
}

// unspecified delay time
void delay_(uint16_t duration){
   uint16_t d;
   uint16_t n;
   uint16_t counter;

   counter=(duration);
   for(d=0;d<counter;d++){
      for(n=0;n<3;n++) PORTB &= ~(PIEZOSPEAKER);
      for(n=0;n<3;n++) PORTB &= ~(PIEZOSPEAKER);
   }
}

void glissando(uint8_t start, uint8_t stop, uint16_t duration){
	uint8_t i;
	if(start <= stop){
		for (i = start; i < stop; i++){
			OCR0A = i;
			delay_(duration);			
		}

	} else {
		for(i = start; i > stop; i--){
			OCR0A = i;
			delay_(duration);
		}
	}
}

void chirp(uint8_t tone, uint16_t delay){
	
	// das erste Glissando ist langsamer: 
	// das Delay ist länger.
	glissando(tone, tone-60, delay*3);
	
	SPEAKEROFF;
	delay_(1000);
	
	SPEAKERON;
	glissando(tone-60, tone-40, delay);
	
	SPEAKEROFF;
	delay_(200);
	
	SPEAKERON;
	glissando(tone-40, tone-10, delay);
	
	SPEAKEROFF;
	delay_(400);
	
	SPEAKERON;
	glissando(tone-20, tone+20, delay);
	// halte den letzten Ton
	delay_(delay*64);
}

void playPattern(){

	chirp(tone_global, delay_global);
	
	// kurze Pause
	SPEAKEROFF;
	delay_(2000);
	SPEAKERON;

	// verändere die Werte
	tone_global = tone_global + 80;
	delay_global = delay_global - 4;
	cycles_global = cycles_global - 1;

	// die Werte zurücksetzen, wenn sie 
	// zu gross oder zu klein werden
	if(tone_global > 180) {
	tone_global = tone_global - 190;	
	} 
	if(delay_global < 8) {
		delay_global = delay_global + 24;
	}

	// immer nach 10 Durchläufen macht der Vogel eine längere Pause
	if (cycles_global == 0){
		SPEAKEROFF;
		delay_(30000);
		SPEAKERON;
		cycles_global = 10;
	}
}

int main(void)
{  
	// Initiation
	init_timer();

	while(1)
	{


		// basic example
		/*
		uint8_t tonhoehe;
		tonhoehe = 122;
		
		OCR0A = 255; 
		delay_(400);
		
		SPEAKEROFF; 
		delay_(9000);
		
		OCR0A = tonhoehe;
		SPEAKERON;
		delay_(600);
		*/

		// advanced sound
		playPattern();		

	}
	return 0;
}
/***************************************************************************
*   
*   (c) 2008 christoph(at)roboterclub-freiburg.de
*		2015 urbanbieri(at)gmx.ch
*
***************************************************************************
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation version 2 of the License,                *
*   If you extend the program please maintain the list of authors.        *
*   If you want to use this software for commercial purposes and you      *
*   don't want to make it open source, please contact the authors for     *
*   licensing.                                                            *
***************************************************************************/ 