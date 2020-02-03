#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>
#include <util/setbaud.h>
#include <Arduino.h>
#include <lib.h>
#include "SoftPWM.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 3
#define CS_PIN 2
#define RED 4
#define GREEN 5

class Radio {

public:
    RF24 instance;

};

class Motor {

public:
    int power;
};

const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};

int radio_number = ROLE;
int role = ROLE;

int main(void) {

    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);

    uart_init();
    SoftPWMBegin();

//    SoftPWMSetFadeTime(4, 2000, 2000);
//    pinMode(15, OUTPUT);
//
//    pinMode(16, OUTPUT);
//    pinMode(17, OUTPUT);
//
//    pinMode(18, OUTPUT);
//    pinMode(19, OUTPUT);


    RF24 radio(CE_PIN, CS_PIN);
    radio.begin();

    if (radio_number) {
        radio.openWritingPipe(pipes[1]);
        radio.openReadingPipe(1, pipes[0]);
    } else {
        radio.openWritingPipe(pipes[0]);
        radio.openReadingPipe(1, pipes[1]);
    }

    radio.startListening();


    //SoftPWMSet(16, 255);
    //SoftPWMSet(19, 255);


    //digitalWrite(18, HIGH);
    // SoftPWMSet(18, 255, 1);
//    digitalWrite(19, LOW);

//    digitalWrite(14, LOW);
//    digitalWrite(15, HIGH);

    digitalWrite(GREEN, HIGH);
    _delay_ms(200);
    digitalWrite(GREEN, LOW);

    int accum = 20;
    int i = 5;
    for (;;) {

        unsigned long start_time = micros();

        // ping
        if (role == 1) {


            if (accum > 230) {
                i = -5; accum = 230;
            } else if (accum < 100) {
                i = 5; accum = 100;
            }
            accum += i;

        //    SoftPWMSet(14, accum);
           // SoftPWMSet(19, accum);
            _delay_ms(100);
//
////            digitalWrite(GREEN, HIGH);
////            _delay_ms(200);
////            digitalWrite(GREEN, LOW);
//            radio.stopListening();
//
//
//            if (!radio.write(&start_time, sizeof(unsigned long))) {
////                digitalWrite(RED, HIGH);
////                _delay_ms(1000);
////                digitalWrite(RED, LOW);
//                radio.startListening();
//                continue;
//            }
//
//            radio.startListening();
//            unsigned long started_waiting_at = micros();
//            boolean timeout = false;
//
//            _delay_ms(390);
//            if (!radio.available()) {
//                timeout = true;
//            }
////            while (!radio.available()) {
////                if (micros() - started_waiting_at > 20) {
////                    timeout = true;
////                    break;
////                }
////            }
//            if (timeout) {
//
////                digitalWrite(RED, HIGH);
////                _delay_ms(200);
////                digitalWrite(RED, LOW);
//
//            } else {
//                unsigned long got_time;
//                radio.read(&got_time, sizeof(unsigned long));
//                unsigned long end_time = micros();
//
////                digitalWrite(GREEN, HIGH);
////                _delay_ms(200);
////                digitalWrite(GREEN, LOW);
//            }


            //_delay_ms(500);

        }

        // ping bag 
        if (role == 0) {
            _delay_ms(100);
            unsigned long got_time;
            if (radio.available()) {
                while (radio.available()) {
                    radio.read(&got_time, sizeof(unsigned long));
                }
                radio.stopListening();
                radio.write(&got_time, sizeof(unsigned long));
                radio.startListening();
            }
        }
    }
}

