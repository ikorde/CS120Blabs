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
unsigned char b1 = 0x00;
unsigned char correct = 0; 
unsigned char playPress=0;
enum states {init,press,play,end,correct1} state;
//int tick(state){
	void tick() {
	b1 = ~PINA & 0x07; 
	switch(state) {
		case init:
			if(b1==0x01) {
				//playPress=1; 
				state = play; 
			}
			//else state = init;
			break; 
		case press:
			//if(b1==0x02) state = init; 
			//else state = press; 
			break; 
		case play:
			//if(playPress==1) state = play;
			if(b1==0x04) state = correct1;
			else state = play;
			/*if(correct) state = end; 
			else state = play; */
			break;
		case correct1:
			state = correct1;
			break;
		case end: 
			state =end; 
			break;  
		default: state = init;break;
	}
	
	switch(state) {
		case init: 
			LCD_DisplayString(1, "start");
			break; 
		case press: 
			LCD_DisplayString(1, "press");
			break;
		case play: 
			if(b1==0x00) {
				LCD_DisplayString(1, "2+2=");
				//LCD_DisplayString(8, "r");
				LCD_Cursor(17); 
				LCD_WriteData(8+'0');
				LCD_Cursor(25);
				LCD_WriteData(4+'0');
			}
			
			//LCD_DisplayString(16, "A:4");
			//LCD_DisplayString(20, "B: 5");
			break; 
		case correct1:
			LCD_ClearScreen();
			LCD_DisplayString(4, "YAS");
			break;
		case end: 
			LCD_ClearScreen(); 
			LCD_DisplayString(4, "you win");
			break;
	}
	
	
	
	return state;
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	unsigned long int SMTick1_period = 20;

	// Task 1
	task1.state = init;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &tick;//Function pointer for the tick.
	
	TimerSet(20);
	TimerOn();
	LCD_init();

	unsigned short i;
	while(1)
	{
		//PORTB = 0x01; 
		// Scheduler code
		/*
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if (tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}*/
		
		tick();
		while (!TimerFlag);
		TimerFlag = 0;
	}
}




