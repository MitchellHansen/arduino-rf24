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


const uint64_t dozer_pipe = 0xF0F0F0F0D2LL;
const uint64_t remote_pipe = 0xF0F0F0F0E1LL;

struct StickValues {
    uint32_t stick_1_a;
    uint32_t stick_1_b;
    uint32_t stick_2_a;
    uint32_t stick_2_b;
};

int main() {

    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);

    uart_init();
    SoftPWMBegin();

    RF24 radio(CE_PIN, CS_PIN);
    radio.begin();
    radio.openWritingPipe(dozer_pipe);
    radio.openReadingPipe(1, remote_pipe);
    radio.startListening();

    //digitalWrite(18, HIGH);
    // SoftPWMSet(18, 255, 1);


    digitalWrite(GREEN, HIGH);
    _delay_ms(2000);
    digitalWrite(GREEN, LOW);

    bool breaking = false;
    int cycles = 0;
    while (!breaking) {

        //unsigned long start_wait = micros();
        bool timeout = false;

        while (!radio.available()) {
            if (cycles++ > 10) {
                digitalWrite(RED, HIGH);
                _delay_ms(50);
                digitalWrite(RED, LOW);
                _delay_ms(50);
                timeout = true;
                cycles = 0;
                break;
            }

        }

        if (timeout) {
//            SoftPWMSet(17, 0);
//            SoftPWMSet(18, 0);
        } else {

            StickValues stick_values;
            radio.read(&stick_values, sizeof(uint32_t)*4);
            radio.flush_rx();
            radio.flush_tx();

//            SoftPWMSet(17, stick_values.stick_1_a);
//            SoftPWMSet(18, stick_values.stick_1_b);
        }

        SoftPWMSet(17, 80);
        SoftPWMSet(18, 120);
    }
}
