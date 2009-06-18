#ifndef ARDUINO_EXT_H
#define ARDUINO_EXT_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "wiring.h"

// mmm 9615.4 Hz...
#define F_ADC_SAMPLE ((float) F_CPU / (128*13))

// don't nest these! or analogRead()!
void analogStart(uint8_t pin, void (*callback)(int));
void analogStop(void);

// don't use analogWrite() with these!
void modulatedWrite(uint8_t pin, uint8_t val);
void endModulatedWrite(uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif
