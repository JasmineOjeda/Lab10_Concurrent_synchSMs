/*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Lab 10  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtube.com/shorts/5Wwy4Hd3a9s?feature=share
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
            PORTB = ~(threeLEDs | blinkingLED);
            break;
        default: break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(100);
    TimerOn();

    PORTB = 0x00;
    threeLEDs = 0x00;
    blinkingLED = 0x00;

    unsigned char threeLEDCount = 3;
    unsigned char blinkingCount = 10;

    threeLEDState = THREE_LEDS_START;
    blinkingState = BLINKING_START;
    combineState = COMBINE_START;

    /* Insert your solution below */
    while (1) {
        if (threeLEDCount == 3) {
            ThreeLEDsSM();
	    threeLEDCount = 0;
	}

	if (blinkingCount == 10) {
	    BlinkingLEDSM();
	    blinkingCount = 0;
	}
	CombineLEDsSM();
        
	threeLEDCount++;
	blinkingCount++;

        while (!TimerFlag) { };
	TimerFlag = 0;
    }
    return 1;
}
