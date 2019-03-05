
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
unsigned char c = 1;

enum character { startR, moving, loseR };
enum cursor { startC, play, loseC};

int character(int state) {
	butt = PINC & 0x01;
	
	switch(state) {		//transitions
		case(startR):
		if(butt) {	//press button to start game
			state = moving;
			buttOn = 1;
		}
		else {
			state = startR;
		}
		
		case(moving):
		if(buttOn==1) state = moving; 	//if start button pressed, play game
		else state = loseR;
		
		case(loseR):
		if(butt) {
			buttOn = 0;
			state = startR;
		}
		else if( curpos == r1pos || curpos == r2pos ) {
			state = startR;
		}
	}

	switch(state) {		//actions
		case(startR):
		r1pos=10;
		r2pos=14;
		
		
		case(moving):
		if(r1pos>1 && r1pos<=16) r1pos--;
		else if(r1pos==1) r1pos=16;
		if(r2pos>17 && r2pos<=32) r1pos--;
		else if(r1pos==17) r1pos=32;
		LCD_WriteData(0x04);
		
		case(loseR):
		LCD_WriteData("End Game");
	}
	
}

int cursor(int state) {
	butt = PINC & 0x01;
	buttUp = PINC & 0x02;
	buttDown = PINC & 0x04;
	switch(state) {
		case(startC):
		if(butt) state = play;
		else state = startC;
		
		case(play):
		if(buttOn==1) state = play;
		else state = loseC;
		
		case loseC:
		if(butt) state = startC;
		else state = loseC;
	}

	switch(state) {
		case(startC):
		curpos=2;
		
		case(play):
		if(buttUp && curpos==18) curpos=2;
		else if(buttUp && curpos==2) curpos=2;
		else if(buttDown && curpos==18) curpos=18;
		else if(buttDown && curpos==2) curpos=18;
		
		case(loseC):
		curpos=0;
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
	
	// Period for the tasks
	unsigned long int SMTick1_calc = 300;
	
	//Calculating GCD
	unsigned long int tmpGCD = 1;
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;
	
	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	
	//Declare an array of tasks
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
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	LCD_init();
	
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		// Scheduler code
		
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	
	// Error: Program should not exit!
	return 0;
}

