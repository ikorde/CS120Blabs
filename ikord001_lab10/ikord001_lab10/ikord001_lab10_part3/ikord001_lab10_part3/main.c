/*    Partner(s) Name & E-mail:
 *	   Ishita Korde ikord001@ucr.edu 
 *	   Yuteng Zhang yzhan399@ucr.edu
 *	  
 *    Lab Section: 021
 *    Assignment: Lab #10  Exercise #1
 *    
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A

	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

// SynchSM Code
enum States_1 { L0_1,L1_1,L2_1} state_1;
enum States_2 { L0_2,L1_2} state_2;
enum States_3 { L0_3, L1_3} state_3;

//unsigned char tmpA = 0x00;
unsigned char threeLEDS = 0x00;
unsigned char blinkingLED = 0x00;
unsigned char speakerON = 0x00;

void Tick_1()
{
	//tmpA = PINA & 0x01;
	
	switch (state_1)	//transitions
	{
		
		case L0_1:
		state_1=L1_1;
		break;
		
		case L1_1:
		state_1=L2_1;
		break;

		case L2_1:
		state_1=L0_1;
		break;
		
		default:
		break;
	}
	
	switch (state_1)	//actions
	{
	
		case L0_1:
		threeLEDS = 0x01;
		break;
		
		case L1_1:
		threeLEDS = 0x02;
		break;

		case L2_1:
		threeLEDS = 0x04;
		break;
		
		default:
		break;
	}
}

void Tick_2()
{
	//tmpA = PINA & 0x01;
	
	switch (state_2)
	{
		
		case L0_2:
		state_2=L1_2;
		break;
		
		case L1_2:
		state_2=L0_2;
		break;
		
		default:
		break;
	}
	
	switch (state_2)
	{
		
		case L0_2:
		blinkingLED = 0x08;
		break;
		
		case L1_2:
		blinkingLED = 0x00;
		break;
		
		default:
		break;
	}
}


void Tick_speaker() {
	switch (state_3) {
		case L0_3:
		state_3 = L1_3;
		break; 

		case L1_3: 
		state_3 = L0_3;
		break; 
	}
	switch (state_3) {
		case L0_3:
		speakerON =0x00;
		break; 

		case L1_3: 
		if(PINA == 0x04) {speakerON = 0x10;}
		else speakerON = 0x00;
		break; 
	}

}


int main(void){
	
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s

	unsigned long three_elapsedTime = 300;
	unsigned long blink_elapsedTime = 1000; 
	unsigned long speaker_elapsedTime = 2;
	const unsigned long timerPeriod = 1; 

	TimerSet(timerPeriod);
	TimerOn();

	state_1 = L0_1;
	state_2 = L0_2;
	state_3 = L0_3; 
	while(1) {
		// User code (i.e. synchSM calls)
		if(three_elapsedTime >= 300) {
			Tick_1(); 
			three_elapsedTime = 0; 
		}
		if(blink_elapsedTime >= 1000) {
			Tick_2(); 
			blink_elapsedTime = 0; 
		}
		if(speaker_elapsedTime >= 2) {
			Tick_speaker(); 
			speaker_elapsedTime = 0; 
		}

		PORTB = threeLEDS | blinkingLED | speakerON;

		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;	
		three_elapsedTime += timerPeriod;
		blink_elapsedTime += timerPeriod; 	
		speaker_elapsedTime += timerPeriod; 
		
	}
}