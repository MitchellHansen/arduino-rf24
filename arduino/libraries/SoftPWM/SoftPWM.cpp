/*
|| @author         Brett Hagman <bhagman@wiring.org.co>
|| @contribution   Paul Stoffregen (paul at pjrc dot com)
|| @url            http://wiring.org.co/
|| @url            http://roguerobotics.com/
||
|| @description
|| | A Software PWM Library
|| | 
|| | Written by Brett Hagman
|| | http://www.roguerobotics.com/
|| | bhagman@roguerobotics.com, bhagman@wiring.org.co
|| |
|| | A Wiring (and Arduino) Library, for Atmel AVR8 bit series microcontrollers,
|| | to produce PWM signals on any arbitrary pin.
|| | 
|| | It was originally designed for controlling the brightness of LEDs, but
|| | could be adapted to control servos and other low frequency PWM controlled
|| | devices as well.
|| | 
|| | It uses a single hardware timer (Timer 2) on the Atmel microcontroller to
|| | generate up to 20 PWM channels (your mileage may vary).
|| | 
|| #
||
|| @license Please see the accompanying LICENSE.txt file for this project.
||
|| @notes
|| | Minor modification by Paul Stoffregen to support different timers.
|| |
|| #
||
|| @name Software PWM Library
|| @type Library
|| @target Atmel AVR 8 Bit
||
|| @version 1.0.1
||
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "SoftPWM.h"
#include "SoftPWM_timer.h"

#if defined(WIRING)
#include <wiring_private.h>
#elif ARDUINO >= 100
#include <Arduino.h>
#else

#include <Arduino.h>

#endif

#if F_CPU
#define SOFTPWM_FREQ 60UL
#define SOFTPWM_OCR (F_CPU/(8UL*256UL*SOFTPWM_FREQ))
#else
// 130 == 60 Hz (on 16 MHz part)
#define SOFTPWM_OCR 130
#endif

volatile uint8_t _isr_softcount = 0xff;
uint8_t _softpwm_defaultPolarity = SOFTPWM_NORMAL;

typedef struct {
    // hardware I/O port and pin for this channel
    int8_t pin;
    uint8_t polarity;
    volatile uint8_t *outport;
    uint8_t pinmask;
    uint8_t pwmvalue;
    uint8_t checkval;
    uint8_t fadeuprate;
    uint8_t fadedownrate;
} softPWMChannel;

softPWMChannel _softpwm_channels[SOFTPWM_MAXCHANNELS];


// Here is the meat and gravy
#ifdef WIRING
void SoftPWM_Timer_Interrupt(void)
#else
ISR(TIMER1_COMPA_vect)
#endif
{

    uint8_t i;
    int16_t newvalue;
    int16_t direction;

    if (++_isr_softcount == 0)
    {
        // set all channels high - let's start again
        // and accept new checkvals
        for (i = 0; i < SOFTPWM_MAXCHANNELS; i++) {

            // check on pwm, 0-255
            _softpwm_channels[i].checkval = _softpwm_channels[i].pwmvalue;

            // now set the pin high (if not 0)
            if (_softpwm_channels[i].checkval > 0)
            {
                if (_softpwm_channels[i].polarity == SOFTPWM_NORMAL)
                    *_softpwm_channels[i].outport |= _softpwm_channels[i].pinmask;
                else
                    *_softpwm_channels[i].outport &= ~(_softpwm_channels[i].pinmask);
            }
        }
    }

    for (i = 0; i < SOFTPWM_MAXCHANNELS; i++)
    {
        if (_softpwm_channels[i].pin >= 0)  // Only set un-flagged pins
        {
            if (_softpwm_channels[i].checkval == _isr_softcount)  // if we have hit the width
            {
                // turn off the channel
                if (_softpwm_channels[i].polarity == SOFTPWM_NORMAL)
                    *_softpwm_channels[i].outport &= ~(_softpwm_channels[i].pinmask);
                else
                    *_softpwm_channels[i].outport |= _softpwm_channels[i].pinmask;
            }
        }
    }
}


void SoftPWMBegin(uint8_t defaultPolarity) {
    // We can tweak the number of PWM period by changing the prescalar
    // and the OCR - we'll default to ck/8 (CS21 set) and OCR=128.
    // This gives 1024 cycles between interrupts.  And the ISR consumes ~200 cycles, so
    // we are looking at about 20 - 30% of CPU time spent in the ISR.
    // At these settings on a 16 MHz part, we will get a PWM period of
    // approximately 60Hz (~16ms).

    // Hand init
    cli();                   // Disable interrupts while setting registers
    TCCR1A = 0;              // Reset the timer
    TCCR1B = 0;              // Reset the timer
    TIMSK1 = (1 << OCIE1A);  // Enable interrupt
    TCCR1B = (1 << CS21);   /* start timer (ck/8 prescalar) */ \
    TCCR1A = (1 << WGM21);  /* CTC mode */ \
    OCR1A = SOFTPWM_OCR;     // compare value
    sei();

    for (uint8_t i = 0; i < SOFTPWM_MAXCHANNELS; i++) {
        _softpwm_channels[i].pin = -1;
        _softpwm_channels[i].polarity = SOFTPWM_NORMAL;
        _softpwm_channels[i].outport = 0;
        _softpwm_channels[i].fadeuprate = 0;
        _softpwm_channels[i].fadedownrate = 0;
    }

    _softpwm_defaultPolarity = defaultPolarity;
}


void SoftPWMSetPolarity(int8_t pin, uint8_t polarity) {
    uint8_t i;

    if (polarity != SOFTPWM_NORMAL)
        polarity = SOFTPWM_INVERTED;

    for (i = 0; i < SOFTPWM_MAXCHANNELS; i++) {
        if ((pin < 0 && _softpwm_channels[i].pin >= 0) ||  // ALL pins
            (pin >= 0 && _softpwm_channels[i].pin == pin))  // individual pin
        {
            _softpwm_channels[i].polarity = polarity;
        }
    }
}



void SoftPWMSet(uint8_t pin, uint8_t value, uint8_t hardset) {


    if (hardset) {
        SOFTPWM_TIMER_SET(0);
        _isr_softcount = 0xff;
    }

    int8_t firstfree = -1;  // first free index

    // If the pin isn't already set, add it
    for (uint8_t i = 0; i < SOFTPWM_MAXCHANNELS; i++) {
        if (_softpwm_channels[i].pin == pin)
        {
            // set the pin (and exit, if individual pin)
            _softpwm_channels[i].pwmvalue = value;

            if (pin >= 0) // we've set the individual pin
                return;
        }

        // get the first free pin if available
        if (firstfree < 0 && _softpwm_channels[i].pin < 0)
            firstfree = i;
    }

    if (firstfree >= 0) {
        // we have a free pin we can use
        _softpwm_channels[firstfree].pin = pin;
        _softpwm_channels[firstfree].polarity = _softpwm_defaultPolarity;
        _softpwm_channels[firstfree].outport = portOutputRegister(digitalPinToPort(pin));
        _softpwm_channels[firstfree].pinmask = digitalPinToBitMask(pin);
        _softpwm_channels[firstfree].pwmvalue = value;
//    _softpwm_channels[firstfree].checkval = 0;

        // now prepare the pin for output
        // turn it off to start (no glitch)
        if (_softpwm_defaultPolarity == SOFTPWM_NORMAL)
            digitalWrite(pin, LOW);
        else
            digitalWrite(pin, HIGH);
        pinMode(pin, OUTPUT);
    }
}

void SoftPWMEnd(uint8_t pin) {

    for (uint8_t i = 0; i < SOFTPWM_MAXCHANNELS; i++) {
        if (_softpwm_channels[i].pin == pin) {

            // Disable pin output
            digitalWrite(_softpwm_channels[i].pin, 1);
            pinMode(_softpwm_channels[i].pin, INPUT);

            // Flag as removed
            _softpwm_channels[i].pin = -1;
        }
    }
}

void SoftPWMEndAll() {

    for (uint8_t i = 0; i < SOFTPWM_MAXCHANNELS; i++) {

        // Disable pin output
        digitalWrite(_softpwm_channels[i].pin, 1);
        pinMode(_softpwm_channels[i].pin, INPUT);

        // Flag as removed
        _softpwm_channels[i].pin = -1;
    }
}


void SoftPWMSetFadeTime(int8_t pin, uint16_t fadeUpTime, uint16_t fadeDownTime) {
    int16_t fadeAmount;
    uint8_t i;

    for (i = 0; i < SOFTPWM_MAXCHANNELS; i++) {
        if ((pin < 0 && _softpwm_channels[i].pin >= 0) ||  // ALL pins
            (pin >= 0 && _softpwm_channels[i].pin == pin))  // individual pin
        {

            fadeAmount = 0;
            if (fadeUpTime > 0)
                fadeAmount = 255UL * (SOFTPWM_OCR * 256UL / (F_CPU / 8000UL)) / fadeUpTime;

            _softpwm_channels[i].fadeuprate = fadeAmount;

            fadeAmount = 0;
            if (fadeDownTime > 0)
                fadeAmount = 255UL * (SOFTPWM_OCR * 256UL / (F_CPU / 8000UL)) / fadeDownTime;

            _softpwm_channels[i].fadedownrate = fadeAmount;

            if (pin >= 0)  // we've set individual pin
                break;
        }
    }
}
