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
unsigned char wait = 0;
unsigned char score = 0; 
enum states {init,start,q1,disp1,q2,disp2,q3,disp3,end} state;
//int tick(state){
	void tick() {
	b1 = ~PINA & 0x07; 
	switch(state) {
		case init:
			if(wait<20) {++wait; state=init;}
			else {state=start; wait=0; }
			break; 
		case start:
			if(wait<20) {++wait; state=start;}
			else {state=q1; wait=0; }
			break; 
		case q1:
			//if(wait<5000 && b1 == 0x00) {++wait; state=q1;} 
			//else {state=disp1; wait=0; }
			if(b1==0x04) {state = disp1; correct=1;}
			else if(b1==0x02) {state = disp1; correct=0;}
			else state = q1; 
			break;
		case disp1: 
			if(wait<20) {++wait; state=disp1;}
			else {state=q2; wait=0; }
			break;
		case q2:
			//if(wait<5000 && b1 == 0x00) {++wait; state=q2;}
			//else {state=disp2; wait=0; }
			correct=0; 
			if(b1==0x02) {state = disp2; correct=1;} 
			else if(b1==0x04) {state = disp2; correct=0;} 
			else state = q2;
			break;
		case disp2:
			if(wait<20) {++wait; state=disp2;}
			else {state=q3; wait=0; }
			break;
		case q3:
			//if(wait<5000 && b1 == 0x00) {++wait; state=q3;}
			//else {state=disp3; wait=0; }
			if(b1==0x04) {state = disp3; correct=1;}
			else if(b1==0x02) {state = disp2; correct=0;}
			else state = q3;
			break;
		case disp3:
			if(wait<20) {++wait; state=disp3;}
			else {state=end; wait=0; }
			break;
		case end: 
			state = end; 
			break;  
		default: state = init;break;
	}
	
	switch(state) {
		case init: 
			LCD_DisplayString(1, "Quick Maths"); // with 3 questions each. You will have 5 seconds to answer each question.
			break; 
		case start:
			LCD_ClearScreen(); 
			LCD_DisplayString(1, "QUESTION 1");
			break;
		case q1: 
			if(b1==0x00) {
				LCD_ClearScreen();
				LCD_DisplayString(1, "2+2=");
				//LCD_DisplayString(8, "r");
				LCD_Cursor(17); 
				LCD_WriteData(8+'0');
				LCD_Cursor(25);
				LCD_WriteData(4+'0');
			}
			if(correct) ++score;
			break; 
		case disp1:
			LCD_ClearScreen();
			LCD_DisplayString(4, "QUESTION 2");
			break;
		case q2:
			if(b1==0x00) {
				LCD_ClearScreen();
				LCD_DisplayString(1, "9*8=");
				LCD_Cursor(17);
				LCD_WriteData(7+'0');
				LCD_Cursor(18);
				LCD_WriteData(2+'0');
				LCD_Cursor(25);
				LCD_WriteData(6+'0');
				LCD_Cursor(26);
				LCD_WriteData(4+'0');
			}
			if(correct) ++score;
			break;
		case disp2:
			LCD_ClearScreen();
			LCD_DisplayString(4, "QUESTION 3");
			break;
		case q3:
			if(b1==0x00) {
				LCD_ClearScreen();
				LCD_DisplayString(1, "7*3+15 / 4=");
				//LCD_DisplayString(8, "r");
				LCD_Cursor(17);
				LCD_WriteData(6+'0');
				LCD_Cursor(25);
				LCD_WriteData(9+'0');
			}
			if(correct) ++score;
			break;
		case disp3:
			LCD_ClearScreen();
			LCD_DisplayString(4, "END ROUND1");
			break;	
		case end: 
			LCD_ClearScreen(); 
			LCD_DisplayString(6, "Score");
			LCD_Cursor(25);
			LCD_WriteData(score+'0');
			
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
	
	TimerSet(100);
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




