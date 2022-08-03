/*  add message packet protocol on top of v0.2 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>

#include "commons.h"
#include "multimeter_click.h"
#include "spi.h"
#include "timers.h"
#include "uart.h"

#define BLINK_MS    1000

uint8_t buffer[20] = {0};

volatile uint32_t buzzer_pwm_pulse = 0;
volatile uint32_t tick = 0;
volatile uint8_t beeps = 0;
volatile uint8_t blinks = 0;
volatile uint8_t message;
volatile bool message_ready = false;
volatile bool beep_flag = false;

struct message_packet_t message_packet;

void init_system();
void enable_beep();
void enable_blink();
void test_relay_module();
void test_test_points();
void test_1();
void test_2();
void test_3();
void test_4();
void test_5();
void test_6();
void test_7();
void test_8();
void test_9();
void test_10();
void test_11();
void _abort();
void end();
// void transition_look_up(struct state_machine_t *state_machine, enum event_t event);

/* define all possible states */
enum state_t {
    _ENTRY,
    _TEST_1,
    _TEST_2,
    _TEST_3,
    _TEST_4,
    _TEST_5,
    _TEST_6,
    _TEST_7,
    _TEST_8,
    _TEST_9,
    _TEST_10,
    _TEST_11,
    _ABORT,
    _END
};

/* define all events */
enum event_t {
    test_start      =   0x30,
    test_abort      =   0x31,
    test_complete   =   0x32,
    test_next       =   0x33                        
};

/* define a row in state transition matrix */
struct state_transit_row_t {
    enum state_t current_state;
    enum event_t event;
    enum state_t next_state;
};

/* define a matrix of state transistion */
static struct state_transit_row_t state_transition_matrix[] = {
    {_ENTRY, test_start, _TEST_1},
    {_TEST_1, test_next, _TEST_2},
    {_TEST_2, test_next, _TEST_3},
    {_TEST_3, test_next, _TEST_4},
    {_TEST_4, test_next, _TEST_5},
    {_TEST_5, test_next, _TEST_6},
    {_TEST_6, test_next, _TEST_7},
    {_TEST_7, test_next, _TEST_8},
    {_TEST_8, test_next, _TEST_9},
    {_TEST_9, test_next, _TEST_10},
    {_TEST_10, test_next, _TEST_11},
    {_TEST_1, test_abort, _ABORT},
    {_TEST_2, test_abort, _ABORT},
    {_TEST_3, test_abort, _ABORT},
    {_TEST_4, test_abort, _ABORT},
    {_TEST_5, test_abort, _ABORT},
    {_TEST_6, test_abort, _ABORT},
    {_TEST_7, test_abort, _ABORT},
    {_TEST_8, test_abort, _ABORT},
    {_TEST_9, test_abort, _ABORT},
    {_TEST_10, test_abort, _ABORT},
    {_TEST_11, test_abort, _ABORT},
    {_TEST_11, test_next, _ABORT}
};

/* define a row in state function matrix */
struct state_function_row_t {
    const char* name;
    void (*func)(void);
};

/* define a matrix of state functions */
static struct state_function_row_t state_function_matrix[] = {
    {"POWER ON", test_1},
    {"GROUND", test_2},
    {"PILOT STATE A", test_3},
    {"PILOT STATE B", test_4},
    {"DIODE", test_5},
    {"OVER CURRENT", test_6},
    {"GFCI_L1_LOW_LEAKAGE", test_7},
    {"GFCI_L1_HIGH_LEAKAGE", test_8},
    {"GFCI_L2_LOW_LEAKAGE", test_9},
    {"GFCI_L2_HIGH_LEAKAGE", test_10},
    {"STUCK RELAY", test_11},
    {"STATE ABORT 1", _abort},
    {"STATE ABORT 2", _abort},
    {"STATE ABORT 3", _abort},
    {"STATE ABORT 4", _abort},
    {"STATE ABORT 5", _abort},
    {"STATE ABORT 6", _abort},
    {"STATE ABORT 7", _abort},
    {"STATE ABORT 8", _abort},
    {"STATE ABORT 9", _abort},
    {"STATE ABORT 10", _abort},
    {"STATE ABORT 11", _abort},
    {"THE END", _abort}
};

struct state_machine_t {
    enum state_t current_state;
};

void transition_look_up(struct state_machine_t* state_machine, enum event_t event) {
    for (uint8_t i=0;i<sizeof(state_transition_matrix)/sizeof(state_transition_matrix[0]);i++) {
        if (state_transition_matrix[i].current_state == state_machine->current_state) {
            if (state_transition_matrix[i].event == event) {
                uart0_puts(state_function_matrix[i].name);
                uart0_puts("\r\n");

                (state_function_matrix[i].func)();
                state_machine->current_state = state_transition_matrix[i].next_state;
                break;
            }
        }
    }
}

int main() {
    init_system();

    /* initialize state machine */
    struct state_machine_t state_machine; 
    state_machine.current_state = _ENTRY;

    enum event_t x;
    
    for (;;) {
        if (message_ready) {
            x = message;
            message_ready = false;

            transition_look_up(&state_machine, x);
            PORTB ^= (1 << PB7);                                        // toggling LED provides visual indication
            sleep_mode();                                               // put MCU to sleep after function returns
        }

        /* if look-up is placed here, it gets executed on every timer0 interrupt */
        // transition_look_up(&state_machine, x);
        // PORTB ^= (1 << PB7);                                        // toggling LED provides visual indication
        // sleep_mode();                                               // put MCU to sleep after function returns
    }
}

void init_system() {
    DDRB |= (1 << PB7);

    DDRE |= (1 << PE3) | (1 << PE4) | (1 << PE5);                   // D5/2/3
    PORTE |= (1 << PE3) | (1 << PE4) | (1 << PE5);                  

    DDRG |= (1 << PG5);                                             // D4
    PORTG |= (1 << PG5);

    DDRF = (1<<PF4) | (1<<PF5) | (1<<PF6) | (1<<PF7);               // data pins to PD3603A PSU
    
    RT67_ON
    enable_blink();
    uart0_init();
    init_timer0();
    // test_relay_module();

    set_sleep_mode(0);                                              // in Idle Mode, UART still runs

    sei();                                                          // enable global interrupt
}

void enable_beep() {
    // EICRA |= (1 << ISC01) | (1 << ISC00);                           // rising edge on INT0 generates an interrupt reques
    // EIMSK |= (1 << INT0);                                           // enable INT0(PD0/D21)

    EICRA |= (1 << ISC21) | (1 << ISC20);                           // rising edge on INT2 generates an interrupt reques
    EIMSK |= (1 << INT2);                                           // enable INT2(PD2/D19)
}

void enable_blink() {
    DDRH |= (1 << PH0) | (1 << PH1);                                // PH0: s3, PH1: s2
    DDRJ |= (1 << PJ0) | (1 << PJ1);                                // PJ0: s1, PJ1: s0

    PORTJ |= (1 << PJ0) | (1 << PJ1);                               // frequency scaling 100%
}

void test_1() {
    uart0_puts("test: POWER ON\t\t");

    DDRC |= (1 << relay_16[2].pin);
    _delay_ms(500);
    DDRC |= (1 << relay_16[0].pin) | (1 << relay_16[1].pin);

    // for (;;) {
    //     /* DEFAULT READ */
        
    //     PORTH |= (1 << PH0) | (1 << PH1);                           // G
    //     _delay_ms(5000);
    //     PORTH &= ~(1 << PH1);                                       // B
    //     _delay_ms(5000);
    //     PORTH &= ~(1 << PH0);                                       // R
    //     _delay_ms(5000);
    // }


    /* send test results at the end */
    // message_packet.number_beep = 0x5556;
    // message_packet.number_blink = 0x5354;
    // message_packet.output_volt = 0x5152;
    // message_packet.TP12 = 0x3940;
    // message_packet.TP19 = 0x3738;
    // message_packet.TP21 = 0x4950;
    // message_packet.TP22 = 0x4748;
    // message_packet.TP25 = 0x4546;
    // message_packet.TP26 = 0x4344;
    // message_packet.TP35 = 0x4142;

    // char* p = (char*)&message_packet;

    // for (uint8_t i=0;i<sizeof(message_packet);i++) {
    //     uart0_transmit(*p++);
    // }
    uart0_puts("exit\r\n");
}

void test_2() {
    uart0_puts("test: GROUND\t\t");

    PORTC |= (1 << relay_16[2].pin);

    _delay_ms(4000);

     

    PORTC |= (1 << relay_16[0].pin) | (1 << relay_16[1].pin);

    _delay_ms(1500);                                                // after relay opens, it take ~ 1.3 s for the board to emit beep, shoulde wait for the until the next test

    // message_packet.number_beep = 0x5556;
    // message_packet.number_blink = 0x5354;
    // message_packet.output_volt = 0x5152;
    // message_packet.TP21 = 0x4950;
    // message_packet.TP22 = 0x4748;
    // message_packet.TP25 = 0x4546;
    // message_packet.TP26 = 0x4344;
    // message_packet.TP35 = 0x4142;

    // char* p = (char*)&message_packet;

    // for (uint8_t i=0;i<sizeof(message_packet);i++) {
    //     uart0_transmit(*p++);
    // }
    uart0_puts("exit\r\n");
}

void test_3() {
    uart0_puts("test: PILOT STATE A\t");

    PORTC &= ~(1 << relay_16[2].pin);
    _delay_ms(500);
    PORTC &= ~((1 << relay_16[0].pin) | (1 << relay_16[1].pin));
    _delay_ms(2000);
    DDRK |= (1 << relay_16[10].pin);
    uart0_puts("exit\r\n");
}

void test_4() {
    uart0_puts("test: PILOT STATE B\t");

    DDRK |= (1 << relay_16[9].pin);

    // for (;;) {
    //     /* DEFAULT READ */
        
        // PORTH |= (1 << PH0) | (1 << PH1);                           // G
    //     _delay_ms(5000);
        PORTH |= (1 << PH0);                                       // B
    //     _delay_ms(5000);
    //     PORTH &= ~(1 << PH0);                                       // R
    //     _delay_ms(5000);
    // }

    uart0_puts("exit\r\n");
}

void test_5() {
    uart0_puts("test: DIODE\r\n\t");

    DDRK |= (1 << relay_16[11].pin);

    uint32_t t_0 = tick;                                            // record start time

    enable_beep();
    // enable_blink();

    ADCSRB |= (1 << ACME);

    /* blink and beep test */
    for (;;) {
        if (beep_flag) {
            beep_flag = false;

            if (tick - t_0 > 1300) {                                // there are a few relay clicks between 0 ~ 900 ms, 1300 ms a good start point to count beeps
                uart0_puts("beep\t");

                sprintf(buffer, "%u\r\n\t", tick-t_0);
                uart0_puts(buffer);
            }
        }
        if (tick - t_0 > 9000) {
            /* timeout */
            break;
        }
    }
    
    // sprintf(buffer, "%u\r\n", buzzer_pwm_pulse);
    // uart0_puts(buffer);

    PORTC |= (1 << relay_16[2].pin) | (1 << relay_16[0].pin) | (1 << relay_16[1].pin);
    PORTK |= (1 << relay_16[11].pin);

    uart0_puts("\t\t\texit\r\n");
}

void test_6() {
    PORTC &= ~(1 << relay_16[2].pin);
    _delay_ms(500);
    PORTC &= ~((1 << relay_16[0].pin) | (1 << relay_16[1].pin));

    // TODO: APPLY DC AT TP29 & TP30

    _delay_ms(11000);

    PORTC |= (1 << relay_16[2].pin) | (1 << relay_16[0].pin) | (1 << relay_16[1].pin);        
}

void test_7() {
    PORTC &= ~(1 << relay_16[2].pin);
    _delay_ms(500);
    PORTC &= ~((1 << relay_16[0].pin) | (1 << relay_16[1].pin));
    _delay_ms(2000);
    DDRC |= (1 << relay_16[5].pin);

    // TODO

    _delay_ms(11000);

    PORTC |= (1 << relay_16[2].pin) | (1 << relay_16[0].pin) | (1 << relay_16[1].pin)| (1 << relay_16[5].pin);
}

void test_8() {
    PORTC &= ~(1 << relay_16[2].pin);
    _delay_ms(500);
    PORTC &= ~((1 << relay_16[0].pin) | (1 << relay_16[1].pin));
    _delay_ms(2000);
    DDRK |= (1 << relay_16[6].pin);

    // TODO

    _delay_ms(11000);

    PORTC |= (1 << relay_16[2].pin) | (1 << relay_16[0].pin) | (1 << relay_16[1].pin);
    PORTK |= (1 << relay_16[6].pin);
}

void test_9() {
    PORTC &= ~(1 << relay_16[2].pin);
    _delay_ms(500);
    PORTC &= ~((1 << relay_16[0].pin) | (1 << relay_16[1].pin));
    _delay_ms(2000);
    DDRK |= (1 << relay_16[7].pin);

    // TODO

    _delay_ms(11000);

    PORTC |= (1 << relay_16[2].pin) | (1 << relay_16[0].pin) | (1 << relay_16[1].pin);
    PORTK |= (1 << relay_16[7].pin);
}

void test_10() {
    PORTC &= ~(1 << relay_16[2].pin);
    _delay_ms(500);
    PORTC &= ~((1 << relay_16[0].pin) | (1 << relay_16[1].pin));
    _delay_ms(2000);
    DDRK |= (1 << relay_16[8].pin);

    // TODO

    _delay_ms(11000);

    PORTC |= (1 << relay_16[2].pin) | (1 << relay_16[0].pin) | (1 << relay_16[1].pin);
    PORTK |= (1 << relay_16[8].pin);
}

void test_11() {
    PORTC &= ~(1 << relay_16[2].pin);
    _delay_ms(500);
    PORTC &= ~((1 << relay_16[0].pin) | (1 << relay_16[1].pin));
    _delay_ms(2000);
    DDRC |= (1 << relay_16[3].pin) | (1 << relay_16[4].pin);

    // TODO

    _delay_ms(11000);

    PORTC |= (1 << relay_16[2].pin) | (1 << relay_16[0].pin) | (1 << relay_16[1].pin) | (1 << relay_16[3].pin) | (1 << relay_16[4].pin);
}

void _abort() {
    uart0_puts("test aborted!\r\n");
    
    wdt_enable(WDTO_15MS);
    for (;;);
}

/* ISR */

ISR(TIMER0_COMPA_vect) {
    tick++;
}

// ISR(INT0_vect) {
//     // 
// }

ISR(INT2_vect) {
    beep_flag = true;
}

// ISR(TIMER0_OVF_vect) {
// }

ISR(USART0_RX_vect) {
    message = UDR0;
    message_ready = true;
}