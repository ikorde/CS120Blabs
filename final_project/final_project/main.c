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
#include "usart.h"

unsigned char b1 = 0x00;
unsigned char correct = 0; 
unsigned char wait = 0;
unsigned char score = 0x00; 
unsigned char finished = 0; 
enum states1 {init,start,q1,disp1,q2,disp2,q3,disp3,end} state1;
enum states2 {init2,dispJ}state2;

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}
	
//int tick(state){
void tick1() {
	b1 = ~PINA & 0x07; 
	switch(state1) {
		case init:
			if(wait<20) {++wait; state1=init;}
			else {state1=start; wait=0; }
			break; 
		case start:
			if(wait<20) {++wait; state1=start;}
			else {state1=q1; wait=0; }
			break; 
		case q1:
			//if(wait<5000 && b1 == 0x00) {++wait; state1=q1;} 
			//else {state1=disp1; wait=0; }
			if(b1==0x04) {state1 = disp1; correct=1;}
			else if(b1==0x02) {state1 = disp1; correct=0;}
			else state1 = q1; 
			break;
		case disp1: 
			if(wait<20) {++wait; state1=disp1;}
			else {state1=q2; wait=0; }
			break;
		case q2:
			//if(wait<5000 && b1 == 0x00) {++wait; state1=q2;}
			//else {state1=disp2; wait=0; }
			correct=0; 
			if(b1==0x02) {state1 = disp2; correct=1;} 
			else if(b1==0x04) {state1 = disp2; correct=0;} 
			else state1 = q2;
			break;
		case disp2:
			if(wait<20) {++wait; state1=disp2;}
			else {state1=q3; wait=0; }
			break;
		case q3:
			//if(wait<5000 && b1 == 0x00) {++wait; state1=q3;}
			//else {state1=disp3; wait=0; }
			if(b1==0x04) {state1 = disp3; correct=1;}
			else if(b1==0x02) {state1 = disp3; correct=0;}
			else state1 = q3;
			break;
		case disp3:
			if(wait<20) {++wait; state1=disp3;}
			else {state1=end; wait=0; }
			break;
		case end: 
			state1 = end; 
			break;  
		default: state1 = init;break;
	}
	
	switch(state1) {
		case init: 
			LCD_DisplayString(1, "Quick Maths"); // with 3 questions each. You will have 5 seconds to answer each question.
			score=0x00;
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
			if(correct) score=score+1;
			
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
			if(correct) score=score+1;
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
			if(correct) score=score+1;
			break;
		case disp3:
			LCD_ClearScreen();
			LCD_DisplayString(4, "END ROUND1");
			break;	
		case end: 
			LCD_ClearScreen(); 
			LCD_DisplayString(6, "Score");
			//score=3;
			LCD_Cursor(25);
			LCD_WriteData(score+'0');
			
			//LCD_DisplayString(1,score);
			finished=1; 
			if(finished==1) PORTA=0x01; 
			break;
	}
	return state1;
}


void tick2() {
	unsigned short x=ADC;
	switch(state2) {
		case init2:
			/*if(wait<20) {++wait; state1=init2;}
			else {state1=dispJ; wait=0; }*/
			state2=dispJ;
		break;

		
		case dispJ:
			state2 = dispJ; 
		break;
	}
	switch(state2) {
		case init2: 
			LCD_ClearScreen();
			LCD_DisplayString(3,"Round 2");
			break;
		case dispJ:
			LCD_ClearScreen();
			if(ADC<20) LCD_DisplayString(1,"20");	
			if(ADC>40) LCD_DisplayString(1,"40");
			if(ADC>60) LCD_DisplayString(1,"60");
			if(ADC==1023) LCD_DisplayString(1,"1000");
			break;
	}
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF; 
	//DDRA = 0xFF; PORTA = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	unsigned long int SMTick1_period = 20;

	// USART code , communication between two micro-controllers
	/*initUSART(0);
	unsigned char t = 0x01;
	//USART_Send(t,0);
	if(USART_HasReceived(0)==0x01) {
		PORTB = USART_Receive(0); 
	}*/
	
	// Task 1
	task1.state = init;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &tick1;//Function pointer for the tick.
	
	TimerSet(100);
	TimerOn();
	LCD_init();
	ADC_init();

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
		
		//tick1();
		/*
		initUSART(0);
		unsigned char t = 0x02;
		USART_Send(t,0);
		//if(USART_HasReceived(0)) {
			PORTB = USART_Receive(0);
		//}*/
		
		//if(PINB==0x01) {PORTA=0x01;}
		//PORTA=0x01;
		
		//unsigned short my_short = ADC;
		//unsigned short c = 0x0F;
		if(ADC < 971) {
			PORTB = 0x01;
		}
		if(ADC > 1000) {
			PORTB = 0x04; 
		}
		if(ADC < 500) PORTB=0x02;
		//PORTB=ADC;
		//else PORTB = 0x04; 
		//state2=init;
		//tick2();
		
		//PORTB=ADC;
		//my_char = (char)(my_short >> 4); // my_char = 0xBC
		//PORTD = my_char;
		
		while (!TimerFlag);
		TimerFlag = 0;
		
	}
}




