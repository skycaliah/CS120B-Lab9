/*	Author: Skyler Saltos 
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *	Praciting use of concurrent SMs
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

static char blinkingLED;
static char threeLEDs;
static char toggleSpeaker;
enum SM1_States { SM1_Start,SM1_OnB3, SM1_OffB3 } SM1_State;

void BlinkingLEDSM() {
   
   switch(SM1_State) { // Transitions
      case SM1_Start:
         SM1_State = SM1_OnB3;
         break;
      case SM1_OnB3:
            SM1_State = SM1_OffB3;
         break;
      case SM1_OffB3:
            SM1_State = SM1_OnB3;
         break;
      default:
         SM1_State = SM1_Start;
      } // Transitions

   switch(SM1_State) { // State actions
      case SM1_OnB3:
         blinkingLED = 0x08;
         break;
      case SM1_OffB3:
         blinkingLED = 0x00;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

enum SM2_States { SM2_Start,SM2_LightB0, SM2_LightB1, SM2_LightB2 } SM2_State;

void ThreeLEDsSM() {
   
   switch(SM2_State) { // Transitions
      case SM2_Start:
         SM2_State = SM2_LightB0;
         break;
      case SM2_LightB0:
            SM2_State = SM2_LightB1;
         break;
      case SM2_LightB1:
            SM2_State = SM2_LightB2;
         break;
      case SM2_LightB2:
            SM2_State = SM2_LightB0;
         break;
      default:
         SM2_State = SM2_Start;
      } // Transitions

   switch(SM2_State) { // State actions
      case SM2_LightB0:
         threeLEDs = 0x01;
         break;
      case SM2_LightB1:
         threeLEDs = 0x02;
         break;
      case SM2_LightB2:
         threeLEDs = 0x04;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

enum SM3_States { SM3_Start,SM3_s1 } SM3_State;

void CombineLEDsSM() {
   switch(SM3_State) { // Transitions
      case SM3_Start:
         SM3_State = SM3_s1;
         break;
      case SM3_s1:
            SM3_State = SM3_s1;
         break;
      default:
         SM3_State = SM3_Start;
      } // Transitions

   switch(SM3_State) { // State actions
      case SM3_s1:
	//PORTB = 0x08;
       	PORTB = threeLEDs | blinkingLED | toggleSpeaker;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}


enum SM4_States { SM4_Start,SM4_waitInput, SM4_buttonPress } SM4_State;
void SpeakerSM() {

	unsigned char A2 = PINA & 0x04;
   
   switch(SM4_State) { // Transitions
      case SM4_Start:
         SM4_State = SM4_waitInput;
         break;
      case SM4_waitInput:
         if (A2) {
           SM4_State = SM4_waitInput;
         }
         else if (!A2) {
            SM4_State = SM4_buttonPress;
         }
         break;
      case SM4_buttonPress:
         if (!A2) {
            SM4_State = SM4_buttonPress;
         }
         else if (A2) {
            SM4_State = SM4_waitInput;
         }
         break;
      default:
         SM4_State = SM4_Start;
      } // Transitions

   switch(SM4_State) { // State actions
      case SM4_waitInput:
	 toggleSpeaker = 0x00;
         break;
      case SM4_buttonPress:
         if(toggleSpeaker == 0x00){
         
         toggleSpeaker = 0x10;
         
         }//toggle on
         
         else if (toggleSpeaker == 0x10){
         
         toggleSpeaker = 0x00;
         
         }//toggle off
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}//end toggleSpeaker SM





int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF;	PORTB = 0x00; // PORTB as output
	DDRA = 0x00;	PORTA = 0xFF;
    /* Insert your solution below */
	SM1_State = SM1_Start;
	SM2_State = SM2_Start;
	SM3_State = SM3_Start;
	SM4_State = SM4_Start;

	unsigned long SM1_time = 1000;
	unsigned long SM2_time = 300;
	unsigned long SM3_time = 1; // change to one due to updated code with speaker
	unsigned long SM4_time = 2;
	const unsigned long period = 1;


	TimerSet(period);
	TimerOn();

    while (1) {

	if(SM1_time >= 1000){

		BlinkingLEDSM();
		SM1_time = 0;
	}//one tick of first SM
	if(SM2_time >= 300){

		ThreeLEDsSM();
		SM2_time = 0;
	}//one tick of second SM

	if(SM3_time >= 1){
		CombineLEDsSM();
		SM3_time = 0;
	}//one tick of thrid SM

	if(SM4_time >= 2){
		SpeakerSM();
		SM4_time = 0;

	}//one tick for foruth SM

	

	while(!TimerFlag); // wait 1 msec before continuing
	TimerFlag = 0;
	SM1_time += period;
	SM2_time += period;
	SM3_time += period;
	SM4_time += period;
    }
    return 1;
}
