#include <avr/io.h>

#include "timers.h"

/* OVERFLOW MODE */
// void init_timer0() {
//     // TCCR0B |= (1 << CS02) | (1 << CS00);
//     TCCR0B |= (1 << CS01);
//     TIMSK0 |= (1 << TOIE0);
// }

/* CTC MODE */
void init_timer0() {
    TCCR0A |= (1 << WGM01);                     // CTC mode
    TCCR0B |= (1 << CS01) | (1 << CS00);        // f=clk_io/64
    TIMSK0 |= (1 << OCIE0A);                    // enable COMPARE MATCH A INTERRUPT
    OCR0A = 249;                                // yields 1ms interrupt
}