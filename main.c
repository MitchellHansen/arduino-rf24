#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>
#include <util/setbaud.h>

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



int main(void) {
    
    uart_init();

	DDRC &= ~(1<<0); // Input
	PORTC |= (1<<0); // Enable Internal Pull Up (Setting it to HIGH)

//    DDRD |= (1 << ENABLE);
    DDRD = DDRD | 0xFC;
   
    PORTD |= _BV(ENABLE);
    PORTD |= _BV(DIRECTION);
   
    for(;;){
            
//        uart_putstr("hello\n");
        _delay_ms(400);
        PORTD |= _BV(PULL);
        _delay_ms(400);
        PORTD &= ~_BV(PULL);
    
        uint8_t port_value = 2;
//        port_value = PINC & (1 << 0);
        uart_putchar((char)port_value);
//        uart_putchar((char)'\n');
    }

}
