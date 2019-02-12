/*	Partner(s) Name & E-mail:
	Ishita Korde ikord001@ucr.edu
	Yuteng Zhang yzhan399@ucr.edu
 *	Lab Section: 021
 *	Assignment: Lab #8  Exercise #4 
 *	Exercise Description: lights a light depending on how bright sensed light is
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

int main(void)
{
	/* Replace with your application code */
	
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	ADC_init();
	int MAX = 900;
	//unsigned short x = ADC;  // Value of ADC register now stored in variable x.
	while (1)
	{
		
		
		if(ADC <= MAX/8) {PORTB = 0x01; }		//light the LED connected to PB0
		else if(ADC <= (MAX/8)*2 && ADC>MAX/8) {PORTB = 0x02; }
		else if(ADC <= (MAX/8)*3 && ADC>2*MAX/8) {PORTB = 0x04; }
		else if(ADC <= (MAX/8)*4 && ADC>3*MAX/8) {PORTB = 0x08; }
		else if(ADC <= (MAX/8)*5 && ADC>4*MAX/8) {PORTB = 0x10; }
		else if(ADC <= 6*(MAX/8) && ADC>5*MAX/8)  {PORTB = 0x20; }
		else if(ADC <= 7*MAX/8  && ADC>6*MAX/8) {PORTB = 0x40; }
		else if(ADC <= MAX && ADC>7*MAX/8) {PORTB = 0x80; }
		else {PORTB = 0x00; }	//do not light LED
		
	}
}