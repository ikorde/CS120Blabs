
#include <avr/interrupt.h>
#include "io.c"
#include "timer.h"
#include "scheduler.h"
#include "usart.h"

unsigned char b1 = 0x00;			// value of portB (buttons)
unsigned char b1_2 = 0x00;
unsigned char correct = 0;			// flag correct button press
unsigned char wait = 0;				// wait in current state 
unsigned char score = 0x00;			// add 1 if answered correct
unsigned char finished = 0;			// flag finish round1
unsigned char startGame=0, round1end=0, round2end=0, gameEnd=0; 
enum states1 {reset,init,start,q1,disp1,q2,disp2,q3,disp3,end} state1;
enum states2 {reset2,init_2,start_2,q1_2,disp1_2,q2_2,disp2_2,q3_2,disp3_2,end_2} state2;
enum states3 {welcome,round1,round2,win,lose} state3;
	
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}
	
	
unsigned char print[16];
char * string; 
unsigned char cnt = 0;
void message(char * s, unsigned char c){
	for (int i=0; i<16; ++i){
		print[i] = s[(cnt+i)%c];
	}

	cnt = (cnt+1)%c;
	LCD_DisplayString(1, print);
	//return state;
}


void fill() {
	LCD_Cursor(17); LCD_WriteData('+');
	LCD_Cursor(19); LCD_WriteData('=');
	LCD_Cursor(21); LCD_WriteData('-');
	LCD_Cursor(23); LCD_WriteData('<');
	LCD_Cursor(25); LCD_WriteData('*');
	LCD_Cursor(27); LCD_WriteData('/');
	LCD_Cursor(29); LCD_WriteData('>');
	LCD_Cursor(31); LCD_WriteData('-');
}
/*  Tick function for round 1: displays 3 consecutive questions on LCD
	and only moves on to next if current is answered correCtly.
	Input taken from two buttons. 
*/
void tick1() {
//int tick1(state1){
	b1 = ~PINA & 0x0F;
	//wait=0; 
	switch(state1) {		// Transitions
		case reset: 
			if(b1==0x08) state1=init; 
			else state1=reset; 
		break;
		case init:
			if( wait<20) {++wait; state1=init; startGame = 1;}
			else {state1=start; wait=0; }
		break; 
		case start:
			if(b1==0x08) state1 = reset;
			if(wait<20) {++wait; state1=start;}
			else {state1=q1; wait=0; }
		break; 
		case q1:
			if(b1==0x04) {state1 = disp1;score++;  wait=0; }
			else if(b1==0x02) {state1 = disp1; wait=0; }
			else if(b1==0x08) {state1 = reset; wait=0;  }
			else if(wait > 50) {state1=disp1; wait=0; }
			else {state1 = q1; ++wait; }
		break;
		case disp1: 
			if(b1==0x08) state1 = reset;
			if(wait<20) {++wait; state1=disp1;}
			else {state1=q2; wait=0; }
		break;
		case q2: 
			if(b1==0x02) {state1 = disp2; score++; wait=0; } 
			else if(b1==0x04) {state1 = disp2; wait=0; } 
			else if(b1==0x08) { state1 = reset; wait=0; }
			else if(wait > 50) {state1=disp2; wait=0; }
			else { state1 = q2; ++wait; }
		break;
		case disp2:
			if(b1==0x08) state1 = reset;
			if(wait<20) {++wait; state1=disp2;}
			else {state1=q3; wait=0; }
		break;
		case q3:
			if(b1==0x04) {state1 = disp3; score++; wait=0; }
			else if(b1==0x02) {state1 = disp3; wait=0; }
			else if(b1==0x08) {state1 = reset; wait=0; }
			else if(wait > 50) {state1=disp3; wait=0; }
			else { state1=q3; ++wait; }
		break;
		case disp3:
			if(b1==0x08) state1 = reset;
			if(wait<20) {++wait; state1=disp3;}
			else {state1=end; wait=0; }
		break;
		case end: 
			if(b1==0x08) state1 = reset;
			else state1 = end; 
		break;  
		default: state1 = init;break;
	}
	
	switch(state1) {		//Actions
		case reset:
			LCD_DisplayString(1,"+ > * / - = + <");
			LCD_Cursor(18); LCD_WriteData('-');
			LCD_Cursor(20); LCD_WriteData('=');
			LCD_Cursor(22); LCD_WriteData('+');
			LCD_Cursor(24); LCD_WriteData('<');
			LCD_Cursor(26); LCD_WriteData('*');
			LCD_Cursor(28); LCD_WriteData('/');
			LCD_Cursor(30); LCD_WriteData('>');
			LCD_Cursor(32); LCD_WriteData('=');
		break;
		case init: 
			LCD_ClearScreen();
			LCD_DisplayString(1, "QUICK MATHS"); // with 3 questions each. You will have 5 seconds to answer each question.
			fill();
			score=0x00;
		break; 
		case start:
			LCD_ClearScreen(); 
			LCD_DisplayString(1, "QUESTION 1");
			fill();
		break;
		case q1: 
			if(b1==0x00) {
				LCD_ClearScreen();
				LCD_DisplayString(1, "2+2=");
				LCD_Cursor(17); 
				LCD_WriteData(8+'0');
				LCD_Cursor(25);
				LCD_WriteData(4+'0');
			}
			
		break; 
		case disp1:
			LCD_ClearScreen();
			LCD_DisplayString(1, "QUESTION 2");
			fill();
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
		break;
		case disp2:
			LCD_ClearScreen();
			LCD_DisplayString(1, "QUESTION 3");
			fill();
		break;
		case q3:
			if(b1==0x00) {
				LCD_ClearScreen();
				LCD_DisplayString(1, "7*3+15 / 4=");
				LCD_Cursor(17);
				LCD_WriteData(6+'0');
				LCD_Cursor(25);
				LCD_WriteData(9+'0');
			}
		break;
		case disp3:
			LCD_ClearScreen();
			LCD_DisplayString(1, "END ROUND1");
			fill();
		break;	
		case end: 
			finished=1; 
		break;
	}
	//return state1;
}

/*  Tick function for round2. Displays two questions on separate lines of LCD. 
	If top equation is true, push joystick up, if false, push 
	joystick down. 
*/
void tick2() {
	unsigned short x=ADC;
	b1_2=~PINA & 0x0F; 
	switch(state2) {			// Transitions
		case reset2: 
			if(b1==0x08) {state2 = init_2; }
			else state2=reset2;
		case init_2:
			if(wait<20) {++wait; state2=init_2;}
			else if(b1_2==0x08) {state2 = reset2; wait=0; }
			else {state2=start_2; wait=0; }
		break;
		case start_2:
			if(wait<20) {++wait; state2=start_2;}
			else if(b1_2==0x08) {state2 = reset2; wait=0; }
			else {state2=q1_2; wait=0; }
		break;
		case q1_2:
			if(ADC > 900) { state2=disp1_2; wait=0; }					//joystick up
			else if(ADC < 500) {score++; state2=disp1_2; wait=0; }		//joystick down
			else if(wait > 75) {state2=disp1_2; wait=0; }
			else if(b1_2==0x08) {state2 = reset2; wait=0; }
			else {state2 = q1_2; ++wait;}
		break;
		case disp1_2: 
			if(wait<20) {++wait; state2=disp1_2;}
			else if(b1_2==0x08) {state2 = reset2; wait=0; }
			else {state2=q2_2; wait=0; }
		break;
		case q2_2:
			if(ADC > 900) {score++; state2=disp2_2; wait=0; }			//joystick up
			else if(ADC < 500) {state2=disp2_2;wait=0;  }				//joystick down
			else if(wait > 75) {state2=disp2_2; wait=0; }
			else if(b1_2==0x08) {state2 = reset2; wait=0; }
			else {state2 = q2_2; ++wait; }
		break;
		case disp2_2:
			if(wait<20) {++wait; state2=disp2_2;}
			else if(b1_2==0x08) {state2 = reset2; wait=0; }
			else {state2=q3_2; wait=0; }
		break;
		case q3_2:
			if(ADC > 900) {state2=disp3_2; wait=0; }					//joystick up
			else if(ADC < 500) {score++; state2=disp3_2; wait=0; }		//joystick down
			else if(wait > 75) {state2=disp3_2; wait=0; }
			else if(b1_2==0x08) {state2 = reset2; wait=0; }
			else { state2 = q3_2; ++wait; } 
		break;
		case disp3_2:
			if(wait<20) {++wait; state2=disp3_2;}
			else if(b1_2==0x08) {state2 = reset2; wait=0; }
			else {state2=end_2; wait=0; }
		break;
		case end_2:
			if(b1_2==0x08) {state2 = reset2; }
			else state2 = end_2;
		break;
		default: state2 = init_2;break;
	}	
	switch(state2) {			// Actions
		case reset2: 
			LCD_DisplayString(1,"+ > * / - = + <");
			LCD_Cursor(18); LCD_WriteData('-');
			LCD_Cursor(20); LCD_WriteData('=');
			LCD_Cursor(22); LCD_WriteData('+');
			LCD_Cursor(24); LCD_WriteData('<');
			LCD_Cursor(26); LCD_WriteData('*');
			LCD_Cursor(28); LCD_WriteData('/');
			LCD_Cursor(30); LCD_WriteData('>');
			LCD_Cursor(32); LCD_WriteData('=');
		break;
		case init_2: 
			LCD_ClearScreen();
			LCD_DisplayString(1,"ROUND 2");
			fill();
		break;
		case start_2: 
			LCD_ClearScreen();
			LCD_DisplayString(1,"QUESTION 1");
			fill();
		break;
		case q1_2:
			LCD_ClearScreen();
			LCD_DisplayString(2, "32*3 > 61*2");
		break;
		case disp1_2:
			LCD_ClearScreen();
			LCD_DisplayString(1, "QUESTION 2");
			fill();
		break;
		case q2_2:
			LCD_ClearScreen();
			LCD_DisplayString(1, "8*3-2/2 = 2+6+3");
		break;
		case disp2_2:
			LCD_ClearScreen();
			LCD_DisplayString(1, "QUESTION 3");
			fill();
		break;
		case q3_2:
			LCD_ClearScreen();
			LCD_DisplayString(1, "86*3+9 > 15*18+1");
		break;
		case disp3_2:
			LCD_ClearScreen();
			LCD_DisplayString(1, "END ROUND 2");
			fill();
		break;
		case end_2:
			LCD_ClearScreen();
			LCD_DisplayString(1, "DONE");
			fill();
			round2end=1;
		break;
	}
}

void tick3(unsigned char sc) {
	switch(state3) {	// Transitions
		
		case welcome:
			if(startGame) { state3=round1;}
			else {state3=welcome; }
		break;
		case round1:
			if(!finished) state3 = round1; 
			else state3 = round2; 
		break;
		case round2:
			if(!round2end) state3 = round2; 
			else if (sc==6) state3 = win; 
			else state3 = lose;  
		break;
		case win: 
			state3 = win;
		break;
		case lose: 
			state3 = lose; 
		break;
	}
	switch(state3) {	// Actions
		case welcome:
			//LCD_ClearScreen();
			string= "Quick Maths.Answer fast.Win=100%.";
			LCD_DisplayString(1,string);
			
		break;
		case round1:
			LCD_ClearScreen();
			string = "Use buttons to  select answer";
			//message(string,37);
			LCD_DisplayString(1,string);
		break;
		case round2:
			LCD_ClearScreen();
			string = "Use joystick to select T/F";
			//message(string,35);
			LCD_DisplayString(1,string);
		break;
		case win:
			LCD_ClearScreen();
			string = "WINNER :)";
			LCD_DisplayString(1,string);
		break;
		case lose: 
			LCD_ClearScreen();
			string = "NOT QUICK ENOUGH :(";
			LCD_DisplayString(1,string); 
		break;
	}
}

void scoreDisp(unsigned char finalscore) {
	if(finalscore==0) {
		PORTA = 0x04 | 0x08 | 0x02 ;
		PORTB = 0x01 | 0x02 | 0x04;
	}
	else if(finalscore==1) {
		PORTA = 0x08 ;
		PORTB = 0x04;
	}
	else if(finalscore==2) {
		PORTA = 0x04 | 0x08 | 0x01;
		PORTB = 0x01 | 0x02;
	}
	else if(finalscore==3) {
		PORTA = 0x01 | 0x04 | 0x08 ;
		PORTB = 0x04 | 0x02;
	}
	else if(finalscore==4) {
		PORTA = 0x08 | 0x02 | 0x01 ;
		PORTB = 0x04 ;
	}
	else if(finalscore==5) {
		PORTA = 0x04 | 0x02 | 0x01;
		PORTB = 0x02 | 0x04;
	}
	else if(finalscore==6) {
		PORTA = 0x01 | 0x02 | 0x04;
		PORTB = 0x01 | 0x02 | 0x04;
	}
}

int main(void)
{
	/* Transmitter 
	DDRB = 0x00; PORTB = 0xFF; 
	DDRA = 0x00; PORTA = 0xFF;
	*/
	// /* Receiver
	DDRB = 0xFF; PORTB = 0x00; 
	DDRA = 0xFF; PORTA = 0x00;
	// */ 
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	unsigned long int SMTick1_period = 20;
	
	TimerSet(100);
	TimerOn();
	LCD_init();
	ADC_init();
	initUSART(1);
	unsigned short i;
	unsigned char t = 0x00;
	unsigned char fs = 0x00;
	unsigned char send = 0x00; 
	while(1)
	{
		/* Transmitter 
		if(!finished) tick1();
		else tick2();
		*/
		
		//USART 
		send = score; 
		if(startGame) send = send | 0x80; 
		if(finished) send = send | 0x40; 
		if(round2end) send = send | 0x20;
		
		/*
		USART_Send(send,1);
		*/
 		if(USART_HasReceived(1)) {
 			fs = USART_Receive(1) & 0x0F;
 			scoreDisp(fs);
			t = USART_Receive(1) & 0x80;
			if(t) startGame=1; 
			t = USART_Receive(1) & 0x40; 
			if(t) finished=1;
			t = USART_Receive(1) & 0x20;
			if(t) round2end=1;
 		}
		
		// /* Receiver
		tick3(fs); 
		// */ 
		
		while (!TimerFlag);
		TimerFlag = 0;
	}
}




