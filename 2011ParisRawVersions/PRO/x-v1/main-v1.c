 /*************************************************************************

   solar bird

   attiny13 power experiments

   28.9.08 christoph(at)roboterclub-freiburg.de
   15.5.11 urban.bieri(at)gmx.ch

*************************************************************************/
/*************************************************************************

   Hardware
   
   prozessor:   ATtin13
   clock:      9.6 Mhz internal oscillator

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
      for(n=0;n<1;n++) {
      	PORTB &= ~(PIEZOSPEAKER);
      	OCR0B=n;
      }
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
void invokePattern1b(uint16_t duration, uint16_t pause, int8_t pitchstep){
	OCR0A += pitchstep;
	delay_(duration);
	OCR0A -= (pitchstep);
	delay_(duration);
	OCR0A += pitchstep;
	delay_(duration);
	OCR0A += pitchstep;
	delay_(duration);
	OCR0A += pitchstep;
	delay_(duration);
	OCR0A += pitchstep;
	delay_(duration);
	OCR0A -= (3*pitchstep);
	SPEAKEROFF; //PAUSE!
	delay_(pause);
	SPEAKERON;
}
void invokePattern2b(uint16_t duration, uint16_t pause, int8_t pitchstep){
	delay_(duration);
	OCR0A -= pitchstep;
	delay_(duration);
	OCR0A -= pitchstep;
	delay_(duration);
	OCR0A -= pitchstep;
	SPEAKEROFF; //PAUSE!
	delay_(pause<<3);
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
		for (i = 0; i < 50; ++i)
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

		SPEAKEROFF;
		for(i=0 ; i<5000; i++){ 
			delay_(10000);
		}
		SPEAKERON;
		
		for (i = 0; i < 15; ++i)
		{
		for(i=0; i<4; i++){
			invokePattern1b(tone_d, pause_d, pitchstep);
			if(0b00000100==4)pitchstep++;
			tone_d += 7;
			pause_d -= 1;
		}
		invokePattern2b((tone_d<<1), (pause_d<<2), pitchstep);
		
//		SPEAKEROFF;
//		for(i=0 ; i<5; i++) delay_(100000);
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

		SPEAKEROFF;
		for(i=0 ; i<200; i++) delay_(10000);
		SPEAKERON;
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