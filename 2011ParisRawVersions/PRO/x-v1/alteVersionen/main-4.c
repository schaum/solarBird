 /*************************************************************************

   solar bird

   attiny 13 low power experiments

   28.9.08 christoph(at)roboterclub-freiburg.de
   15.9.10 urban.bieri(at)gmx.ch

*************************************************************************/
/*************************************************************************

   Hardware
   
   prozessor:   AtTiny13
   clock:      1,2 Mhz internal oscillator
   
   PIN5, PORTB0/OC10: piezo speaker

*************************************************************************/
#include <avr/io.h>

#define PIEZOSPEAKER 	(1<<PINB0)

//GLOBAL VARIABLES
	uint16_t tone_d = 4;
	uint16_t pause_d = 20;
	uint8_t start_0 = 1;
	uint8_t stop_0 = 2;
	uint8_t start_1 = 255;
	uint8_t stop_1 = 248;
	uint8_t stepsize = 1<<5;
	

void init_timer()
{
   DDRB |= PIEZOSPEAKER; // Piezo as output   

   TCCR0A=(1<<COM0A0) | 0x02; //CTC mode and toogle OC0A port on compare match
   TCCR0B=(1<<CS00) ; // no prescaling,
   OCR0A=255; // in CTC Mode the counter counts up to OCR0A
      
   // calculated frequency at 1.2MHz: 1/1.2e6/(8*(124+1))= 1200 Baud
   // bit time = 104,17us
}

void delay_ms(uint16_t duration)
{
   uint16_t d;
   uint8_t n;
   uint16_t counter;

   counter=(3*duration);
   // periode duration ??
   for(d=0;d<counter;d++)
   {
      for(n=0;n<1;n++) PORTB &= ~(PIEZOSPEAKER);
      for(n=0;n<1;n++) PORTB &= ~(PIEZOSPEAKER);
   }
}

#define SPEAKEROFF	TCCR0A=(0x02)
#define SPEAKERON	TCCR0A=((1<<COM0A0) | 0x02)

void invokeGlissando(uint8_t start, uint8_t stop, uint16_t duration, uint16_t pause){
	uint8_t j; //working variable
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
//	SPEAKEROFF;
	delay_ms(pause);
//	SPEAKERON;
}

int main(void)
{  
	uint8_t i; //working variable
	
   // Initiation
   OCR0A = 255;
   init_timer();
	
   while(1)
   {
		
		for(i=0; i<245; i+=stepsize){
			invokeGlissando(start_0, stop_0, tone_d, pause_d);
			invokeGlissando(start_1, stop_1, tone_d, pause_d);
			start_0 += 1;
			stop_0 += 2;
			start_1 -= 3;
			stop_1 -= 7;
			tone_d += 1;
			pause_d -= 2;
			if(start_0>=240) start_0=0;
//			if(stop_0>=253) stop_0=0;
			if(stop_0-start_0>15) stop_0=start_0;
			if(start_1<127) start_1=255;
//			if(stop_1<95) stop_1=255;
			if(start_1-stop_1>31) stop_1=start_1;
			if(tone_d>20)tone_d=1;
			if(pause_d<2)pause_d=20;
		}

		
		if(stepsize<=32){
			stepsize = stepsize<<1;
		}else{
			stepsize = 1;
		}
		
		SPEAKEROFF;
		delay_ms(200);
		SPEAKERON;
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