/*  add message packet protocol on top of v0.2 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdbool.h>

#include "commons.h"
#include "spi.h"
#include "timers.h"
#include "uart.h"

#define BLINK_MS    1000

volatile uint8_t message;
volatile bool message_ready = false;

void init_system();
void test_relay_module();
void test_test_points();
void idle();
void test();
void _abort();
void end();
// void transition_look_up(struct state_machine_t *state_machine, enum event_t event);

/* define all possible states */
enum state_t {
    _ENTRY,
    _TEST,
    _ABORT,
    _END
};

/* define all events */
enum event_t {
    test_start      =   0x30,
    test_abort      =   0x31,
    // test_complete   =   0x32,
    test_none       =   0x33                                        // enables an "automatic" transition
};

/* define a row in state transition matrix */
struct state_transit_row_t {
    enum state_t current_state;
    enum event_t event;
    enum state_t next_state;
};

/* define a matrix of state transistion */
static struct state_transit_row_t state_transition_matrix[] = {
    {_ENTRY, test_start, _TEST},
    {_TEST, test_abort, _ABORT},
    // {_TEST, test_complete, _END},
    {_ABORT, test_none, _END}
};

/* define a row in state function matrix */
struct state_function_row_t {
    const char* name;
    void (*func)(void);
};

/* define a matrix of state functions */
static struct state_function_row_t state_function_matrix[] = {
    {"STATE TEST", test},
    {"STATE ABORT", _abort},
    {"STATE END", end},
    {"STATE END", end}
};

struct state_machine_t {
    enum state_t current_state;
};

void transition_look_up(struct state_machine_t* state_machine, enum event_t event) {
    for (uint8_t i=0;i<sizeof(state_transition_matrix)/sizeof(state_transition_matrix[0]);i++) {
        if (state_transition_matrix[i].current_state == state_machine->current_state) {
            if (state_transition_matrix[i].event == event) {
                // uart0_puts(state_function_matrix[i].name);
                // uart0_puts("\t");

                (state_function_matrix[state_machine->current_state].func)();
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
        }

        transition_look_up(&state_machine, x);
        PORTB ^= (1 << PB7);                                        // toggling LED provides visual indication
        sleep_mode();                                               // put MCU to sleep after function returns
    }
}

void init_system() {
    DDRB |= (1 << PB7);

    DDRE |= (1 << PE3) | (1 << PE4) | (1 << PE5);                   // D5/2/3
    PORTE |= (1 << PE3) | (1 << PE4) | (1 << PE5);                  

    DDRG |= (1 << PG5);                                             // D4
    PORTG |= (1 << PG5);

    // DDRC |= (1 << RLY1) | (1 << RLY2) | (1 << RLY3) | (1 << RLY4) | (1 << RLY5) | (1 << RLY6);
    // DDRK |= (1 << RLY7) | (1 << RLY8) | (1 << RLY9) | (1 << RLY10) | (1 << RLY11) | (1 << RLY12); 

    uart0_init();
    // init_timer0();
    // test_relay_module();

    set_sleep_mode(0);                                              // in Idle Mode, UART still runs

    sei();
}

void test_relay_module() {
    for (uint8_t i=0;i<sizeof(relay_16)/sizeof(relay_16[0]);i++) {
        if (i<6) {
            DDRC |= (1 << relay_16[i].pin);
            _delay_ms(10);                                          // if commented out, relay has not time to react, looks as if off all time
            PORTC |= (1 << relay_16[i].pin);
            uart0_puts(relay_16[i].descriptor);
            uart0_puts("\r\n");
        }
        else {
            DDRK |= (1 << relay_16[i].pin);
            _delay_ms(10);                                          // if commented out, relay has not time to react, looks as if off all time
            PORTK |= (1 << relay_16[i].pin);
            uart0_puts(relay_16[i].descriptor);
            uart0_puts("\r\n");
        }
    }
}

void test_test_points() {
    for (uint8_t i=0;i<sizeof(test_points)/sizeof(test_points[0]);i++) {
        // if 
    }
}

void idle() {
}

void test() {
    // uart0_puts("inside test()\r\n");
    // TODO

    struct message_packet_t message_packet;

    message_packet.number_beep = 0x5556;
    message_packet.number_blink = 0x5354;
    message_packet.output_volt = 0x5152;
    message_packet.TP21 = 0x4950;
    message_packet.TP22 = 0x4748;
    message_packet.TP25 = 0x4546;
    message_packet.TP26 = 0x4344;
    message_packet.TP35 = 0x4142;
    
    char* p = (char*)&message_packet;

    for (uint8_t i=0;i<sizeof(message_packet);i++) {
        uart0_transmit(*p++);
    }
}

void _abort() {
    uart0_puts("inside abort()\r\n");
    // TODO
}

void end() {
    uart0_puts("inside end()\r\n");
    // TODO
}


ISR(TIMER0_OVF_vect) {
    /*  no need to put anything here
        provides 'tick' for transition look up iteration
    */
}

ISR(USART0_RX_vect) {
    message = UDR0;
    message_ready = true;
}