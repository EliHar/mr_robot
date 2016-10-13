// interrupt is called when counter overflows

#define F_CPU 555555UL

#include <avr/io.h>
#include <avr/interrupt.h>

void initialize_timer1() {
    cli(); // disable global interrupts

    TIMSK1 = (1 << TOIE1); // enable Timer1 overflow interrupt

    /* Set clock
        CS12    CS11    CS10
        0       0       0       no clock source
        0       0       1       clk/1
        0       1       0       clk/8
        0       1       1       clk/64
        1       0       0       clk/256
        1       0       1       clk/1024
        1       1       0       clock on falling edge
        1       1       1       clock on rising edge
    */
    TCCR1B |= ((0 << CS12) | (1 << CS11) | (0 << CS10));

    sei(); // enable global interrupts:
}

// Interrupt Service Routine (interrupts when timer overflows)
ISR(TIMER1_OVF_vect) {
    cli(); // disable interrupts

    PORTB ^= (1 << PB1); // toggle led

    sei(); // re-enable interrupts
}

int main(void) {
    DDRB |= (1 << PB1); // set PB1 as output pin

    initialize_timer1();

    // loop()
    for (;;) {
        // can run code here that won't need to worry about delays
    }
}
