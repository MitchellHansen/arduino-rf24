#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <util/setbaud.h>

#define LED             PORTB5
#define RELAY           PORTB0 

uint16_t adc_read(uint8_t adcx);
void uart_init();
void uart_putchar(char c);
void uart_putstr(char *data);

int main(void) {

    uart_init();
	
    /* Enable the ADC */
	ADCSRA |= _BV(ADEN);

	/* Set the LED pin as an output. */
    DDRB |= _BV(LED);
    DDRB |= _BV(RELAY);

    const int ring_buf_size = 20;
    uint16_t ring_buf[ring_buf_size];
    int index = 0;

    float average = 10;
    const int threshold = 50;

    for (;;_delay_ms(200)) {

        uint16_t pin_value = adc_read(0);

        if (index > ring_buf_size)
            index = 0;
        ring_buf[index] = pin_value;
        index++;

        average = 0;
        for (int i = 0; i < ring_buf_size; i++){
            average += ring_buf[i];
        }
        average /= ring_buf_size;

        // Print the value to serial
        char buff[10];
        itoa(pin_value, buff, 10);
        uart_putstr(buff);
        uart_putstr("\n");

        itoa(average, buff, 10);
        uart_putstr(buff);
        uart_putstr("\n");

//        PORTD |= (1 << WIRE);
//        PORTD &= (0 << WIRE);

        if (abs(pin_value - average) > threshold)
			PORTB |= _BV(LED);
		else
			PORTB &= ~_BV(LED);
        
    }
}

uint16_t adc_read(uint8_t adcx) {
	/* adcx is the analog pin we want to use.  ADMUX's first few bits are
	 * the binary representations of the numbers of the pins so we can
	 * just 'OR' the pin's number with ADMUX to select that pin.
	 * We first zero the four bits by setting ADMUX equal to its higher
	 * four bits. */
    ADMUX = _BV(REFS0) | adcx;
	/* This starts the conversion. */
	ADCSRA |= _BV(ADSC);

	/* This is an idle loop that just wait around until the conversion
	 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
	 * set above, to see if it is still set.  This bit is automatically
	 * reset (zeroed) when the conversion is ready so if we do this in
	 * a loop the loop will just go until the conversion is ready. */
	while ( (ADCSRA & _BV(ADSC)) );

	/* Finally, we return the converted value to the calling function. */
	return ADC;
}

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
