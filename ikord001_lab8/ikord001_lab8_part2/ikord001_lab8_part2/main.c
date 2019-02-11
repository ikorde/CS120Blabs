/*	Partner(s) Name & E-mail:
	Ishita Korde ikord001@ucr.edu
	Yuteng Zhang yzhan399@ucr.edu
 *	Lab Section: 021
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
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
	DDRD = 0xFF; PORTD = 0x03; // Init port B to 0s
	ADC_init();
	//unsigned short x = ADC;  // Value of ADC register now stored in variable x.
    while (1) 
    {
		unsigned short my_short = ADC;
		unsigned char my_char = (char)my_short; // my_char = 0xCD
		PORTB = my_char; 
		my_char = (char)(my_short >> 4); // my_char = 0xBC
		PORTD = my_char; 
    }
}
