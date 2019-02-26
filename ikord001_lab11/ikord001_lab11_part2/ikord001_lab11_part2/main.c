/*    Partner(s) Name & E-mail:
 *    Ishita Korde ikord001@ucr.edu
 *    Yuteng Zhang yzhan399@ucr.edu
 *
 *    Lab Section: 021
 *    Assignment: Lab #11  Exercise #2
 *
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */

#include <avr/interrupt.h>
#include "io.c"
#include "timer.h"
#include "scheduler.h"

char * string = "CS120B is Legend... wait for it DARY!  ";
unsigned char cnt = 0;
unsigned char print[16];


enum states {init} state;
int tick(state){
	for (int i=0; i<16; ++i){
		print[i] = string[(cnt+i)%38];
	}

	cnt = (cnt+1)%38;
	LCD_DisplayString(1, print);
	return state;
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0xFF; PORTA = 0x00;
	
	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	unsigned long int SMTick1_period = 20;

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &tick;//Function pointer for the tick.
	
	TimerSet(20);
	TimerOn();
	LCD_init();

	unsigned short i;
	while(1)
	{
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if (tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while (!TimerFlag);
		TimerFlag = 0;
	}
}

