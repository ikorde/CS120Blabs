/*
 * ikord001_lab8_part1.c
 *
 * Created: 2/7/2019 10:32:02 AM
 * Author : ucrcse
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
	unsigned short x = ADC;  // Value of ADC register now stored in variable x.
	ADC_init(); 
	DDRA = 0x00;
	PORTA = 0xFF;
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s
	DDRD = 0xFF; // Set port B to output
	PORTD = 0x00; // Init port B to 0s
	
    while (1) 
    {
		unsigned short my_short = 0xABCD;
		unsigned char my_char = (char)my_short; // my_char = 0xCD
		my_char = (char)(my_short >> 4); // my_char = 0xBC
    }
}

