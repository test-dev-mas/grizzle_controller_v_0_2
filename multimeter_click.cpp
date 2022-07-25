#include "commons.h"
#include "multimeter_click.h"

void multimeter_init() {
    DDRD |= (1 << CS);
    PORTD |= (1 << CS);

    DDRB |= (1 << CLK) | (1 << D_IN);
    PORTB |= (1 << D_IN);


}

static uint16_t multimeter_read_channel(uint8_t channel) {
    uint8_t write_data;
    uint8_t read_buf[2];

    PORTD &= ~(1 << CS);                // bringing CS low initiates communication with MCP3204
    PORTB |= (1 << CLK);                // first clock received with CS low and DIN high will constitute a start bit
}