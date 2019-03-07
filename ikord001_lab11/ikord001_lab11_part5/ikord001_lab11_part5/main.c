#include <avr/io.h>
#include <avr/interrupt.h>
#include "bit.h"
#include "io.c"
#include "io.h"
#include "timer.h"
#include "keypad.h"
#include "scheduler.h"
#include <stdio.h>

unsigned char tmpB = 0x00;
unsigned char r1pos = 0x00;
unsigned char r2pos = 0x00;
unsigned char curpos = 0x00;
unsigned char cnt = 0;
unsigned char buttOn = 0;
unsigned char butt = 0x00; 		//start,pause,end button
unsigned char buttUp = 0x00; 	//move cursor up button
unsigned char buttDown = 0x00;	//move cursor down button
unsigned char c = 9;

enum character { startR, moving, loseR }state1;
enum cursor { startC, play, loseC}state2;

void Tick1() {
	butt = ~PINA & 0x01;
	
	switch(state1) {		//transitions
		case(startR):
			if(butt) {	//press button to start game
				state1 = moving;
				buttOn = 1;
			}
			else {
				state1 = startR;
			}
		break;
		
		case(moving):
			if(buttOn==1) state1 = moving; 	//if start button pressed, play game
			else state1 = loseR;
		break;
		
		case(loseR):
			if(butt) {
				buttOn = 0;
				state1 = startR;
			}
			else if( curpos == r1pos || curpos == r2pos ) {
				state1 = startR;
			}
		break;
	}

	switch(state1) {		//actions
		case(startR):
			r1pos=10;
			r2pos=14;
			LCD_WriteData('4');
			LCD_DisplayString(1,"start");
		break; 
		
		case(moving):
			if(r1pos>1 && r1pos<=16) r1pos--;
			else if(r1pos==1) r1pos=16;
			if(r2pos>17 && r2pos<=32) r1pos--;
			else if(r1pos==17) r1pos=32;
			LCD_Cursor(10);
			LCD_WriteData(c);
			//LCD_DisplayString(r1pos,"#");
			//LCD_DisplayString(r2pos,"#");
		break;
		
		case(loseR):
			LCD_WriteData("End Game");
		break;
	}
	
}

void Tick2() {
	butt = ~PINA & 0x01;
	buttUp = ~PINA & 0x02;
	buttDown = ~PINA & 0x04;
	switch(state2) {
		case(startC):
			if(butt) state2 = play;
			else state2 = startC;
		break;
		
		case(play):
			if(buttOn==1) state2 = play;
			else state2 = loseC;
		break;
		
		case loseC:
			if(butt) state2 = startC;
			else state2 = loseC;
		break;
	}

	switch(state2) {
		case(startC):
			curpos=2;
		break;
		
		case(play):
			if(buttUp && curpos==18) curpos=2;
			else if(buttUp && curpos==2) curpos=2;
			else if(buttDown && curpos==18) curpos=18;
			else if(buttDown && curpos==2) curpos=18;
		break;
		
		case(loseC):
			curpos=0;
		break;
	}
	LCD_Cursor(curpos);
}


int main(void)
{
	// Set Data Direction Registers
	// Buttons PORTA[0-7], set AVR PORTA to pull down logic
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	// . . . etc
	
	state1 = startR;
	state2 = startC; 
	
	// Period for the tasks
	unsigned long int SMTick1_calc = 300;
	
	//Calculating GCD
	unsigned long int tmpGCD = 1;
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;
	
	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	
	//Declare an array of tasks
	/*
	static task task1;
	static task task2;
	task *tasks[] = { &task1,&task2 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = 1000;//Task Period.
	task1.elapsedTime = 1000;//Task current elapsed time.
	task1.TickFct = &character;//Function pointer for the tick.
	
	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = 1000;//Task Period.
	task2.elapsedTime = 1000;//Task current elapsed time.
	task2.TickFct = &cursor;//Function pointer for the tick.
	*/
	// Set the timer and turn it on
	TimerSet(100);
	TimerOn();
	
	LCD_init();
	

	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		
		// Scheduler code
		/*
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}*/
		//LCD_Cursor(1);
		Tick1(); 
		//Tick2(); 
		while(!TimerFlag);
		TimerFlag = 0;
	}
	
	// Error: Program should not exit!
	return 0;
}


/*
#include <avr/io.h>
#include "io.c"

int main(void)
{
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();
	
	// Starting at position 1 on the LCD screen, writes Hello World
	LCD_DisplayString(1, "Hello World");
	
	while(1) {continue;}
}
*/


