#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>
#include <util/setbaud.h>
#define LED PORTB5
#define WIRE PORTB0


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


int main(void) {
    
    uart_init();

    DDRB |= (1 << LED);
    DDRB |= (1 << WIRE);
    for(;;){
            

        uart_putstr("hello\n");
        
        _delay_ms(100);
        
        PORTB |= (1 << LED);
        PORTB |= (1 << WIRE);
        
        _delay_ms(100);
        
        PORTB &= (0 << LED);
        PORTB &= (0 << WIRE);
    
    }

}
