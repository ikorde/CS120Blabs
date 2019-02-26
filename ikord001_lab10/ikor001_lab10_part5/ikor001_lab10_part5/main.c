

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
	OCR1A = 125;    // Timer interrupt will be generated when TCNT1==OCR1A
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



enum States{wait, increase, decrease,reset} state;

unsigned char button1;
unsigned char button2;
unsigned char tmpB = 0x00;
unsigned char count = 0;
unsigned long x = 1000;
unsigned char c1 = 0;
unsigned char c2 = 0;

void button_Tick(){
	button1  = ~PINA & 0x01;
	button2 = ~PINA & 0x02;
	
	switch(state){ // Transitions
		case wait:
		if(button1 == 0x01 && button2 != 0x02){
			state = increase;
			c1+=1;
			if(c1 >= 3){x = 10;TimerSet(30);}
		}
		else if(button1 != 0x01 && button2 ==0x02){
			state = decrease;
			c2+=1;
			if(c2 >= 3){x = 10;TimerSet(30);}
		}
		else if(button1==0x01 && button2==0x02){
			state = reset;
			c1 = 0;
			c2 = 0;
			TimerSet(100);
		}
		else
		{ state = wait;
			c1 = 0;
			c2=0; 
			TimerSet(100);

		}
		break;
		
		case increase:
		if(button1==0x01 && button2!=0x02 ) {
			state = increase;
			c1+=1;
			if(c1>=3){x = 30;TimerSet(30);}
		}
		
		else if(button1==0x01 && button2==0x02){
			state = reset;
		}
		else
		state = wait;
		break;
		
		
		case decrease:
		if(button1!=0x01 && button2==0x02){
			state = decrease;
			c2+=1;
			if(c2>=3){x = 30;TimerSet(30);}
		}
		else if(button1==0x01 && button2==0x02){
			state = reset;
		}
		else
		state = wait;
		
		break;
		case reset:
		if(button1==0x01 && button2==0x02){
			state = reset;
		}
		else
		state = wait;
		break;
		
	}
	
	switch(state){ // State actions
		case wait:
		//tmpB = 0x01;
		break;
		
		case increase:
		if(tmpB < 0x09)
		tmpB = tmpB + 0x01;
		break;
		
		
		case decrease:
		if(tmpB > 0x00)
		tmpB = tmpB - 0x01;
		break;
		
		case reset:
		tmpB = 0x00;
		break;
		
	}
	PORTB = tmpB;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // A input initialize to 0xFF
	DDRB = 0xFF; PORTB = 0x00;

	unsigned long elapsedTime = 30;
	unsigned long timerPeriod = 100;
	
	TimerSet(timerPeriod);
	TimerOn();
	
	state = wait;
	tmpB = 0x00;
	
	while(1)
	{
		if(elapsedTime >= x) {
			button_Tick();
			elapsedTime = 0;
		}
		
		else {
			button_Tick();
			elapsedTime = 0;
		}
		
		
		while(!TimerFlag){}
		TimerFlag = 0;
		elapsedTime +=1;
		
	}
}

