#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>
#include <util/setbaud.h>
#include <Arduino.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#define CE_PIN 7
#define CE_PIN 3
//#define CS_PIN 8
#define CS_PIN 2

#define LED PORTD2
#define PULL PORTD3
#define ENABLE PORTD5
#define DIRECTION PORTD6

void uart_init() {
    // Upper and lower bytes of the calculated prescaler value for baud.
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // Configure data frame size to 8-bits.
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

    // Configure to enable transmitter.
    UCSR0B = _BV(TXEN0);
}

void uart_putchar(char c) {
    // Wait until the register to write to is free.
    loop_until_bit_is_set(UCSR0A, UDRE0);

    // Write the byte to the register.
    UDR0 = c;
}

void uart_putstr(char *data) {
    // Loop until end of string writing char by char.
    while(*data){
      uart_putchar(*data++);
    }
}


const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

int radio_number = ROLE;
int role = ROLE;

int main(void) {

    uart_init();
    
//    pinMode(LED_BUILTIN, OUTPUT);
//    digitalWrite(LED_BUILTIN, LOW);

    RF24 radio(CE_PIN, CS_PIN);
    radio.begin();

    if(radio_number) {
        radio.openWritingPipe(pipes[1]);
        radio.openReadingPipe(1,pipes[0]);
    } else {
        radio.openWritingPipe(pipes[0]);
        radio.openReadingPipe(1,pipes[1]);
    }

    radio.startListening();

    

    int i = 0;
    for(;;) {
    
        unsigned long start_time = micros();
        // ping
        if (role == 1) {
            radio.stopListening();

//            uart_putstr("Now sending\n");
//            digitalWrite(LED_BUILTIN, HIGH);
//           _delay_ms(100);
//            digitalWrite(LED_BUILTIN, LOW);
            
            if (!radio.write( &start_time, sizeof(unsigned long))) {
//                uart_putstr("failed\n");
            }
        
        
            radio.startListening();
            unsigned long started_waiting_at = micros();
            boolean timeout = false;
            
            while (!radio.available()) {
                if (micros() - started_waiting_at > 20000) {
                    timeout = true;
                    break;
                }
            }
            
            if (timeout) { 
//                uart_putstr("Failed, reponse timed out\n");
            } else {
                unsigned long got_time;
                radio.read( & got_time, sizeof(unsigned long));
                unsigned long end_time = micros();
            
//                uart_putstr("Got response\n");
            
            }
        
            delay(500);
        
        } 
        
        // ping bag 
        if (role == 0) {
            unsigned long got_time;
            if (radio.available()) {
                while (radio.available()) {
                    radio.read( &got_time, sizeof(unsigned long));
                }
                radio.stopListening();
                radio.write( & got_time, sizeof(unsigned long));
                radio.startListening();
//                uart_putstr("Sent\n");
                digitalWrite(LED_BUILTIN, HIGH);
                _delay_ms(100);
                digitalWrite(LED_BUILTIN, LOW);


            }
        } 
    }
}
