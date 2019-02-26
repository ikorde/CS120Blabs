/*    Partner(s) Name & E-mail:
 *    Ishita Korde ikord001@ucr.edu
 *    Yuteng Zhang yzhan399@ucr.edu
 *
 *    Lab Section: 021
 *    Assignment: Lab #11  Exercise #3
 *
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */

#include <avr/interrupt.h>
#include <stdio.h>
#include "scheduler.h"
#include "timer.h"
#include "keypad.h"
#include "io.h"
#include "io.c"

enum states {start, s0, s1} state;
unsigned char key = 0x1F;
unsigned char temp1 = 0, temp2 = 0;

int tick(int state){
	switch (state){			//transitions
		case start:
			if(key== 0x1F) {
				state = start;
			}
			else if(key != 0x1F) {
				state = s0;
			}
		break;
		
		case s0:
			if(key != 0x1F) {
				state = s0;
			}
			else if(key == 0x1F) {
				state = s1;
			}
		break;
		
		case s1:
			state = start;
			break;
		
		default:
			state = start;
			key = 0x1F;
		break;
	}
	switch (state){			//actions
		case start:
			key = GetKeypadKey();
		break;
		
		case s0:
			key = GetKeypadKey();
			temp1 = GetKeypadKey();
		break;
		
		case s1:
			temp2 = temp1;
		break;
		
		default: break;
	}
	return state;
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	
	unsigned long int SMTick1_period = 50;
	unsigned char out = 0;
	
	//Declare an array of tasks
	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	//Task 1
	task1.state = -1;
	task1.period = SMTick1_period;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &tick;
	
	TimerSet(50);
	TimerOn();
	
	//lcd input output
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0xFF; PORTA = 0x00;
	LCD_start();
	LCD_ClearScreen();
	LCD_Cursor(1);
	
	while(1)
	{ 	
		// Scheduler code
		for ( unsigned short i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		
		switch (temp1)
		{
			LCD_ClearScreen();
			// All 5 LEDs on
			case '\0': PORTA = 0x1F; break;
			// binary equivalent
			case '1': 	PORTA = 0x01; out = 1; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '2': 	PORTA = 0x02; out = 2; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '3': 	PORTA = 0x03; out = 3; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '4': 	PORTA = 0x04; out = 4; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '5': 	PORTA = 0x05; out = 5; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '6': 	PORTA = 0x06; out = 6; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '7': 	PORTA = 0x07; out = 7; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '8': 	PORTA = 0x08; out = 8; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '9': 	PORTA = 0x09; out = 9; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case '0': 	PORTA = 0x00; out = 0; 
						LCD_Cursor(1); LCD_WriteData(out+'0'); 
			break;
			case 'A': 	PORTA = 0x0A; out = 65;
						LCD_Cursor(1); LCD_WriteData(out); 
			break;
			case 'B': 	PORTA = 0x0B; out = 66; 
						LCD_Cursor(1); LCD_WriteData(out); 
			break;
			case 'C': 	PORTA = 0x0C; out = 67; 
						LCD_Cursor(1); LCD_WriteData(out); 
			break;
			case 'D': 	PORTA = 0x0D; out = 68; 
						LCD_Cursor(1); LCD_WriteData(out); 
			break;
			case '*': 	PORTA = 0x0E; out = 42; 
						LCD_Cursor(1); LCD_WriteData(out); 
			break;
			case '#': 	PORTA = 0x0F; out = 35; 
						LCD_Cursor(1); LCD_WriteData(out); 
			break;

			default: PORTA = 0x1B; 
			break;


			while (!TimerFlag);
			TimerFlag = 0;
		}
	}
}