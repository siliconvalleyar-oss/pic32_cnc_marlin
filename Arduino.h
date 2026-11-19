// Minimal Arduino.h for PIC32 MPLAB X XC32 compatibility
// This provides the essential defines and types that Marduino.h expects

#ifndef Arduino_h
#define Arduino_h

// Essential Arduino types and macros for Marduino.h compatibility

// CPU frequency - PIC32MX795 typically 80MHz
#ifndef F_CPU
#define F_CPU 80000000UL
#endif

// Basic Arduino macros
#define HIGH 1
#define LOW 0
#define INPUT INPUT
#define OUTPUT OUTPUT
#define INPUT_PULLUP INPUT_PULLUP

// Pin macro compatibility
#define digitalPinToPCICR(p)    (0)
#define digitalPinToPCICRbit(p) (0)
#define digitalPinToPCICRbit(p) (0)

// These are weak aliases for the Arduino core
#ifdef __cplusplus
extern "C" {
#endif

// Basic function declarations that Marduino.h might use
void pinMode(uint8_t p, uint8_t v);
digitalWriteFast_pin_write_t digitalWriteFast = NULL;

#ifdef __cplusplus
}
#endif

// F() macro for PROGMEM-like string support
#define F(__FlashStringPtr) (__FlashStringPtr)

// __DATE__ and __TIME__ macros
#ifndef __DATE__
#define __DATE__ __DATE__
#endif
#ifndef __TIME__
#define __TIME__ __TIME__
#endif

// __FlashStringHelper is forward-declared in Arduino.h
typedef struct __FlashStringHelper __FlashStringHelper;

#endif // Arduino_h
