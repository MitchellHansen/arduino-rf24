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

    RF24 radio(CE_PIN, CS_PIN);
    radio.begin();
    radio.openWritingPipe(remote_pipe);
    radio.openReadingPipe(1, dozer_pipe);
    radio.startListening();

    bool breaking = false;
    while (!breaking) {

        _delay_ms(100);

        // Get input

        // Parse into a struct
        StickValues stick_values;
        stick_values.stick_1_a = 100;
        stick_values.stick_1_b = 50;
        stick_values.stick_2_a = 0;
        stick_values.stick_2_b = 0;

        // Send to dozer
        radio.stopListening();
        radio.write(&stick_values, sizeof(uint32_t)*4);
        radio.startListening();
    }
}

// Dot really ever have to read
//if (radio.available()) {
//    while (radio.available()) {
//        radio.read(&got_time, sizeof(unsigned long));
//    }
//}