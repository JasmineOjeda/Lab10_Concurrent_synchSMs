/*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Lab 10  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtube.com/shorts/BA0-uJDpoVs?feature=share
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
	_avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

unsigned char threeLEDs;
unsigned char blinkingLED;
unsigned char speaker;

unsigned char frequency;

enum SpeakerStates {SPEAKER_START, WAIT, PLAY} speakerState;
unsigned long i = 0;
void SpeakerSM() {
    unsigned char tmp_A = (~PINA) & 0x04;
    
    switch (speakerState) {
        case SPEAKER_START:
	    i = 0;
	    speakerState = WAIT;
	    break;
	case WAIT:
            if (tmp_A) {
                speakerState = PLAY;
	    }
	    else {
		speakerState = WAIT;
	    }
	    break;
	case PLAY:
	    if (tmp_A) {
                speakerState = PLAY;
            }
	    else {
		i = 0;
                speakerState = WAIT;
	    }
	    break;
	default: break;
    }

    switch (speakerState) {
        case SPEAKER_START:
            speaker = 0x00;
            break;
        case WAIT:
            speaker = 0x00;
            break;
        case PLAY:
	    if (i == frequency) {
                speaker = ~speaker & 0x10;
		i = 0;
	    }
	    i++;
            break;
        default: break;
    }
}

enum ThreeLEDStates {THREE_LEDS_START, ONE, TWO, THREE} threeLEDState;

void ThreeLEDsSM() {
    switch (threeLEDState) {
        case THREE_LEDS_START:
            threeLEDState = ONE;
	    break;
	case ONE:
	    threeLEDState = TWO;
	    break;
	case TWO:
	    threeLEDState = THREE;
	    break;
	case THREE:
	    threeLEDState = ONE;
        default: break;
    }

    switch (threeLEDState) {
        case THREE_LEDS_START:
            threeLEDs = 0x00;
            break;
        case ONE:
            threeLEDs = 0x01;
            break;
        case TWO:
            threeLEDs = 0x02;
            break;
        case THREE:
            threeLEDs = 0x04;
        default: break;

    }
}

enum BlinkingStates {BLINKING_START, ON, OFF} blinkingState;

void BlinkingLEDSM() {
    switch (blinkingState) {
        case BLINKING_START:
	    blinkingState = ON;
	    break;
	case ON:
	    blinkingState = OFF;
	    break;
	case OFF:
	    blinkingState = ON;
	    break;
	default: break;
    }

    switch (blinkingState) {
        case BLINKING_START:
            blinkingLED = 0x00;
            break;
        case ON:
            blinkingLED = 0x08;
            break;
        case OFF:
            blinkingLED = 0x00;
            break;
        default: break;
    }
}

enum CombineLEDStates {COMBINE_START, COMBINE} combineState;

void CombineLEDsSM() {
    switch (combineState) {
        case COMBINE_START:
	    combineState = COMBINE;
	    break;
	case COMBINE:
	    combineState = COMBINE;
	    break;
	default: break;
    }

    switch (combineState) {
        case COMBINE_START:
            break;
        case COMBINE:
            PORTB = (~(threeLEDs | blinkingLED) & 0x0F) | speaker;
            break;
        default: break;
    }
}

enum FrequencyState {FREQ_START, STAY, PUSHED} freqState;

void FrequencySM() {
    unsigned char tmp_A = (~PINA) & 0x03;

    switch (freqState) {
        case FREQ_START:
	    freqState = STAY;
	    break;
	case STAY:
            if (tmp_A == 0x01) {
		frequency--;
                freqState = PUSHED;
	    }
	    else if (tmp_A == 0x02) {
                frequency++;
		freqState = PUSHED;
	    }
	    else {
                freqState = STAY;
	    }
	    break;
	case PUSHED:
	    if (tmp_A == 0x00) {
                freqState = STAY;
	    }
	    else {
                freqState = PUSHED;
            }
	    break;
	default: break;
    }    
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
    DDRA = 0x00; PORTA = 0xFF;

    TimerSet(1);
    TimerOn();

    PORTB = 0x00;
    threeLEDs = 0x00;
    blinkingLED = 0x00;
    speaker = 0x00;

    frequency = 2;

    unsigned int threeLEDCount = 300;
    unsigned int blinkingCount = 1000;

    threeLEDState = THREE_LEDS_START;
    blinkingState = BLINKING_START;
    combineState = COMBINE_START;
    speakerState = SPEAKER_START;
    freqState = FREQ_START;

    /* Insert your solution below */
    while (1) {
        if (threeLEDCount == 300) {
            ThreeLEDsSM();
	    threeLEDCount = 0;
	}

	if (blinkingCount == 1000) {
	    BlinkingLEDSM();
	    blinkingCount = 0;
	}
        
	FrequencySM();
	SpeakerSM();
	CombineLEDsSM();
        
	threeLEDCount++;
	blinkingCount++;

        while (!TimerFlag) { };
	TimerFlag = 0;
    }
    return 1;
}
