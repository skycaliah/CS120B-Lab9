#include <avr/interrupt.h>


volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

//Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal countt of 1ms ticks


void TimerOn() {

// AVR timer/counter controller register TCCR1
 TCCR1B = 0x0B; // bit3 = 0; CTC mode (clear timer on compare)
		// bit2bit1bit0 = 011: pre-scaler / 64
		// 00001011: 0x0B
		// So, 8 MHz clock or 8,000,000 / 64 = 125,000 tick/s
		// Thus, TCNT1 register will count at 125,000 tick/s

// AVR output compare register OCRIA. 
OCR1A = 125;	// Timer interrupt will be generated when TCNT1 == OCRIA
		// We want a 1 ms tick. 0.001 s * 125, 000 tick/s - 125
		// So when TCNT1 register == 125
		// 1 ms has passed. Thus, we compare to 125.

//AVR timer interrupt mask register
TIMSK1 = 0x02; // bit1: OCTE1A -- enables compare match interrupt


//Initialize avr counter
TCNT1 = 0;

_avr_timer_cntcurr = _avr_timer_M; 
// TimerISR will be called every _avr_timer_cntcurr ms


//Enable global interrputs
SREG |= 0x80; // 0x80: 1000000



}//end TimerOn() function




//The "enable global interrupts" line is SREG |= 0x80, not SREG |- 0x80;


void TimerOff() {

	TCCR1B = 0x00; // lower three bits all 0

}//end TimerOff()


void TimerISR() {
	
	TimerFlag = 1;

}//end TimerISR



// In our approach, the C programmer does not touch this ISR, but rather TimerISR()

ISR(TIMER1_COMPA_vect) {
	//CPU automatically call when TCNT1 == OCR1 ( every 1 ms per TimerOn settings) 
	
	_avr_timer_cntcurr--;  //Count down to 0 rather than up to TOP
	if ( _avr_timer_cntcurr == 0 ) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;

	}//end if 


}//end ISR



// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {

	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;

}//end TimerSet()
