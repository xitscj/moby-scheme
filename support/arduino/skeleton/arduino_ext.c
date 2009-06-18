#include <stdlib.h>
#include "wiring_private.h"
#include "pins_arduino.h"

// thankfully they didn't declare this static
extern uint8_t analog_reference;

static void (*callback)(int) = NULL;

SIGNAL(ADC_vect) {
	if (callback != NULL) {
		// roughly the second half of analogRead()
		uint8_t low, high;
		low = ADCL;
		high = ADCH;
		callback((high << 8) | low);
	}
}

void analogStart(uint8_t pin, void (*f)(int)) {
	callback = f;
	
	// roughly the first half of analogRead()
	ADMUX = (analog_reference << 6) | (pin & 0x07);
#if defined(__AVR_ATmega1280__)
	ADCSRB = (ADCSRB & ~((1 << MUX5) | (7 << ADTS0)) | (((pin >> 3) & 0x01) << MUX5) | (0 << ADTS0);
#else
	ADCSRB = (ADCSRB & ~(7 << ADTS0)) | (0 << ADTS0);
#endif
	ADCSRA |= _BV(ADATE) | _BV(ADIE);
	sbi(ADCSRA, ADSC);
}

void analogStop(void) {
	callback = NULL;
	cbi(ADCSRA, ADATE);
}

#define F_MODULATION 56000
#define P_MODULATION ((F_CPU/F_MODULATION+1))/2

// Note: you should always call endModulatedWrite
// after done with modulatedWrites in order to
// allow PWM to work correctly again.

void modulatedWrite(uint8_t pin, uint8_t val) {
	pinMode(pin, OUTPUT);
	
	// change timer 1 prescale to 1... *shouldn't* mess anything up...
	cbi(TCCR1B, CS11);
	sbi(TCCR1B, CS10);
	
	// set up timer mode...
	sbi(TCCR1B, WGM13);
	cbi(TCCR1B, WGM12);
	sbi(TCCR1A, WGM11);
	cbi(TCCR1A, WGM10);
	
	// set up modulation frequency...
	ICR1 = P_MODULATION;
	
	if (digitalPinToTimer(pin) == TIMER1A) {
		// connect pwm to pin on timer 1, channel A
		sbi(TCCR1A, COM1A1);
		// set pwm duty
		OCR1A = val ? P_MODULATION / 2 : 0;
	} else if (digitalPinToTimer(pin) == TIMER1B) {
		// connect pwm to pin on timer 1, channel B
		sbi(TCCR1A, COM1B1);
		// set pwm duty
		OCR1B = val ? P_MODULATION / 2 : 0;
	}
	else digitalWrite(pin, val);
}

void endModulatedWrite(uint8_t pin) {
	// back to Arduino default...
	sbi(TCCR1B, CS11);
	sbi(TCCR1B, CS10);
	
	cbi(TCCR1B, WGM13);
	cbi(TCCR1B, WGM12);
	cbi(TCCR1A, WGM11);
	sbi(TCCR1A, WGM10);
	
	if (digitalPinToTimer(pin) == TIMER1A)
		cbi(TCCR1A, COM1A1);
	else if (digitalPinToTimer(pin) == TIMER1B)
		cbi(TCCR1A, COM1B1);
}
