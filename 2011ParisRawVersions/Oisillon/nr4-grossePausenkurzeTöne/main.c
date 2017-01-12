 /*************************************************************************

   solar bird

   attiny13 power experiments

   28.9.08 christoph(at)roboterclub-freiburg.de
   15.5.11 urbanbieri(at)gmx.ch

*************************************************************************/
/*************************************************************************

   Hardware
   
   prozessor:   ATtin13

   PIN5, PORTB0/OC10: piezo controll speaker

*************************************************************************/
#include <avr/io.h>

#define PIEZOSPEAKER 	(1<<PINB0)

#define SPEAKEROFF	TCCR0A=(0x02) // PIN to normal port operation
#define SPEAKERON	TCCR0A=((1<<COM0A0) | 0x02) // toggle PIN

//GLOBAL VARIABLES
	uint16_t tone_d = 101;
	uint16_t pause_d = 500;
	int8_t pitchstep = 1;

void init_timer(){
   DDRB |= PIEZOSPEAKER; // Piezo as output   

   TCCR0A=(1<<COM0A0) | 0x02; //CTC mode and toogle OC0A port on compare match
   TCCR0B=(1<<CS00) ; // no prescaling,
   OCR0A=255; // in CTC Mode the counter counts up to OCR0A     
}

// unspecified delay time
void delay_(uint16_t duration){
   uint16_t d;
   uint8_t n;
   uint16_t counter;

   counter=(3*duration);
   // periode duration ??
   for(d=0;d<counter;d++){
      for(n=0;n<1;n++) PORTB &= ~(PIEZOSPEAKER);
      for(n=0;n<1;n++) PORTB &= ~(PIEZOSPEAKER);
   }
}

void invokePattern1(uint16_t duration, uint16_t pause, int8_t pitchstep){
	OCR0A = OCR0A - pitchstep;
	delay_(duration);
	OCR0A -= pitchstep;
	delay_(duration);
	OCR0A -= pitchstep;
	delay_(duration);
	OCR0A += (2*pitchstep);
	SPEAKEROFF; //PAUSE!
	delay_(pause);
	SPEAKERON;
}
void invokePattern2(uint16_t duration, uint16_t pause, int8_t pitchstep){
	delay_(duration<<1);
	OCR0A -= pitchstep+4;
	SPEAKEROFF; //PAUSE!
	delay_(pause<<2);
	SPEAKERON;
}

int main(void)
{  
	// working variable
	uint8_t i;
	
	// Initiation
	init_timer();
	
	while(1)
	{
		for(i=0; i<4; i++){
			invokePattern1(tone_d, pause_d, pitchstep);
			pitchstep++;
			tone_d += 20;
			pause_d -= 5;
		}
		invokePattern2((tone_d<<1), (pause_d<<2), pitchstep);
		
		if(OCR0A<=16)OCR0A=255;
		if(pitchstep>32)pitchstep=1;
		if(tone_d>200)tone_d=20;
		if(pause_d<20)pause_d=200;
		if(pitchstep<8 && OCR0A<40){
			tone_d=1;
			pitchstep = 0;
		}
	}
	return 0;
}
/***************************************************************************
*   
*   (c) 2008 christoph(at)roboterclub-freiburg.de
*		2011 urbanbieri(at)gmx.ch
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