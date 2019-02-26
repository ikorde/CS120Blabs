/*    Partner(s) Name & E-mail:
 *    Ishita Korde ikord001@ucr.edu
 *    Yuteng Zhang yzhan399@ucr.edu
 *
 *    Lab Section: 021
 *    Assignment: Lab #11  Exercise #1
 *
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */

#include "avr/io.h"
#include "scheduler.h"
#include "keypad.h"
#include "stdio.h"

enum States {start, s0, s1};     
task task_keypad;                           

int tick_keypad(int state) {

    unsigned char x;            
    
    switch(state) {     // Transitions
        case start: 
            state = s0; break;    
        case s0:
            state = s1; break;    
        case s1:
            break;
        default: 
            state = start; break;  
    }

    switch(state) {     // Actions
        case s1:
            x = GetKeypadKey();    

            // LED output
            switch(x) {
                case '\0': PORTB = 0x1F; break;
                case '1': PORTB = 0x01; break;
                case '2': PORTB = 0x02; break;
                case '3': PORTB = 0x03; break;
                case '4': PORTB = 0x04; break;
                case '5': PORTB = 0x05; break;
                case '6': PORTB = 0x06; break;
                case '7': PORTB = 0x07; break;
                case '8': PORTB = 0x08; break;
                case '9': PORTB = 0x09; break;
                case 'A': PORTB = 0x0A; break;
                case 'B': PORTB = 0x0B; break;
                case 'C': PORTB = 0x0C; break;
                case 'D': PORTB = 0x0D; break;
                case '*': PORTB = 0x0E; break;
                case '0': PORTB = 0x00; break;
                case '#': PORTB = 0xF0; break;
                default: PORTB = 0x1B; break;
            }
            break;
        default: break;
    }
    return state;
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;	
	DDRC = 0xF0; PORTC = 0x0F;	

    task_keypad.state = start;      
    task_keypad.TickFct = &tick_keypad;   

	while(1)
	{
        task_keypad.state = task_keypad.TickFct(task_keypad.state);   
	}
}

